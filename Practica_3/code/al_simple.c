/*
 * OpenAL — reproducción simple, sin HRTF, sin loopback, sin WAV
 *
 * Versión reducida de al_hrtf_loopback_wav.c para comparación.
 * Un único contexto sobre el dispositivo de salida real.
 * Fuente 3D orbitando con sine 440 Hz durante 'duration' segundos.
 *
 * Compilación (Linux):
 *   gcc al_simple.c -o al_simple -lopenal -lm
 *
 * Compilación (Windows/MinGW):
 *   gcc al_simple.c -o al_simple -lOpenAL32 -lm
 *
 * Uso:
 *   ./al_simple [-dur <segundos>]
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "AL/al.h"
#include "AL/alc.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ------------------------------------------------------------------ */
/* Parámetros de audio                                                  */
/* ------------------------------------------------------------------ */
#define SAMPLE_RATE 44100

/* ------------------------------------------------------------------ */
/* Buffer de audio de prueba: sine 440 Hz, mono 16-bit, 10 s           */
/* ------------------------------------------------------------------ */
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
/* sleep multiplataforma — equivalente a sleep_10ms() del original     */
/* ------------------------------------------------------------------ */
static void sleep_10ms(void)
{
#ifdef _WIN32
    Sleep(10);
#else
    struct timespec ts = {0, 10000000};
    nanosleep(&ts, NULL);
#endif
}

/* ------------------------------------------------------------------ */
/* main                                                                 */
/* ------------------------------------------------------------------ */
int main(int argc, char *argv[])
{
    double duration = 10.0;

    for(int i = 1; i < argc; i++)
        if(strcmp(argv[i], "-dur") == 0 && i+1 < argc)
            duration = atof(argv[++i]);

    /* ---- Abrir dispositivo y crear contexto ---- */
    ALCdevice *dev = alcOpenDevice(NULL);
    if(!dev)
    {
        fprintf(stderr, "No se pudo abrir el dispositivo de salida.\n");
        return 1;
    }

    ALCcontext *ctx = alcCreateContext(dev, NULL);
    if(!ctx || alcMakeContextCurrent(ctx) == ALC_FALSE)
    {
        fprintf(stderr, "No se pudo crear el contexto.\n");
        alcCloseDevice(dev);
        return 1;
    }

    /* ---- Crear buffer y fuente 3D ---- */
    ALuint buf = create_sine_buffer();
    if(!buf)
    {
        alcDestroyContext(ctx);
        alcCloseDevice(dev);
        return 1;
    }

    ALuint src = 0;
    alGenSources(1, &src);
    alSourcei(src, AL_SOURCE_RELATIVE, AL_FALSE);
    alSource3f(src, AL_POSITION, 0.0f, 0.0f, -1.0f);
    alSourcei(src, AL_BUFFER,  (ALint)buf);
    alSourcei(src, AL_LOOPING, AL_TRUE);

    /* ---- Bucle de reproducción con órbita ---- */
    int    total_frames = (int)(SAMPLE_RATE * duration);
    int    rendered     = 0;
    double angle        = 0.0;

    alSourcePlay(src);

    printf("Reproduciendo %.1f s (sin HRTF)...\n", duration);
    fflush(stdout);

    while(rendered < total_frames)
    {
        angle += 0.01 * M_PI * 0.5;
        if(angle > M_PI) angle -= M_PI * 2.0;

        alSource3f(src, AL_POSITION,
                   (ALfloat)sin(angle), 0.0f, -(ALfloat)cos(angle));

        sleep_10ms();
        rendered += SAMPLE_RATE / 100;   /* ~10 ms de frames por iteración */

        printf("\r  %.1f / %.1f s  (ángulo: %+.0f°)   ",
               rendered / (double)SAMPLE_RATE, duration,
               angle * 180.0 / M_PI);
        fflush(stdout);
    }
    printf("\nFin.\n");

    /* ---- Limpieza ---- */
    alSourceStop(src);
    alDeleteSources(1, &src);
    alDeleteBuffers(1, &buf);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(ctx);
    alcCloseDevice(dev);

    return 0;
}
