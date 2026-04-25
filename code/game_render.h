#ifndef GAME_RENDER_H
#define GAME_RENDER_H

typedef struct Vec3
{
    float x, y, z;
};

typedef struct Coordinates4f
{
    float x1, x2;
    float y1, y2; 
};

typedef struct Hitbox
{
    float w, h;
};

void init(void);
void startWindow(void);
void drawMenu(void (*reshape)(int, int), bool *isGame, int &marioWin);
void drawMapMenu(void);
void drawKartMenu(void);
void renderGame(void (*reshape)(int, int), bool *isGame);
void comprobateLimits(void);
bool comprobateTrackLimits(void);
void loadGame(void);
void selectButton(void);
void closeGame(int &marioWin);

// KEYBOARD FUNCS
void keyW(void);
void keyS(void);
void keyA(void);
void keyD(void);
void key1(void);
void key2(void);
void key3(void);

// MOUSE FUNCS
void rightClick(void);
void leftClick(float glX, float glY);
void mouseFunc(float glX, float glY);

#endif
