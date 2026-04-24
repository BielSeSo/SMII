#ifndef SOUNDMAKER_H
#define SOUNDMAKER_H

#include <AL/alut.h>

void intSoundsMenu(void);
void initSoundsGame(void);
void playMenuSound(int id);
void stopMenuSound(int id);
void playGameSound(int id);
void stopGameSound(int id);
void destroySoundsMenu(void);
void destroySoundsGame(void);
void playEngineSound();

#endif