
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <time.h>

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h" 

#include <GL/glut.h>
#include <AL/alut.h>

#include "actividad_2.h"


// ============ FUNCTION PROTOTYPE =============
void init(void);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void comprobateSound(void);
void comprobatePlayerPosition(void);
void updatePositionAmbulance(void);

// ========================================
//         Function for Activity 2
// ========================================
void playSource(int num);
void stopSource(int num);

static int wav_open(WavWriter *w, const char *path, int rate, int channels, int bits);
static void wav_write(WavWriter *w, const float *buf, int frames, int channels);
static void wav_close(WavWriter *w, int channels);

static void write_u16le(FILE *fp, uint16_t v);
static void write_u32le(FILE *fp, uint32_t v);

int initContexts(void);
int save_audio(void);

void cleanContextB(void);
void cleanContextA(void);

void DebugTerminal() {
    system("stty sane");
}

// ========= THREAD FUNCTION ===========
void* save_audio_thread(void* arg) {
    save_audio();
    return NULL;
}


// =====================================================
// MAIN
// =====================================================
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(400, 400);
    GLwin = glutCreateWindow("PIGE - OpenAL Example");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutReshapeFunc(reshape);

    glutMainLoop();

    DebugTerminal();

    return 0;
}

// =====================================================
// init()
// =====================================================
void init(void)
{
    alutInit(NULL, NULL);

    if(initContexts() == -1)
    {
        printf("Error creating context");
    }   

    alListenerfv(AL_POSITION, player.position);
    alListenerfv(AL_VELOCITY, player.velocity);
    alListenerfv(AL_ORIENTATION, player.listenerOrientation);

    alGenBuffers(NUM_SOURCES, buffer);

    printf("===========================================================\n");
    for (int i = 0; i < NUM_SOURCES; i++) {
        char fullPath[256];
        snprintf(fullPath, sizeof(fullPath), "%s%s", directoryMother, sourceRute[i]);
        printf("Cargando archivo: %s\n", fullPath);
        buffer[i] = alutCreateBufferFromFile(fullPath);
    }
    printf("===========================================================\n");

    // =================================
    //      Sources Contexto A
    // =================================
    alGenSources(NUM_SOURCES, source_out);

    for (int i = 0; i < NUM_SOURCES; i++) {
        alSourcef(source_out[i], AL_PITCH, 1.0f);
        alSourcef(source_out[i], AL_GAIN, 1.0f);
        alSourcefv(source_out[i], AL_POSITION, sourcePos[i]);
        alSourcefv(source_out[i], AL_VELOCITY, sourceVel[i]);
        alSourcei(source_out[i], AL_BUFFER, buffer[i]);
        alSourcei(source_out[i], AL_LOOPING, AL_TRUE);
    }

    // =====================================
    //          Sources Contexto B
    // =====================================
    alGenSources(NUM_SOURCES, source_lb);

    for (int i = 0; i < NUM_SOURCES; i++) {
        alSourcef(source_lb[i], AL_PITCH, 1.0f);
        alSourcef(source_lb[i], AL_GAIN, 1.0f);
        alSourcefv(source_lb[i], AL_POSITION, sourcePos[i]);
        alSourcefv(source_lb[i], AL_VELOCITY, sourceVel[i]);
        alSourcei(source_lb[i], AL_BUFFER, buffer[i]);
        alSourcei(source_lb[i], AL_LOOPING, AL_TRUE);
    }

    // Grabacion del audio
    pthread_create(&thread_audio, NULL, save_audio_thread, NULL);

    // Posiciones aleatorias
    srand(time(NULL));
    for (int i = 0; i < NUM_SOURCES-1; i++) {
        sourcePos[i][0] = -3.5f + (float)rand()/RAND_MAX * 7.0f;
        sourcePos[i][1] = 0.0f;
        sourcePos[i][2] = -5.0f + (float)rand()/RAND_MAX * -15.0f;
    }

    sourcePos[3][0] = 0.0f;
    sourcePos[3][1] = 0.0f;
    sourcePos[3][2] = -4.0f;
}

// =====================================================
// display()
// =====================================================
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glRotatef(20.0, 1.0, 1.0, 0.0);

    for(int i = 0; i < NUM_SOURCES; i++) {
        glPushMatrix();
        glTranslatef(sourcePos[i][0], sourcePos[i][1], sourcePos[i][2]);

        if(i == 0) glColor3f(1,0,0);
        else if(i == 1) glColor3f(0,1,0);
        else if(i == 2) glColor3f(0,0,1);
        else if(i == 3) glColor3f(1,0,1);

        glutWireCube(0.5);
        glPopMatrix();
    }

    updatePositionAmbulance();
    comprobatePlayerPosition();

    glPushMatrix();
    glTranslatef(player.position[0], player.position[1], player.position[2]);
    glColor3f(1,1,1);
    glutWireCube(0.5);
    glPopMatrix();

    glPopMatrix();
    glutSwapBuffers();
    glutPostRedisplay();
}

// =====================================================
// POSITION UPDATES
// =====================================================
void updatePositionAmbulance(void)
{
    sourcePos[3][0] += sourceVel[3][0];

    if (sourcePos[3][0] > 3.5f || sourcePos[3][0] < -3.5f) {
        sourceVel[3][0] = 0.0f;
    }
}

void comprobatePlayerPosition(void)
{
    player.position[2] += player.velocity[2];

    if (player.position[2] >= 4.0f) {
        player.position[2] = 4.0f;
        player.velocity[2] = 0.0f;
    }
    else if (player.position[2] <= -19.5f) {
        player.position[2] = -19.5f;
        player.velocity[2] = 0.0f;
    }

    player.position[0] += player.velocity[0];

    if (player.position[0] > 3.5f) {
        player.position[0] = 3.5f;
        player.velocity[0] = 0.0f;
    }
    else if (player.position[0] < -3.5f) {
        player.position[0] = -3.5f;
        player.velocity[0] = 0.0f;
    }
}

// =====================================================
// reshape()
// =====================================================
void reshape(int w, int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0,(float)w/h,1.0,30.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0,0.0,-6.6);
}

// =====================================================
// keyboard()
// =====================================================
void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 'h':
    case 'H':
        printf("==============================\n");
        printf("1 Play/Stop BasketBall_1\n2 Play/Stop BasketBall_2\n3 Public\n4 Ambulancia\n");
        printf("Mover: A,S,Q,Z o flechas\n");
        printf("==============================\n");
        break;

    case '1': isSound[0] = !isSound[0]; comprobateSound(); break;
    case '2': isSound[1] = !isSound[1]; comprobateSound(); break;
    case '3': isSound[2] = !isSound[2]; comprobateSound(); break;
    case '4': isSound[3] = !isSound[3]; comprobateSound(); break;

    // --- Movimiento ---
    case 'a':
    case 'A':
        player.velocity[0] -= deltaVel;
        alListenerfv(AL_POSITION, player.position);
        break;

    case 's':
    case 'S':
        player.velocity[0] += deltaVel;
        alListenerfv(AL_POSITION, player.position);
        break;

    case 'q':
    case 'Q':
        player.velocity[2] -= deltaVel;
        alListenerfv(AL_POSITION, player.position);
        break;

    case 'z':
    case 'Z':
        player.velocity[2] += deltaVel;
        alListenerfv(AL_POSITION, player.position);
        break;

    // --- Doppler Factor ---
    case 'd':
        dopplerFactor -= 0.1f;
        if (dopplerFactor < 0.1f) dopplerFactor = 0.1f;
        alDopplerFactor(dopplerFactor);
        break;

    case 'D':
        dopplerFactor += 0.1f;
        alDopplerFactor(dopplerFactor);
        break;

    // --- Doppler Velocity ---
    case 'e':
        dopplerVelocity -= 10.0f;
        if (dopplerVelocity < 10.0f) dopplerVelocity = 10.0f;
        alDopplerVelocity(dopplerVelocity);
        break;

    case 'E':
        dopplerVelocity += 10.0f;
        alDopplerVelocity(dopplerVelocity);
        break;

    // --- Velocidad ambulancia ---
    case 'v':
        sourceVel[3][0] -= deltaVel;
        alSourcefv(source_out[3], AL_VELOCITY, sourceVel[3]);
        alSourcefv(source_lb[3], AL_VELOCITY, sourceVel[3]);
        break;

    case 'V':
        sourceVel[3][0] += deltaVel;
        alSourcefv(source_out[3], AL_VELOCITY, sourceVel[3]);
        alSourcefv(source_lb[3], AL_VELOCITY, sourceVel[3]);
        break;

    // --- Pantalla completa ---
    case 'f':
    case 'F':
        if (!isFullscreen) {
            windowedWidth  = glutGet(GLUT_WINDOW_WIDTH);
            windowedHeight = glutGet(GLUT_WINDOW_HEIGHT);
            windowedPosX   = glutGet(GLUT_WINDOW_X);
            windowedPosY   = glutGet(GLUT_WINDOW_Y);
            glutFullScreen();
            isFullscreen = 1;
        } else {
            glutReshapeWindow(windowedWidth, windowedHeight);
            glutPositionWindow(windowedPosX, windowedPosY);
            isFullscreen = 0;
        }
        break;

    // --- Salir ---
    case 27:
        for (int i = 0; i < NUM_SOURCES; i++)
        {
            stopSource(i);
        }
        isExit = 1;

        pthread_join(thread_audio, NULL);

        glutDestroyWindow(GLwin);
        exit(0);
    }

    glutPostRedisplay();
}

// =====================================================
// specialKeys()
// =====================================================
void specialKeys(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_RIGHT:
        player.velocity[0] += deltaVel;
        break;

    case GLUT_KEY_LEFT:
        player.velocity[0] -= deltaVel;
        break;

    case GLUT_KEY_UP:
        player.velocity[2] -= deltaVel;
        break;

    case GLUT_KEY_DOWN:
        player.velocity[2] += deltaVel;
        break;
    }

    alListenerfv(AL_POSITION, player.position);
    glutPostRedisplay();
}

// =====================================================
// comprobateSound()
// =====================================================
void comprobateSound(void)
{
    if(isSound[0]) { playSource(0); printf("PLAY: BasketBall 1\n"); }
    else { stopSource(0); printf("STOP: BasketBall 1\n"); }

    if(isSound[1]) { playSource(1); printf("PLAY: BasketBall 2\n"); }
    else { stopSource(1); printf("STOP: BasketBall 2\n"); }

    if(isSound[2]) { playSource(2); printf("PLAY: Public.wav\n"); }
    else { stopSource(2); printf("STOP: Public.wav\n"); }

    if(isSound[3]) { playSource(3); printf("PLAY: Ambulance.wav\n"); }
    else { stopSource(3); printf("STOP: Ambulance.wav\n"); }

    printf("----------------------------------------\n");
}


// ============================================================
//                  Ampliacion Actividad 2
// ============================================================

void playSource(int num)
{
    alcMakeContextCurrent(ctx_out);
    alSourcePlay(source_out[num]);

    alcMakeContextCurrent(ctx_lb);
    alSourcePlay(source_lb[num]);
}

void stopSource(int num)
{
    alcMakeContextCurrent(ctx_out);
    alSourceStop(source_out[num]);

    alcMakeContextCurrent(ctx_lb);
    alSourceStop(source_lb[num]);
}

// =============================================================
//  SECCION 1 — Manejo de archivos WAV
// =============================================================

static int wav_open(WavWriter *w, const char *path, int rate, int channels, int bits)
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


// =============================================================
//  SECCION 2 — Utilidades y helpers
// =============================================================

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


// =============================================================
//  SECCION 3 — Preparacion de Contextos y Guardar audio
// =============================================================
int initContexts(void)
{
    const char *hrtf_name = NULL;

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
    printf("[Contexto A — Reproduccion en tiempo real]\n");

    dev_out = alcOpenDevice(NULL);
    if(!dev_out)
    {
        fprintf(stderr, "No se pudo abrir el dispositivo de salida.\n");
        return -1;
    }

    /* alcGetStringiSOFT y alcResetDeviceSOFT se cargan desde dev_out */
    #define LOAD_DEV(d,T,x) ((x) = FUNCTION_CAST(T, alcGetProcAddress((d), #x)))
    LOAD_DEV(dev_out, LPALCGETSTRINGISOFT,  alcGetStringiSOFT);
    LOAD_DEV(dev_out, LPALCRESETDEVICESOFT, alcResetDeviceSOFT);
    #undef LOAD_DEV

    ctx_out = alcCreateContext(dev_out, NULL);
    if(!ctx_out || alcMakeContextCurrent(ctx_out) == ALC_FALSE)
    {
        fprintf(stderr, "No se pudo crear el contexto de salida.\n");
        alcCloseDevice(dev_out);
        return -1;
    }

    /* ==================================================================
     * CONTEXTO B — dispositivo loopback (captura a WAV)
     * ================================================================== */
    printf("[Contexto B — Loopback → WAV]\n");

    ALCint lb_attrs[] = {
        ALC_FORMAT_CHANNELS_SOFT, ALC_STEREO_SOFT,
        ALC_FORMAT_TYPE_SOFT,     ALC_FLOAT_SOFT,
        ALC_FREQUENCY,            SAMPLE_RATE,
        ALC_HRTF_SOFT,            ALC_TRUE,
        0
    };

    dev_lb = alcLoopbackOpenDeviceSOFT(NULL);
    if(!dev_lb)
    {
        fprintf(stderr, "No se pudo abrir el dispositivo loopback.\n");
        cleanContextA();
        return -1;
    }

    if(alcIsRenderFormatSupportedSOFT(dev_lb, SAMPLE_RATE,
        ALC_STEREO_SOFT, ALC_FLOAT_SOFT) == ALC_FALSE)
    {
        fprintf(stderr, "Formato Stereo/Float32/%d Hz no soportado en loopback.\n", SAMPLE_RATE);
        alcCloseDevice(dev_lb);
        cleanContextA();
        return -1;
    }

    ctx_lb = alcCreateContext(dev_lb, lb_attrs);
    if(!ctx_lb || alcMakeContextCurrent(ctx_lb) == ALC_FALSE)
    {
        fprintf(stderr, "No se pudo crear el contexto loopback.\n");
        if(ctx_lb) alcDestroyContext(ctx_lb);
        alcCloseDevice(dev_lb);
        cleanContextA();
        return -1;
    }

    /* Solo intentar HRTF en dispositivo loopback si el contexto ya no lo activó */
    ALCint hrtf_state = 0;
    alcGetIntegerv(dev_lb, ALC_HRTF_SOFT, 1, &hrtf_state);
    if(hrtf_state)
    printf("HRTF activo en loopback: %s\n",alcGetString(dev_lb, ALC_HRTF_SPECIFIER_SOFT));
    else
    {
        printf("HRTF no activo en loopback\n");
        return -1;
    }

    /* Restaurar explícitamente ctx_lb como contexto activo antes de continuar */
    alcMakeContextCurrent(ctx_lb);
    return 0;
}

int save_audio(void)
{ 
    float duration = 0.0f;
    // ==================================================================
    //                   Abrir WAV y buffer de chunk
    // ================================================================== 
    WavWriter wav;
    if(!wav_open(&wav, saveRute, SAMPLE_RATE, NUM_CHANNELS, 32))
    {
        cleanContextB();
        return -1;
    }

    chunk = malloc(sizeof(float) * CHUNK_FRAMES * NUM_CHANNELS);
    int frames_now = CHUNK_FRAMES;

    printf("Grabando a tiempo real en '%s'\n", saveRute);
    printf("Pulsa Esc para detener.\n");
    printf("===========================================================\n");
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

    /* El contexto activo al entrar al bucle es ctx_lb */
    while (isExit == 0)
    {
        //  1. CONTEXTO B (loopback) → capturar al WAV 
        alcMakeContextCurrent(ctx_lb);
        alcProcessContext(ctx_lb);

        alcRenderSamplesSOFT(dev_lb, chunk, frames_now);
        wav_write(&wav, chunk, frames_now, NUM_CHANNELS);

        //  2. CONTEXTO A (salida real) → reproducir sonido 
        alcMakeContextCurrent(ctx_out);
        alcProcessContext(ctx_out);


        //  3. Pausa entre iteraciones
        usleep(10000); // 10 ms aprox
        duration += 0.01f;
    }
    printf("Deteniendo grabacion.\n");
   
    free(chunk);
    wav_close(&wav, NUM_CHANNELS);

    cleanContextA();
    cleanContextB();
    alutExit();
   
    printf("WAV guardado: %s  (%.1f s, %d Hz, estereo Float32)\n", saveRute, duration, SAMPLE_RATE);

    return 0;
}

void cleanContextB(void)
{
    /* Liberar contexto loopback */
    alcMakeContextCurrent(ctx_lb);
    for(int i=0; i<NUM_SOURCES; i++)
    {
        alSourceStop(source_lb[i]);
    }
    alDeleteSources(NUM_SOURCES, source_out);
    /* shared_buffer NO se borra aqui; su propietario es ctx_out */
    alcMakeContextCurrent(NULL);
    alcDestroyContext(ctx_lb);
    alcCloseDevice(dev_lb);   
}

void cleanContextA(void)
{
    /* Liberar contexto de salida (propietario del buffer) */
    alcMakeContextCurrent(ctx_out);
    for(int i=0; i<NUM_SOURCES; i++)
    {
        alSourceStop(source_out[i]);
    }
    alDeleteSources(NUM_SOURCES, source_lb);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(ctx_out);
    alcCloseDevice(dev_out);
}