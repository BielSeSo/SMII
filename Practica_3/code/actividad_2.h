#ifndef A2_H
#define A2_H
#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h" 

#include <pthread.h>

// ============== DEFINITIONS ================
#define NUM_SOURCES 4
#define NUM_ENVIRONMENTS 1

#define FUNCTION_CAST(T, ptr) (T)(ptr)

#define SAMPLE_RATE      44100
#define NUM_CHANNELS     2                      /* HRTF siempre produce estéreo    */
#define BYTES_PER_SAMPLE 4                      /* float32                         */
#define CHUNK_FRAMES     (SAMPLE_RATE / 100)    /* ~10 ms por iteración */

/* ==================================================
*           STRUCTS & VARIABLES from Activity 1
* ===================================================*/

// ======= STRUCTS ==========
typedef struct {
    ALfloat position[3];
    ALfloat velocity[3];
    ALfloat listenerOrientation[6];
} Player;

Player player = {
    {0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0},
    {0.0, 0.0, -1.0, 0.0, 1.0, 0.0}
};

// ====== GLOBAL VARIABLES =======
ALfloat sourcePos[NUM_SOURCES][3];
ALfloat sourceVel[NUM_SOURCES][3] = {
    {0.0,0.0,0.0},
    {0.0,0.0,0.0},
    {0.0,0.0,0.0},
    {0.0,0.0,0.0}
};

ALuint buffer[NUM_SOURCES];

char directoryMother[256] = "code/audios/actividad_1/";

char sourceRute[NUM_SOURCES][128] = {
    "BasketBall.wav",
    "BasketBall.wav",
    "Public.wav",
    "Ambulance.wav"
};

int GLwin;
int isSound[NUM_SOURCES] = {0};
float deltaVel = 0.001f;
float dopplerFactor = 1.0f, dopplerVelocity = 343.0f;

int windowedWidth = 400, windowedHeight = 400;
int windowedPosX = 100, windowedPosY = 100;
int isFullscreen = 0;

/* ==================================================
*      STRUCTS & VARIABLES used for Activity 2
* ===================================================*/

// ======= STRUCTS ==========
static struct termios g_old_term;
typedef struct {
    FILE    *fp;
    uint32_t data_bytes;
} WavWriter;

// ====== GLOBAL VARIABLES =======
int isExit = 0;
ALuint source_out[NUM_SOURCES];
ALuint source_lb[NUM_SOURCES];

static LPALCLOOPBACKOPENDEVICESOFT      alcLoopbackOpenDeviceSOFT;
static LPALCISRENDERFORMATSUPPORTEDSOFT alcIsRenderFormatSupportedSOFT;
static LPALCRENDERSAMPLESSOFT           alcRenderSamplesSOFT;
static LPALCGETSTRINGISOFT               alcGetStringiSOFT;
static LPALCRESETDEVICESOFT             alcResetDeviceSOFT;

static int g_raw = 0;
const char *saveRute = "bin/A2_audio/openal_escena.wav";

ALCdevice *dev_out, *dev_lb;
ALCcontext  *ctx_out, *ctx_lb; 

const float *chunk;

// Hilo para paralelismo de grabacion
pthread_t thread_audio;

#endif
