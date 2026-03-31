/*
 * (C) The OpenAL Utility Toolkit (ALUT) v1.1
 *
 * Compilar:
 * gcc helloWorld_ALUT.c  -lalut -lopenal -o helloWorld_ALUT
 *
 * Es interesante tener a mano la documentación sobre :
  'The OpenAL Utility Toolkit', 'OpenAL 1.1 Specification' y 'OpenAL_Programmers_Guide'

 */

#include <stdlib.h>
#include <AL/alut.h>
 
int main (int argc, char **argv)
{
  ALuint helloBuffer, helloSource;

  alutInit (&argc, argv);

  helloBuffer = alutCreateBufferHelloWorld ();
  alGenSources (1, &helloSource);
  alSourcei (helloSource, AL_BUFFER, helloBuffer);
  alSourcePlay (helloSource);
  alutSleep (1);

  alDeleteSources(1, &helloSource);
  alDeleteBuffers(1, &helloBuffer);
  alutExit ();
  return EXIT_SUCCESS;
}
