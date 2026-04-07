/*
 * Exemple de reproducci� en streaming de fitxers WAVE mitjan�ant libsndfile
 * 
 * gcc openal_stream_wave.c -o openal_stream_wave -lalut `pkg-config openal --cflags --libs` -lsndfile && openal_stream_wave escala.wav
 * 
 * Refer�ncies
 *** http://www.mega-nerd.com/libsndfile/
 *** https://github.com/libsndfile/libsndfile/
 **** https://github.com/kcat/openal-soft/blob/master/examples/alstream.c
 *  IMPT.: Lliges en sf_readf_short en #mostres i en alBufferData indiques en #bytes
 */
#include <stdio.h>
#include <stdlib.h> //EXIT_SUCCESS
#include <math.h>
//#include <AL/al.h>
//#include <AL/alc.h>  // Ja els porten darrere en alut.h
#include <AL/alut.h>

#include <sndfile.h>
#include <string.h> //memset

/* Include this header file to use functions from libsndfile. */
#include	<sndfile.h>


/*	This will be the length of the buffer used to hold.frames while
 * *	we process them.
 */
#define		BUFFER_LEN	2*1024
// ALURE gasta 8192
/* Define the number of buffers and buffer size (in milliseconds) to use. 4
 * buffers with 8192 samples each gives a nice per-chunk size, and lets the
 * queue last for almost one second at 44.1khz. */
//#define NUM_BUFFERS 4
//#define BUFFER_SAMPLES 8192

/* libsndfile can handle more than 6 channels but we'll restrict it to 1 (artxius monof�nics). */
#define		MAX_CHANNELS	2


/* This is a buffer of double precision floating point values --> byte? short! per la forma de treball de la crida de libsndfile
 * which will hold our data while we process it.
 */
short *data; 

/* A SNDFILE is very much like a FILE in the Standard C library. The
 * * sf_open function return an SNDFILE* pointer when they sucessfully
 ** open the specified file.
 */
SNDFILE	*infile; //, *outfile ;

/* A pointer to an SF_INFO struct is passed to sf_open.
 * * On read, the library fills this struct with information about the file.
 ** On write, the struct must be filled in before calling sf_open.
 */
SF_INFO		sfinfo ;
int			readcount ;
char	infilename[128] = "escala.wav" ;


int main(int argc, char *argv[])
{
    ALenum error;
    ALint buffersLibres;
    ALuint bufferID;
    sf_count_t samples = 0, mostresLLegides = 0, numTotalMostres = 0;
    ALuint alsource;
    ALenum formatoAudio; //mono o stereo (1 o 2 canales)
    int numCanales, bitsMuestra, frecuenciaMuestreo;
    size_t frame_size;
    ALsizei mostresEnBytes;
    ALint queued;
    
    #define NUM_BUFFERS 3
    ALuint buffers[NUM_BUFFERS];
    
    char barraActivitat[4] = {'|', '/', '-', '\\'};
    ALint estado;


    
    // The SF_INFO struct must be initialized before using it.
    memset (&sfinfo, 0, sizeof (sfinfo)) ;
    
    if( argc > 1) {
        strcpy(infilename, argv[1]);
    }
    
    if (!alutInit(&argc, argv))
    {
        printf("Fallo al iniciar OpenAL.\n");
        return 0;
    }
    
    
    printf ("La versio de OpenAL instalada es %s \n\n Las diferentes extensiones de OpenAL son: %s \n\n\n", alGetString(AL_VERSION), alGetString(AL_EXTENSIONS));
    printf ("La versio de ALUT instalada es %d.%d \n", alutGetMajorVersion(), alutGetMinorVersion());
    printf ("La versio de libsndfile instalada es %s \n", sf_version_string());
    
    //Generar buffers y fuentes de openAL (1 fuente)
    alGenBuffers(NUM_BUFFERS, buffers);
    if((error=alGetError()) != AL_NO_ERROR)
        printf("Error generando buffers\n");
    alGenSources(1, &alsource);
    if((error=alGetError()) != AL_NO_ERROR)
        printf("Error generando la fuente: %s\n", alutGetErrorString(error));
    
    
    /* Here's where we open the input file. We pass sf_open the file name and
     * * a pointer to an SF_INFO struct.
     ** On successful open, sf_open returns a SNDFILE* pointer which is used
     ** for all subsequent operations on that file.
     ** If an error occurs during sf_open, the function returns a NULL pointer.
     **
     ** If you are trying to open a raw headerless file you will need to set the
     ** format and channels fields of sfinfo before calling sf_open(). For
     ** instance to open a raw 16 bit stereo PCM file you would need the following
     ** two lines:
     **
     **		sfinfo.format   = SF_FORMAT_RAW | SF_FORMAT_PCM_16 ;
     **		sfinfo.channels = 2 ;
     */
    if (!(infile = sf_open(infilename, SFM_READ, &sfinfo)) )
    {	/* Open failed so print an error message. */
        printf ("Not able to open input file %s.\n", infilename) ;
        /* Print the error message from libsndfile. */
        puts (sf_strerror (NULL)) ;
        return 1 ;
    } ;
    
    if (sfinfo.channels > MAX_CHANNELS)
    {	
        printf ("%s t� %d canals? No anem a processar arxius en m�s de %d canals\n", 
                infilename, sfinfo.channels, MAX_CHANNELS) ;
                sf_close (infile) ;
                return 1 ;
    }
    
    numTotalMostres = sfinfo.frames;
    
    printf("SNDINFO: %s:: canals %d, freqMostreig %d, \format %0xd [%s, %s], total (%ld mostres %ld bytes) i temps (%04f segons o %02ld : %02ld minuts:segons; frames %ld)\n",
           argv[1],
           sfinfo.channels,
           sfinfo.samplerate, 
           sfinfo.format, 
           ((sfinfo.format & SF_FORMAT_WAV) > 0? "WAVE": "no WAVE"),   
           ((sfinfo.format & SF_FORMAT_PCM_16) > 0? "16bits" : 
           ((sfinfo.format & SF_FORMAT_PCM_S8) > 0? "8bits" : "")),
           numTotalMostres, //sfinfo.frames, 
           (sfinfo.frames * sfinfo.channels * ((sfinfo.format & SF_FORMAT_PCM_16) > 0? 2 : 1)),
           (float)sfinfo.frames / (float)sfinfo.samplerate,
           (sfinfo.frames  / sfinfo.samplerate) / 60,
           (sfinfo.frames  / sfinfo.samplerate) % 60,
           (sfinfo.frames * sfinfo.channels)
    );
    
    
    // https://libsndfile.github.io/libsndfile/api.html#read
    // The SF_INFO structure is for passing data between the calling function and the library when opening a file for reading or writing. It is defined in sndfile.h as follows:
    // typedef struct
    // {       sf_count_t  frames ;     /* Used to be called samples. */
    //         int         samplerate ;
    //         int         channels ;
    //         int         format ;
    //         int         sections ;
    //         int         seekable ;
    //     } SF_INFO ;
    // Pero en File Read Functions diu: For the frames-count functions, the frames parameter specifies the number of frames. A frame is just a block of samples, one for each channel.
    
    numCanales = sfinfo.channels;
    bitsMuestra = ((sfinfo.format & SF_FORMAT_PCM_16) > 0? 16 : 
    ((sfinfo.format & SF_FORMAT_PCM_S8) > 0? 8 : 0));
    if (bitsMuestra == 0)
        printf("Error en el fichero %s: bitsMuestra = %d\n", infilename, bitsMuestra);
    
    frecuenciaMuestreo = sfinfo.samplerate;
    
    //Asignamos el tipo de formato para pasarselo a openAL dependiendo del n�mero de canales y los bits de muestra
    formatoAudio = 0;
    // Get the sound format, and figure out the OpenAL format
    if(sfinfo.channels == 1)
        formatoAudio = AL_FORMAT_MONO16;
    else
        if(sfinfo.channels == 2)
            formatoAudio = AL_FORMAT_STEREO16;
        else {
          fprintf(stderr, "Unsupported channel count: %d\n", sfinfo.channels);
          sf_close (infile) ;
          free( data );
          alDeleteSources(1, &alsource);
          alDeleteBuffers(NUM_BUFFERS, buffers);
          alutExit();
          return( 1 );
        }
            
    frame_size = (size_t)(BUFFER_LEN * sfinfo.channels) * sizeof(short);
    data = malloc(frame_size);
            
    int i = 0;
    for(i = 0; i < NUM_BUFFERS; i++) {
        samples = sf_readf_short(infile, data, BUFFER_LEN);
        // NO en sf-read_short quan es > 1 canal
        //samples = sf_read_short(infile, data, BUFFER_LEN);
                
      if (samples > 0) {
          mostresLLegides += samples;
          printf("Ha llegit %ld mostres i en porta %ld\n", samples, mostresLLegides);
          mostresEnBytes = samples* sfinfo.channels * (sf_count_t)sizeof(short);
          alBufferData(buffers[i], formatoAudio, data, mostresEnBytes, frecuenciaMuestreo); //BUFFER_LEN, frecuenciaMuestreo);
          printf("Ha asignat %ld elements en buffer[%d]\n", samples, i);

      }
    }

    alGetSourcei(alsource, AL_BUFFERS_QUEUED, &queued);
    printf("En cola (queued): %d buffers\n", queued);
    printf("Encolamos los buffers y empezamos a reproducir\n");
    alSourceQueueBuffers(alsource, NUM_BUFFERS, buffers);
    alGetSourcei(alsource, AL_BUFFERS_QUEUED, &queued);
    printf("En cola (queued): %d bufers\n", queued);
    if((error=alGetError()) != AL_NO_ERROR)
       printf("Error rellenando los buffers por 1a vez: %s\n", alutGetErrorString(error));

    printf("Que comence a sonar!\n");
    alSourcePlay(alsource);
    if((error=alGetError()) != AL_NO_ERROR)
        printf("Error en el Play? Error: %s\n", alutGetErrorString(error));

    printf("Muestras cargadas/leidas %ld de %ld muestras totales.\n", mostresLLegides,  numTotalMostres); //sfinfo.frames*sfinfo.channels*);
    i=0;
    while (mostresLLegides < numTotalMostres) {
                //Consultamos a OpenAl si tiene buffers libres, si no tiene ninguno, continua reproduciendo.
                alGetSourcei(alsource, AL_BUFFERS_PROCESSED, &buffersLibres);

                if( buffersLibres > 0 ){
                    //Cuando se libera algun buffer, se incrementa buffersLibres, y se encola el siguiente trozo
                    //a leer desde el descriptor de lectura (f)
                    while(buffersLibres > 0) {
                        //se lee el siguiente trozo
                        samples = sf_readf_short(infile, data, BUFFER_LEN);
                        // NO en sf-read_short quan es > 1 canal
                        //samples = sf_read_short(infile, data, BUFFER_LEN);
                        if (samples > 0) {
                            mostresLLegides += samples;
                            //lo encolamos a OpenAL        
                            alSourceUnqueueBuffers(alsource, 1, &bufferID);
                            alGetSourcei(alsource, AL_BUFFERS_QUEUED, &queued);
                            printf("En cola (queued): %d, processats/lliures %d.", queued, buffersLibres);
                            
                            //samples *= sfinfo.channels * (sf_count_t)sizeof(short);
                            mostresEnBytes = samples* sfinfo.channels * (sf_count_t)sizeof(short);
                            
                            alBufferData(bufferID, formatoAudio, (short*)data, mostresEnBytes, frecuenciaMuestreo); //, frecuenciaMuestreo);         
                            alSourceQueueBuffers(alsource, 1, &bufferID);
                            buffersLibres--;
                            //alGetSourcei(alsource, AL_BUFFERS_QUEUED, &queued); printf("En fique un a la cola (queued): %d, processats/lliures %d\n", queued, buffersLibres);
                            //printf("."); fflush(stdout);         
                            // printf("%c\r", barraActivitat[i++%4]); fflush( stdout );      
                            printf(" Frames: %7ld / %7ld  mostres %c \r", mostresLLegides, sfinfo.frames, barraActivitat[i++%4]); fflush( stdout );
                            //alSourcePlay(alsource);
                            int val=0;
                            alGetSourcei(alsource, AL_SOURCE_STATE, &val);
                            if(val != AL_PLAYING) {
                                alSourcePlay( alsource );
                            }
                        }
                    }//while( buffersLibres-- )
                }//if( buffersLibres > 0)
    } //while (mostresLLegides < numTotalMostres)
            
    printf("\n"); // Pq es quede a vista el recompte de les mostres llegides
    // Si encara queden buffers per fer-los sonar, t'esperes
    // {
    i=0;
    do {
      alutSleep( 0.1f );
       printf("%c\r", barraActivitat[i++%4]); fflush( stdout );
       alGetSourcei(alsource, AL_SOURCE_STATE, &estado);
    } while(estado == AL_PLAYING);
    //}

            
    // Lliberar recursos i acabar: Close input and output files.
    sf_close (infile) ;
    free( data );
            
    alDeleteSources(1, &alsource);
    alDeleteBuffers(NUM_BUFFERS, buffers);
            
    alutExit();
            
    return EXIT_SUCCESS;
}
