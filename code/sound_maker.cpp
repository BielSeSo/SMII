#include <AL/alut.h>

#include "sound_maker.h"

#define NUM_SOUNDS_MENU 4
#define NUM_SOUNDS_GAME 6

using namespace std;

// ========= GLOBAL VARIABLES =========== //
ALuint buffersMenu[NUM_SOUNDS_MENU];
ALuint buffersGame[NUM_SOUNDS_GAME];

ALuint sourcesMenu[NUM_SOUNDS_MENU];
ALuint soucesGame[NUM_SOUNDS_GAME];

const char *routesBuffersMenu[NUM_SOUNDS_MENU] = 
            {"sources/audios/Intro.wav",
             "sources/audios/musicaFondoMenu.wav",
             "sources/audios/Pop.wav",
             "sources/audios/Back.wav"};

const char *routesBuffersGame[NUM_SOUNDS_GAME] = 
            {"sources/audios/Engine.wav",
             "sources/audios/musicaPista1.wav",
             "sources/audios/Frenar.wav",
             "sources/audios/Horn1.wav",
             "sources/audios/Horn2.wav",
            "sources/audios/Warning.wav"};

// ============ FUNCTIONS =============== //
void intSoundsMenu(void)
{
    alGenBuffers(NUM_SOUNDS_MENU, buffersMenu);
    for(int i=0; i<NUM_SOUNDS_MENU; i++)
    {
        buffersMenu[i] = alutCreateBufferFromFile(routesBuffersMenu[i]);
    }
    
    alGenSources(NUM_SOUNDS_MENU, sourcesMenu);

    // Loop sound
    for (int i=0; i<2; i++) {
        alSourcei(sourcesMenu[i], AL_BUFFER, buffersMenu[i]);
        alSourcei(sourcesMenu[i], AL_LOOPING, AL_TRUE);
    }

    // Not loop sound
    for (int i=2; i<NUM_SOUNDS_MENU; i++) {
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

    // Loop sound
    alGenSources(NUM_SOUNDS_GAME, soucesGame);
    for (int i=0; i<2; i++) {
        alSourcei(soucesGame[i], AL_BUFFER, buffersGame[i]);
        alSourcei(soucesGame[i], AL_LOOPING, AL_TRUE);
    }

    // Not loop sound
    for (int i=2; i<NUM_SOUNDS_GAME; i++) {
        alSourcei(soucesGame[i], AL_BUFFER, buffersGame[i]);
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
    alSourceStop(soucesGame[id]);
}

void destroySoundsMenu(void)
{
    for(int i=0; i<NUM_SOUNDS_MENU; i++)
    {
        alSourceStop(sourcesMenu[i]);
    }
    alDeleteSources(NUM_SOUNDS_MENU, sourcesMenu);
}

void destroySoundsGame(void)
{
    for(int i=0; i<NUM_SOUNDS_GAME; i++)
    {
        alSourceStop(soucesGame[i]);
    }
    alDeleteSources(NUM_SOUNDS_GAME, soucesGame);
}