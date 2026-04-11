#ifndef SOUNDMAKER_H
#define SOUNDMAKER_H

#include <AL/alut.h>

#define NUM_SOUNDS_MENU 1
#define NUM_SOUNDS_GAME 2

void intSoundsMenu(void);
void initSoundsGame(void);
void playMenuSound(int id);
void stopMenuSound(int id);
void playGameSound(int id);
void stopGameSound(int id);

#endif