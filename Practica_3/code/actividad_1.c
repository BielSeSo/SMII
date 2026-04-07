#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <GL/glut.h>
#include <AL/alut.h>

// ==== STRUCTS EN LUGAR DE CLASE ====
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

#define NUM_SOURCES 4
#define NUM_ENVIRONMENTS 1

// === VARIABLES GLOBALES ===
ALfloat sourcePos[NUM_SOURCES][3];
ALfloat sourceVel[NUM_SOURCES][3] = {
    {0.0,0.0,0.0},
    {0.0,0.0,0.0},
    {0.0,0.0,0.0},
    {0.0,0.0,0.0}
};

ALuint buffer[NUM_SOURCES];
ALuint source[NUM_SOURCES];

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

// === PROTOTIPOS ===
void init(void);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void comprobateSound(void);
void comprobatePlayerPosition(void);
void updatePositionAmbulance(void);

void DebugTerminal() {
    system("stty sane");
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

    alDeleteBuffers(NUM_SOURCES, buffer);
    alutExit();
    DebugTerminal();

    return 0;
}

// =====================================================
// init()
// =====================================================
void init(void)
{
    alutInit(NULL, NULL);

    alListenerfv(AL_POSITION, player.position);
    alListenerfv(AL_VELOCITY, player.velocity);
    alListenerfv(AL_ORIENTATION, player.listenerOrientation);

    alGenBuffers(NUM_SOURCES, buffer);

    for (int i = 0; i < NUM_SOURCES; i++) {
        char fullPath[256];
        snprintf(fullPath, sizeof(fullPath), "%s%s", directoryMother, sourceRute[i]);
        printf("Cargando archivo: %s\n", fullPath);
        buffer[i] = alutCreateBufferFromFile(fullPath);
    }

    alGenSources(NUM_SOURCES, source);

    for (int i = 0; i < NUM_SOURCES; i++) {
        alSourcef(source[i], AL_PITCH, 1.0f);
        alSourcef(source[i], AL_GAIN, 1.0f);
        alSourcefv(source[i], AL_POSITION, sourcePos[i]);
        alSourcefv(source[i], AL_VELOCITY, sourceVel[i]);
        alSourcei(source[i], AL_BUFFER, buffer[i]);
        alSourcei(source[i], AL_LOOPING, AL_TRUE);
    }

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
        printf("1 Play/Stop BasketBall_1\n2 Play/Stop BasketBall_2\n3 Public\n4 Ambulancia\n");
        printf("Mover: A,S,Q,Z o flechas\n");
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
        alSourcefv(source[3], AL_VELOCITY, sourceVel[3]);
        break;

    case 'V':
        sourceVel[3][0] += deltaVel;
        alSourcefv(source[3], AL_VELOCITY, sourceVel[3]);
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
            alSourceStop(source[i]);

        alutExit();
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
    if(isSound[0]) { alSourcePlay(source[0]); printf("PLAY: BasketBall 1\n"); }
    else { alSourceStop(source[0]); printf("STOP: BasketBall 1\n"); }

    if(isSound[1]) { alSourcePlay(source[1]); printf("PLAY: BasketBall 2\n"); }
    else { alSourceStop(source[1]); printf("STOP: BasketBall 2\n"); }

    if(isSound[2]) { alSourcePlay(source[2]); printf("PLAY: Public.wav\n"); }
    else { alSourceStop(source[2]); printf("STOP: Public.wav\n"); }

    if(isSound[3]) { alSourcePlay(source[3]); printf("PLAY: Ambulance.wav\n"); }
    else { alSourceStop(source[3]); printf("STOP: Ambulance.wav\n"); }

    printf("----------------------------------------\n");
}
