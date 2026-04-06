/*
 * OpenAL HRTF: reproducción en tiempo real + captura loopback a WAV
 *
 * Fusión de los ejemplos alloopback.c y alhrtf.c de openal-soft.
 * Fuentes originales:
 *   https://github.com/kcat/openal-soft/blob/master/examples/alloopback.c
 *   https://github.com/kcat/openal-soft/blob/master/examples/alhrtf.c
 *
 * Arquitectura de dos contextos:
 *
 *   ┌─────────────────────────────────┐   ┌──────────────────────────────────┐
 *   │  Contexto A — dispositivo real  │   │  Contexto B — loopback           │
 *   │  (tarjeta de sonido del SO)     │   │  (ALC_SOFT_loopback)             │
 *   │  HRTF activo → se escucha       │   │  HRTF activo → WAV               │
 *   │  fuente 3D orbitando            │   │  alcRenderSamplesSOFT → archivo  │
 *   └─────────────────────────────────┘   └──────────────────────────────────┘
 *                     ▲                                    ▲
 *                     └──────── mismo ALuint buffer ───────┘
 *                               fuente independiente en cada contexto,
 *                               posición actualizada en sync en el bucle
 *
 * Compilación (Linux):
 *   gcc al_hrtf_loopback_wav.c -o al_hrtf_loopback_wav -lopenal -lm
 *
 * Compilación (Windows/MinGW):
 *   gcc al_hrtf_loopback_wav.c -o al_hrtf_loopback_wav -lOpenAL32 -lm
 *
 * Uso:
 *   ./al_hrtf_loopback_wav [salida.wav] [-hrtf <nombre>] [-dur <segundos>]
 *
 * Pulsa cualquier tecla para detener. El WAV queda válido aunque se corte antes.
 */

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h" 

const char *saveRute = "bin/E18_audio/hrtf_output.wav";

// DEBUG TERMINAL
void DebugTerminal(void) {int value = system("stty sane"); (void)value;}

/* FUNCTION_CAST: workaround para castear void* a puntero a función en C99.
 * Los ejemplos de OpenAL Soft la definen localmente porque no está en alext.h.
 * Fuente: https://github.com/kcat/openal-soft/blob/master/utils/openal-info.c
 * está en alhelpers
 */

#define FUNCTION_CAST(T, ptr) (T)(ptr)

/* ------------------------------------------------------------------ */
/* Punteros a funciones de extensiones                                  */
/* ------------------------------------------------------------------ */
static LPALCLOOPBACKOPENDEVICESOFT      alcLoopbackOpenDeviceSOFT;
static LPALCISRENDERFORMATSUPPORTEDSOFT alcIsRenderFormatSupportedSOFT;
static LPALCRENDERSAMPLESSOFT           alcRenderSamplesSOFT;
static LPALCGETSTRINGISOFT              alcGetStringiSOFT;
static LPALCRESETDEVICESOFT             alcResetDeviceSOFT;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ------------------------------------------------------------------ */
/* Parámetros de audio                                                  */
/* ------------------------------------------------------------------ */
#define SAMPLE_RATE      44100
#define NUM_CHANNELS     2          /* HRTF siempre produce estéreo    */
#define BYTES_PER_SAMPLE 4          /* float32                         */
#define CHUNK_FRAMES     (SAMPLE_RATE / 100)   /* ~10 ms por iteración */

/* ------------------------------------------------------------------ */
/* Escritura de archivo WAVE IEEE Float32, sin dependencias externas    */
/* ------------------------------------------------------------------ */
typedef struct {
    FILE    *fp;
    uint32_t data_bytes;
} WavWriter;

static void write_u16le(FILE *fp, uint16_t v)
{
    uint8_t b[2] = { (uint8_t)(v & 0xFF), (uint8_t)(v >> 8) };
    fwrite(b, 1, 2, fp);
}

static void write_u32le(FILE *fp, uint32_t v)
{
    uint8_t b[4] = {
        (uint8_t)( v        & 0xFF),
        (uint8_t)((v >>  8) & 0xFF),
        (uint8_t)((v >> 16) & 0xFF),
        (uint8_t)((v >> 24) & 0xFF)
    };
    fwrite(b, 1, 4, fp);
}

static int wav_open(WavWriter *w, const char *path,
                    int rate, int channels, int bits)
{
    w->fp = fopen(path, "wb");
    if(!w->fp) { fprintf(stderr, "No se pudo crear '%s'\n", path); return 0; }
    w->data_bytes = 0;

    fwrite("RIFF", 1, 4, w->fp);  write_u32le(w->fp, 0);   /* parchear al cerrar */
    fwrite("WAVE", 1, 4, w->fp);

    /* fmt — tipo 3 = IEEE Float */
    fwrite("fmt ", 1, 4, w->fp);  write_u32le(w->fp, 18);
    write_u16le(w->fp, 3);
    write_u16le(w->fp, (uint16_t)channels);
    write_u32le(w->fp, (uint32_t)rate);
    write_u32le(w->fp, (uint32_t)(rate * channels * (bits / 8)));
    write_u16le(w->fp, (uint16_t)(channels * (bits / 8)));
    write_u16le(w->fp, (uint16_t)bits);
    write_u16le(w->fp, 0);   /* cbSize */

    /* fact — obligatorio para formatos no-PCM */
    fwrite("fact", 1, 4, w->fp);  write_u32le(w->fp, 4);
    write_u32le(w->fp, 0);        /* parchear */

    /* data */
    fwrite("data", 1, 4, w->fp);  write_u32le(w->fp, 0);   /* parchear */
    return 1;
}

static void wav_write(WavWriter *w, const float *buf, int frames, int channels)
{
    size_t n = (size_t)(frames * channels);
    fwrite(buf, sizeof(float), n, w->fp);
    w->data_bytes += (uint32_t)(n * sizeof(float));
}

// static void wav_close(WavWriter *w, int channels)
// {
//     /*
//      * Offsets de la cabecera generada por wav_open():
//      *  4  ChunkSize
//      * 46  fact.dwSampleLength
//      * 50  data chunk size
//      */
//     uint32_t total_samples = w->data_bytes /
//     (uint32_t)(sizeof(float) * (size_t)channels);
//     uint32_t riff_size = 4 + (4+4+18) + (4+4+4) + (4+4) + w->data_bytes;
//
//     fseek(w->fp,  4, SEEK_SET);  write_u32le(w->fp, riff_size);
//     fseek(w->fp, 46, SEEK_SET);  write_u32le(w->fp, total_samples);
//     fseek(w->fp, 50, SEEK_SET);  write_u32le(w->fp, w->data_bytes);
//     fclose(w->fp);
//     w->fp = NULL;
// }
static void wav_close(WavWriter *w, int channels)
{
    uint32_t total_samples = w->data_bytes /
    (uint32_t)(sizeof(float) * (size_t)channels);

    fseek(w->fp, 0, SEEK_END);
    uint32_t riff_size = (uint32_t)(ftell(w->fp) - 8);

    fseek(w->fp,  4, SEEK_SET);  write_u32le(w->fp, riff_size);
    fseek(w->fp, 46, SEEK_SET);  write_u32le(w->fp, total_samples);
    fseek(w->fp, 54, SEEK_SET);  write_u32le(w->fp, w->data_bytes);
    fclose(w->fp);
    w->fp = NULL;
}

/* ------------------------------------------------------------------ */
/* Buffer de audio de prueba: sine 440 Hz, mono 16-bit, 10 s           */
/* ------------------------------------------------------------------ */
/*
 * alBufferData() copia los datos PCM al driver. El ALuint resultante
 * es un identificador compartible entre contextos del mismo proceso:
 * ambos contextos leerán la misma copia inmutable.
 */
static ALuint create_sine_buffer(void)
{
    const int  n    = SAMPLE_RATE * 10;
    ALshort   *data = malloc((size_t)n * sizeof(ALshort));
    ALuint     buf  = 0;
    ALenum     err;

    for(int i = 0; i < n; i++)
        data[i] = (ALshort)(sin(i / (double)SAMPLE_RATE * 440.0 * 2.0 * M_PI) * 32767.0);

    alGenBuffers(1, &buf);
    alBufferData(buf, AL_FORMAT_MONO16, data, n * (int)sizeof(ALshort), SAMPLE_RATE);
    free(data);

    err = alGetError();
    if(err != AL_NO_ERROR)
    {
        fprintf(stderr, "Error creando buffer: %s\n", alGetString(err));
        if(alIsBuffer(buf)) alDeleteBuffers(1, &buf);
        return 0;
    }
    return buf;
}

/* ------------------------------------------------------------------ */
/* Activación de HRTF en un device ya abierto                           */
/* ------------------------------------------------------------------ */
static int enable_hrtf(ALCdevice *dev, const char *hrtf_name)
{
    ALCint num_hrtf = 0;

    ALCcontext *ctx_prev = alcGetCurrentContext();  /* guardar contexto actual */

    alcGetIntegerv(dev, ALC_NUM_HRTF_SPECIFIERS_SOFT, 1, &num_hrtf);
    printf("  HRTFs disponibles: %d\n", num_hrtf);
    for(int i = 0; i < num_hrtf; i++)
        printf("    %d: %s\n", i, alcGetStringiSOFT(dev, ALC_HRTF_SPECIFIER_SOFT, i));

    ALCint hrtf_index = -1;
    if(hrtf_name && num_hrtf > 0)
    {
        for(int i = 0; i < num_hrtf; i++)
        {
            if(strcmp(alcGetStringiSOFT(dev, ALC_HRTF_SPECIFIER_SOFT, i), hrtf_name) == 0)
            { hrtf_index = i; break; }
        }
        if(hrtf_index < 0)
            printf("  HRTF '%s' no encontrado, usando predeterminado.\n", hrtf_name);
    }

    ALCint a[5]; int i = 0;
    a[i++] = ALC_HRTF_SOFT;  a[i++] = ALC_TRUE;
    if(hrtf_index >= 0) { a[i++] = ALC_HRTF_ID_SOFT; a[i++] = hrtf_index; }
    a[i] = 0;

    // if(!alcResetDeviceSOFT(dev, a))
    //     fprintf(stderr, "  Advertencia: alcResetDeviceSOFT falló: %s\n",
    //             alcGetString(dev, alcGetError(dev)));
    if(!alcResetDeviceSOFT(dev, a))  {
        fprintf(stderr, "  Advertencia: alcResetDeviceSOFT falló: %s\n",
                alcGetString(dev, alcGetError(dev)));
        /* sin HRTF el programa no tiene sentido, salimos limpiamente */
        alcCloseDevice(dev);
        return EXIT_FAILURE;
    }

    ALCint state = 0;
    alcGetIntegerv(dev, ALC_HRTF_SOFT, 1, &state);
    if(state)
        printf("  HRTF activo: %s\n", alcGetString(dev, ALC_HRTF_SPECIFIER_SOFT));
    else
        printf("  Advertencia: HRTF no se activó.\n");

    alcMakeContextCurrent(ctx_prev);  /* restaurar siempre al salir */
    return 0;
}

/* ------------------------------------------------------------------ */
/* Teclado sin bloqueo — multiplataforma                                */
/* ------------------------------------------------------------------ */

#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <time.h>
static struct termios g_old_term;
static int            g_raw = 0;
static void term_raw(void)
{
    struct termios r;
    tcgetattr(STDIN_FILENO, &g_old_term);
    r = g_old_term;
    r.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    r.c_cc[VMIN] = 0; r.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &r);
    g_raw = 1;
}
static void sleep_10ms(void)
{
    struct timespec ts = {0, 10000000};
    nanosleep(&ts, NULL);
}
static int key_pressed(void)
{
    fd_set fds; struct timeval tv = {0,0};
    FD_ZERO(&fds); FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO+1, &fds, NULL, NULL, &tv) > 0;
}

/* ------------------------------------------------------------------ */
/* main                                                                 */
/* ------------------------------------------------------------------ */
int main(int argc, char *argv[])
{ 
    const char *hrtf_name = NULL;
    double      duration  = 5.0;

    for(int i = 1; i < argc; i++)
    {
        if     (strcmp(argv[i], "-hrtf")==0 && i+1<argc) hrtf_name = argv[++i];
        else if(strcmp(argv[i], "-dur") ==0 && i+1<argc) duration  = atof(argv[++i]);
        else if(argv[i][0] != '-') saveRute = argv[i];
    }

    /* ---- Verificar extensión loopback ---- */
    if(!alcIsExtensionPresent(NULL, "ALC_SOFT_loopback"))
    {
        fprintf(stderr, "Error: ALC_SOFT_loopback no disponible.\n");
        return 1;
    }

    /* ---- Cargar funciones de extensión (desde NULL device) ---- */
    #define LOAD(T,x) ((x) = FUNCTION_CAST(T, alcGetProcAddress(NULL, #x)))
    LOAD(LPALCLOOPBACKOPENDEVICESOFT,      alcLoopbackOpenDeviceSOFT);
    LOAD(LPALCISRENDERFORMATSUPPORTEDSOFT, alcIsRenderFormatSupportedSOFT);
    LOAD(LPALCRENDERSAMPLESSOFT,           alcRenderSamplesSOFT);
    #undef LOAD

    /* ==================================================================
     * CONTEXTO A — dispositivo de salida real (tarjeta de sonido del SO)
     * ================================================================== */
    printf("[Contexto A — reproducción en tiempo real]\n");

    ALCdevice *dev_out = alcOpenDevice(NULL);
    if(!dev_out)
    {
        fprintf(stderr, "No se pudo abrir el dispositivo de salida.\n");
        return 1;
    }

    /* alcGetStringiSOFT y alcResetDeviceSOFT se cargan desde dev_out */
    #define LOAD_DEV(d,T,x) ((x) = FUNCTION_CAST(T, alcGetProcAddress((d), #x)))
    LOAD_DEV(dev_out, LPALCGETSTRINGISOFT,  alcGetStringiSOFT);
    LOAD_DEV(dev_out, LPALCRESETDEVICESOFT, alcResetDeviceSOFT);
    #undef LOAD_DEV

    ALCcontext *ctx_out = alcCreateContext(dev_out, NULL);
    if(!ctx_out || alcMakeContextCurrent(ctx_out) == ALC_FALSE)
    {
        fprintf(stderr, "No se pudo crear el contexto de salida.\n");
        alcCloseDevice(dev_out);
        return 1;
    }

    if(alcIsExtensionPresent(dev_out, "ALC_SOFT_HRTF"))
        enable_hrtf(dev_out, hrtf_name);
    else
        printf("  ALC_SOFT_HRTF no disponible en el dispositivo de salida.\n");

    /*
     * El buffer se crea mientras ctx_out es el contexto activo.
     * Los datos PCM quedan en el driver; el ALuint es reutilizable
     * en otros contextos del mismo proceso.
     */
    ALuint shared_buffer = create_sine_buffer();
    if(!shared_buffer)
    {
        alcDestroyContext(ctx_out);
        alcCloseDevice(dev_out);
        return 1;
    }

    /* Fuente 3D — Contexto A */
    ALuint src_out = 0;
    alGenSources(1, &src_out);
    alSourcei(src_out, AL_SOURCE_RELATIVE, AL_FALSE);
    alSource3f(src_out, AL_POSITION, 0.0f, 0.0f, -1.0f);
    alSourcei(src_out, AL_BUFFER,  (ALint)shared_buffer);
    alSourcei(src_out, AL_LOOPING, AL_TRUE);
    assert(alGetError() == AL_NO_ERROR && "Error configurando fuente A");

    /* ==================================================================
     * CONTEXTO B — dispositivo loopback (captura a WAV)
     * ================================================================== */
    printf("[Contexto B — loopback → WAV]\n");

    ALCint lb_attrs[] = {
        ALC_FORMAT_CHANNELS_SOFT, ALC_STEREO_SOFT,
        ALC_FORMAT_TYPE_SOFT,     ALC_FLOAT_SOFT,
        ALC_FREQUENCY,            SAMPLE_RATE,
        ALC_HRTF_SOFT,            ALC_TRUE,
        0
    };


    ALCdevice *dev_lb = alcLoopbackOpenDeviceSOFT(NULL);
    if(!dev_lb)
    {
        fprintf(stderr, "No se pudo abrir el dispositivo loopback.\n");
        goto cleanup_A;
    }

    if(alcIsRenderFormatSupportedSOFT(dev_lb, SAMPLE_RATE,
        ALC_STEREO_SOFT, ALC_FLOAT_SOFT) == ALC_FALSE)
    {
        fprintf(stderr, "Formato Stereo/Float32/%d Hz no soportado en loopback.\n", SAMPLE_RATE);
        alcCloseDevice(dev_lb);
        goto cleanup_A;
    }

    // #define LOAD_DEV(d,T,x) ((x) = FUNCTION_CAST(T, alcGetProcAddress((d), #x)))
    // LOAD_DEV(dev_lb, LPALCGETSTRINGISOFT,  alcGetStringiSOFT);
    // LOAD_DEV(dev_lb, LPALCRESETDEVICESOFT, alcResetDeviceSOFT);
    // #undef LOAD_DEV

    ALCcontext *ctx_lb = alcCreateContext(dev_lb, lb_attrs);
    if(!ctx_lb || alcMakeContextCurrent(ctx_lb) == ALC_FALSE)
    {
        fprintf(stderr, "No se pudo crear el contexto loopback.\n");
        if(ctx_lb) alcDestroyContext(ctx_lb);
        alcCloseDevice(dev_lb);
        goto cleanup_A;
    }

    // if(alcIsExtensionPresent(dev_lb, "ALC_SOFT_HRTF"))
    //     enable_hrtf(dev_lb, hrtf_name);
    // else
    //     printf("  ALC_SOFT_HRTF no disponible en loopback (salida estéreo sin HRTF).\n");


    /* Solo intentar HRTF en dispositivo loopback si el contexto ya no lo activó */
    ALCint hrtf_state = 0;
    alcGetIntegerv(dev_lb, ALC_HRTF_SOFT, 1, &hrtf_state);
    if(hrtf_state)
        printf("  HRTF activo en loopback: %s\n",
               alcGetString(dev_lb, ALC_HRTF_SPECIFIER_SOFT));
        else
        {
            printf("  HRTF no activo en loopback, intentando reset...\n");
            if(alcIsExtensionPresent(dev_lb, "ALC_SOFT_HRTF"))
                enable_hrtf(dev_lb, hrtf_name);  /* con restauración de contexto */
        }

        /* Restaurar explícitamente ctx_lb como contexto activo antes de continuar */
        alcMakeContextCurrent(ctx_lb);

    // /* Verificar que el contexto loopback está realmente activo */
    // ALCcontext *ctx_actual = alcGetCurrentContext();
    // printf("  ctx_lb=%p  ctx_actual=%p  iguales=%s\n",
    //        (void*)ctx_lb, (void*)ctx_actual,
    //        ctx_actual == ctx_lb ? "SI" : "NO");
    //
    // ALCdevice *dev_actual = alcGetContextsDevice(ctx_actual);
    // printf("  dev_lb=%p  dev_actual=%p  iguales=%s\n",
    //        (void*)dev_lb, (void*)dev_actual,
    //        dev_actual == dev_lb ? "SI" : "NO");


    // alGenBuffers(1, &buf_lb);
    // alBufferData(buf_lb, AL_FORMAT_MONO16, pcm_data, pcm_size, SAMPLE_RATE);
    // assert(alGetError() == AL_NO_ERROR && "Error creando buffer loopback");
    ALuint buf_lb = create_sine_buffer();
    if(!buf_lb)
    {
        alcDestroyContext(ctx_lb);
        alcCloseDevice(dev_lb);
        return 1;
    }

    /* Fuente 3D — Contexto B (loopback) */
    alGetError();
    ALuint src_lb = 0;
    alGenSources(1, &src_lb);
    // assert(alGetError() == AL_NO_ERROR && "Error 1");
    alSourcei(src_lb, AL_SOURCE_RELATIVE, AL_FALSE);
    // assert(alGetError() == AL_NO_ERROR && "Error 2");
    alSource3f(src_lb, AL_POSITION, 0.0f, 0.0f, -1.0f);
    // assert(alGetError() == AL_NO_ERROR && "Error 3");
    alSourcei(src_lb, AL_BUFFER,  (ALint)shared_buffer); // buf_lb);
    assert(alGetError() == AL_NO_ERROR && "Error 4");
    alSourcei(src_lb, AL_LOOPING, AL_TRUE);
    assert(alGetError() == AL_NO_ERROR && "Error configurando fuente B");

    /* ==================================================================
     * Abrir WAV y buffer de chunk
     * ================================================================== */
    WavWriter wav;
    if(!wav_open(&wav, saveRute, SAMPLE_RATE, NUM_CHANNELS, 32))
        goto cleanup_B;

    float *chunk = malloc((size_t)(CHUNK_FRAMES * NUM_CHANNELS * BYTES_PER_SAMPLE));

    /* ==================================================================
     * Arrancar ambas fuentes lo más en sync posible
     * ================================================================== */
    alcMakeContextCurrent(ctx_out);
    alSourcePlay(src_out);

    alcMakeContextCurrent(ctx_lb);
    alSourcePlay(src_lb);

    
    term_raw();
    printf("\nReproduciendo en tiempo real Y grabando en '%s'\n", saveRute);
    printf("Duración máxima: %.1f s — pulsa cualquier tecla para detener.\n\n", duration);
    fflush(stdout);

    /* ==================================================================
     * Bucle principal
     *
     * Orden de operaciones por iteración:
     *   1. Actualizar posición en loopback (ctx_lb) y capturar chunk
     *   2. Escribir chunk al WAV
     *   3. Cambiar a ctx_out y actualizar la misma posición
     *   4. Volver a ctx_lb para la próxima iteración
     * ================================================================== */
    int    total_frames  = (int)(SAMPLE_RATE * duration);
    int    rendered      = 0;
    double angle         = 0.0;
    int    stopped_early = 0;
    double var           = 0.0;

    /* El contexto activo al entrar al bucle es ctx_lb */
    while(rendered < total_frames)
    {
        if(key_pressed()) { stopped_early = 1; break; }

        int frames_now = CHUNK_FRAMES;
        if(rendered + frames_now > total_frames)
            frames_now = total_frames - rendered;

        /* --- Avanzar ángulo de órbita (~90°/s) --- */
        angle += 0.01 * M_PI * 0.5;
        if(angle > M_PI) angle -= M_PI * 2.0;

        /* --- Contexto B (loopback): actualizar posición y renderizar --- */
        alcSuspendContext(ctx_lb);
        alSource3f(src_lb, AL_POSITION,
                   (ALfloat)sin(angle), 0.0f, -(ALfloat)cos(angle));
        alcProcessContext(ctx_lb);

        /*
         * alcRenderSamplesSOFT():
         *   - Mezcla todas las fuentes activas del ctx_lb
         *   - Aplica HRTF (si está activo)
         *   - Deposita frames_now frames estéreo Float32 en 'chunk'
         * Es equivalente al callback RenderSDLSamples() del ejemplo
         * alloopback.c, pero aquí lo llamamos manualmente.
         */
        alcRenderSamplesSOFT(dev_lb, chunk, frames_now);
        wav_write(&wav, chunk, frames_now, NUM_CHANNELS);

        /* --- Contexto A (salida real): misma posición → misma espacialización --- */
        alcMakeContextCurrent(ctx_out);
        alcSuspendContext(ctx_out);
        alSource3f(src_out, AL_POSITION,
                   (ALfloat)sin(angle), 0.0f, -(ALfloat)cos(angle));
        alcProcessContext(ctx_out);

        /* Volver al contexto del loopback para la siguiente iteración */
        alcMakeContextCurrent(ctx_lb);

        rendered += frames_now;
        var = (double)(rendered / SAMPLE_RATE);

        printf("\r  %.1f / %.1f s  (ángulo: %+.0f°)   ",
               var, duration,
               angle * 180.0 / M_PI);
        fflush(stdout);

        sleep_10ms();
    }

    if(stopped_early) printf("\nDetenido por el usuario.\n");
    else              printf("\nFin de la duración.\n");

    /* ==================================================================
     * Limpieza ordenada
     * ================================================================== */
    free(chunk);
    wav_close(&wav, NUM_CHANNELS);

    cleanup_B:
    /* Liberar contexto loopback */
    alcMakeContextCurrent(ctx_lb);
    alSourceStop(src_lb);
    alDeleteSources(1, &src_lb);
    /* shared_buffer NO se borra aquí; su propietario es ctx_out */
    alcMakeContextCurrent(NULL);
    alcDestroyContext(ctx_lb);
    alcCloseDevice(dev_lb);

    cleanup_A:
    /* Liberar contexto de salida (propietario del buffer) */
    alcMakeContextCurrent(ctx_out);
    alSourceStop(src_out);
    alDeleteSources(1, &src_out);
    alDeleteBuffers(1, &shared_buffer);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(ctx_out);
    alcCloseDevice(dev_out);

    printf("WAV guardado: %s  (%.1f s, %d Hz, estéreo Float32)\n",
           saveRute, var, SAMPLE_RATE);

    DebugTerminal();
    return 0;
}
