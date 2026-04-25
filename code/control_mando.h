#ifndef CONTROL_MANDO_H
#define CONTROL_MANDO_H

#include <atomic>

#define calibFile "code/calibracion_camara/calibracion_camara.yml"

using namespace std;

void girarDireccion(int angulo);
void acelerar();
void frenar();

void procesarControlMando(string url, atomic<bool> &evento);

#endif