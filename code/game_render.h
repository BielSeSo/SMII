#ifndef GAME_RENDER_H
#define GAME_RENDER_H

void init(void);
void startWindow(void);
void drawMenu(bool *needReshape, bool *isGame, int &marioWin);
void renderGame(bool *needReshape, bool *isGame);
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
