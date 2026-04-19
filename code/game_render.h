#ifndef GAME_RENDER_H
#define GAME_RENDER_H

typedef struct Hitbox
{
    float w, h; // Width & Height
};

void init(void);
void startWindow(void);
void drawMenu(void (*reshape)(int, int), bool *isGame, int &marioWin);
void renderGame(void (*reshape)(int, int), bool *isGame);
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

#endif
