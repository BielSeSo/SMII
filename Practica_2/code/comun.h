#ifndef COMUN_H
#define COMUN_H


// Índices para los planos de una imagen de color y de grises

#define R 2
#define G 1
#define B 0

#define GRIS 0


// Valores lógicos
#define OR ||
#define AND &&
#define TRUE 1
#define FALSE 0
#define BOOL int

// Para mantener la compatibilidad con el código de Learning OpenCV
//#define true TRUE
//#define false FALSE
#define CIERTO TRUE
#define FALSO FALSE
//#define bool BOOL

// Máximo valor de gris o de un componente de una imagen
#define MAXPIXEL (double)255


// La tecla ESC
#define ESC 27



#define DEFOURCC_C1(valor)  (int)((valor)&255)
#define DEFOURCC_C2(valor)  (int)((valor>>8)&255)
#define DEFOURCC_C3(valor)  (int)((valor>>16)&255)
#define DEFOURCC_C4(valor)  (int)((valor>>24)&255)


#include <assert.h> // assert 

#ifndef NDEBUG
#define comprovaCondicio( condicio, cadenaCondicio ) fprintf(stderr, "%s (%d): %s\n", __FILE__, __LINE__, cadenaCondicio ); \
                                                     assert( (condicio) );
#else
#undef comprovaCondicio
#endif


#endif
