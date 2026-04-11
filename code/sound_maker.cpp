#include <string>

#include <AL/alut.h>

#include "sound_maker.h"

using namespace std;

// ========= GLOBAL VARIABLES =========== //
ALuint buffersMenu[NUM_SOUNDS_MENU];
ALuint buffersGame[NUM_SOUNDS_GAME];

ALuint sourcesMenu[NUM_SOUNDS_MENU];
ALuint soucesGame[NUM_SOUNDS_GAME];

const char *routesBuffersMenu[NUM_SOUNDS_MENU] = {"sources/audios/Clic.wav"};
const char *routesBuffersGame[NUM_SOUNDS_GAME] = {"sources/audios/Broom.wav", \
                                                  "sources/audios/chillido_rueda.wav"};

// ============ FUNCTIONS =============== //
void intSoundsMenu(void)
{
    alGenBuffers(NUM_SOUNDS_MENU, buffersMenu);
    for(int i=0; i<NUM_SOUNDS_MENU; i++)
    {
        buffersMenu[i] = alutCreateBufferFromFile(routesBuffersMenu[i]);
    }

    alGenSources(NUM_SOUNDS_MENU, sourcesMenu);
    for (int i = 0; i < NUM_SOUNDS_MENU; i++) {
        alSourcei(sourcesMenu[i], AL_BUFFER, buffersMenu[i]);
    }
}

void initSoundsGame(void)
{
    alGenBuffers(NUM_SOUNDS_GAME, buffersGame);
    for(int i=0; i<NUM_SOUNDS_GAME; i++)
    {
        buffersGame[i] = alutCreateBufferFromFile(routesBuffersGame[i]);
    }

    alGenSources(NUM_SOUNDS_GAME, soucesGame);
    for (int i = 0; i < NUM_SOUNDS_GAME; i++) {
        alSourcei(soucesGame[i], AL_BUFFER, buffersGame[i]);
        alSourcei(soucesGame[i], AL_LOOPING, AL_TRUE);
    }
}

void playMenuSound(int id)
{
    alSourcePlay(sourcesMenu[id]);
}

void stopMenuSound(int id)
{
    alSourceStop(sourcesMenu[id]);
}

void playGameSound(int id)
{
    alSourcePlay(soucesGame[id]);
}

void stopGameSound(int id)
{
    alSourcePlay(soucesGame[id]);
}