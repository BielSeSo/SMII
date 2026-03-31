
#include <stdio.h>
#include <stdlib.h>
#include <AL/alut.h>     


int main(int argc, char **argv)
{
    char c;
    ALuint buffers[7], fuente;

    // Inicializar ALUT
    alutInit(NULL, NULL);

    // Crear buffers de sonido
    buffers[0] = alutCreateBufferWaveform(ALUT_WAVEFORM_SINE, 262.0, 0.0, 1.0);
    buffers[1] = alutCreateBufferWaveform(ALUT_WAVEFORM_SINE, 294.0, 0.0, 1.0);
    buffers[2] = alutCreateBufferWaveform(ALUT_WAVEFORM_SINE, 330.0, 0.0, 1.0);
    buffers[3] = alutCreateBufferWaveform(ALUT_WAVEFORM_SINE, 349.0, 0.0, 1.0);
    buffers[4] = alutCreateBufferWaveform(ALUT_WAVEFORM_SINE, 392.0, 0.0, 1.0);
    buffers[5] = alutCreateBufferWaveform(ALUT_WAVEFORM_SINE, 440.0, 0.0, 1.0);
    buffers[6] = alutCreateBufferWaveform(ALUT_WAVEFORM_SINE, 494.0, 0.0, 1.0);

    alGenSources(1, &fuente);

    printf("Pulsa 1,2,3,4,5,6,7 para sonidos. Pulsa q para salir.\n");

    do {
      c = getchar();

      switch (c) {
      case '1': alSourcei(fuente, AL_BUFFER, buffers[0]); break;
      case '2': alSourcei(fuente, AL_BUFFER, buffers[1]); break;
      case '3': alSourcei(fuente, AL_BUFFER, buffers[2]); break;
      case '4': alSourcei(fuente, AL_BUFFER, buffers[3]); break;
      case '5': alSourcei(fuente, AL_BUFFER, buffers[4]); break;
      case '6': alSourcei(fuente, AL_BUFFER, buffers[5]); break;
      case '7': alSourcei(fuente, AL_BUFFER, buffers[6]); break;
      default: break;
      }

      if (c != 'q')
      {
        alSourcePlay(fuente);
        alutSleep(1);
      }

    } while ((c != 'q') && (c != 'Q'));
    printf("Usuario saliendo\n");

    alDeleteBuffers(6, buffers);
    alutExit();

    return 0;
}