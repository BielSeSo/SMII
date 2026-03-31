/*
//
// opencv_camara.cpp
// 
// Asignatura SMII. 
// Versió 2.0 per Biel Selma Solans
// Simplement se ha fet una petita refactoricació del codi
//
// En este ejemplo se exploran las operaciones de acceso a vídeo, en directo (cámara) o desde fichero con OpenCV:
// * Llegir de càmera, camera IP i de fitxer de vídeo
// * Guardar resultats en mapa de bits
// * Guardar resultats en vídeo
//

// Executar en: 
   $ opencv_camara -c=0                                        --> cámara RGB por defecto como fuente de vídeo
   $ opencv_camara --camera=2                                  --> escoger una cámara que no es la de por defecto
   $ opencv_camara -v=Imagenes/laser.avi                       --> vídeo como fuente de vídeo
   $ opencv_camara -video_file=Imagenes/laser.mpg
   $ opencv_camara -i=http://79.108.129.167:9000/mjpg/video.mjpg
   $ opencv_camara -i=http://158.42.148.154/mjpg/video.mjpg
   $ opencv_camara -i=http://24.172.4.142/mjpg/video.mjpg?COUNTER

 * Està basat en 
  * "VideoCapture" i "Reading and Writing Images and Video" <https://docs.opencv.org/2.4/modules/highgui/doc/reading_and_writing_images_and_video.html>
  * A basic sample on using the VideoCapture interface can be found at opencv_source_code/samples/cpp/starter_video.cpp
  * Another basic video processing sample can be found at opencv_source_code/samples/cpp/video_dmtx.cpp
  * https://docs.opencv.org/3.4/dc/d2c/tutorial_real_time_pose.html
*/


#include <stdio.h>
#include <ctime>

// Les capsaleres canvien en la V4, ho comprobe'm
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>


#include "comun.h"

using namespace cv;  // The new C++ interface API is inside this namespace. Import it.
using namespace std; // para que "string" sea un tipo

// Nom i titul de la finestra per a la imatge original i la resultat
#define fOriginal "Imatge original"
#define fResultat "Imatge de bordes"
#define fGrises "Imatge grises"

// Converteix en un string en format "AnyMesDiaHoraMinutsSegons" en cv_::format
#define FETXA_HORA(ltm) format("%04d%02d%02d%02d%02d%02d", (1900 + ltm->tm_year), (1 + ltm->tm_mon), ltm->tm_mday, (ltm->tm_hour), (ltm->tm_min), (ltm->tm_sec))


enum TIPO_FONT_VIDEO {CAMARA_IP=-2, FITXER_VIDEO = -1, CAMARA_DIGITAL=0};

string hot_keys =
  "\n\nDurant l'execució es poden gastar les tecles següents: \n"
  "\tESC - acabar!\n"
  "\tv: Grava vídeo / para de gravar el vídeo\n"
  "\tf: Pren una foto del original\n"
  "\tF: Pren una foto del processat\n";

void ajuda( char *nomPrograma )
{
  cout << "\nEn este ejemplo se exploran las operaciones de acceso a vídeo, en directo (cámara) o desde fichero con OpenCV:\n" <<
    " * Llegir de càmera i de vídeo\n" <<
    " * Generar resultat de processar vídeo en disc al temps que el mostra en pantalla\n" <<
    "Ús: \n" <<
  nomPrograma << " [--camera=n | -c n] [--video_file ruta | -v n] [--ip_address IP@ | -i IP@]\n";
  cout << hot_keys;
} // Fi de ajuda

//https://stackoverflow.com/questions/9450656/positional-argument-v-s-keyword-argument
// A positional argument is a name that is not followed by an equal sign (=) and default value.
// A keyword argument is followed by an equal sign and an expression that gives its default value.

// "{@camera_number| 0 | camera number}"
// "{@video_file| laser.mpg | fitxer de vídeo a reproduir}"
const String keys = {
    "{help h       |  | mensatge ajuda}"
    "{camera c     |  | capture video from camera (device index starting from 0) }"
    "{video_file v |  | use fitxer de video as input }"
    "{ip_address i |  | address of IP_camera}"  // p. ex. Droid camera
/*    
    "{h | help |   | mensatge ajuda}"
    "{c | camera | 0 | capture video from camera (device index starting from 0) }"
    "{v | @video_file |   | use fitxer de video as input }"
    "{i | @ip_addres| 0 | address of IP_camera}"  // p. ex. Droid camera
*/    
};


const char* acercaDe = "\nDemo de VideoCapture per a llegit un fluxe d'imatges de diferentes \"video streams\": camera digital, fitxer de vídeo o càmera IP.\n";

vector<int> compression_paramsJPEG, compression_paramsPNG;
VideoCapture cap;                // instantiate VideoCapture
int fontDeVideo = 0;    // >= 0 --> camara, -1 fitxer de vídeo, -2 CameraIP
String rutaVideo;       // argument de la línia d'ordres per a llegir d'ell el vídeo o @IP

/*
 * Inicialitza paràmetres
 *  - de generació de formats de mapes de bits
 *  - gestió de paràmetres de línia d'ordres
 */
int inicializaciones(int argc, char **argv) {

  // PNG 0..9, JPEG/WebP 0..100, PXM_BINARY 0..1
  // https://docs.opencv.org/4.2.0/d4/da8/group__imgcodecs.html#gabbc7ef1aa2edfaa87772f1202d67e0ce
  compression_paramsJPEG.push_back(IMWRITE_JPEG_QUALITY);
  compression_paramsJPEG.push_back(95); 

  compression_paramsPNG.push_back(IMWRITE_PNG_COMPRESSION);
  compression_paramsPNG.push_back(9);

  // https://docs.opencv.org/3.0-beta/modules/imgcodecs/doc/reading_and_writing_images.html    
  //
  // Anàlisi de la línia d'ordres
  // 
  CommandLineParser parser(argc, argv, keys);  
  //printf("Per a Openc MAJOR_VERSION > %d\n", CV_MAJOR_VERSION);
  
  // http://answers.opencv.org/question/101515/every-thing-works-fine-except-for-commandlineparser-has-no-member-named-aboutprint-messagehascheck/
  //error: ‘bool cv::CommandLineParser::has(const string&)’ is protected

  parser.about( acercaDe );
  //parser.printMessage();
  if (parser.has("help")) {
      ajuda( argv[0] );
      parser.printMessage();

      return( -3 );
  } 
  
  // Note that there are no default values for help ... so we can check their presence using the has() method.
  // Arguments with default values are considered to be always present. Use the get() method in these cases to check their actual value instead.
  if (parser.has("camera")) {
    fontDeVideo = parser.get<int>("camera");    
    printf("Canviant a la camera %d \n", fontDeVideo);
  } else if (parser.has("video_file")) {
    fontDeVideo = FITXER_VIDEO;      
    rutaVideo = parser.get<String>("video_file"); // rutaVideo = "laser.avi"; 
    printf("Llegint del fitxer de vídeo <%s>\n", rutaVideo.c_str() );
  } else if (parser.has("ip_address")) {
    fontDeVideo = CAMARA_IP;
    rutaVideo = parser.get<String>("ip_address");
    printf("Conectant en la camera en IP@ <%s>\n", rutaVideo.c_str() );
  }

  return( fontDeVideo );
} // Fi de  inicializaciones

//
// Prog. ppal.
// Còdics d'eixida
#define ERROR_ARGUMENTS     -4
#define ERROR_FONTDEVIDEO   -5
#define ERROR_CONVERSIOJPEG -6 
#define ERROR_CONVERSIOPNG  -7
#define FI_APLICACIO_OK      0
//

int  main(int argc, char **argv)
{
  Mat frame, edges, grays;
  bool fiDeVideo = false, volSeguirUsuari = true;
  int tecla;
  time_t now;
  tm *ltm;
  String fetxaString, rutaFitxerVideo; // a on es gravará el resultat en vídeo
  int gravantVideo = false;
  VideoWriter elFitxerDeVideo;
  int codec;

  if ( (fontDeVideo = inicializaciones( argc, argv )) < CAMARA_IP )
  {
    printf("oops %d\n", fontDeVideo);
    return( ERROR_ARGUMENTS );
  }

  //printf("prog. ppal %d\n", fontDeVideo);  
  if ( fontDeVideo >= CAMARA_DIGITAL ) {
    printf("Finalment: obrint cam. #%d.\n", fontDeVideo );
    cap.open( fontDeVideo );
  }
  else {
    printf("Finalment: obrint des de >%s<.\n", rutaVideo.c_str() );
    cap.open( rutaVideo );
  }
  
  if(!cap.isOpened()) { // check if we succeeded
    if (fontDeVideo == CAMARA_IP )
      printf("Error obrint camIP des de %s\n", argv[2]);
    else
    if (fontDeVideo == FITXER_VIDEO )
      printf("Error obrint fitxer de vídeo des de %s\n", argv[2]);
    else
      printf("Error obrint camera %d\n", fontDeVideo);
    return( ERROR_FONTDEVIDEO );
  } // Fi de    if(!cap.isOpened()) 

  if( cap.grab() ) // Congela la imatge a la càmera
  {
    cap.retrieve(frame, 0 ); // Decodes and returns the grabbed video frame.
    printf("AmplexAlt %dx%d, depth %d i components  %d\n",
            frame.cols, frame.rows, frame.depth(), frame.channels() );
  }   
	  	    
  namedWindow( fOriginal, WINDOW_NORMAL); //  If this is set, the user can resize the window (no constraint).
  // WINDOW_OPENGL
  moveWindow( fOriginal, 0, 0 );
  namedWindow( fResultat, WINDOW_AUTOSIZE );
  //If this is set, the window size is automatically adjusted to fit the displayed image, and you cannot change the window size manually.
  moveWindow( fResultat, frame.cols+10, 0 );

  namedWindow( fGrises, WINDOW_NORMAL );
  moveWindow( fResultat, frame.cols+20, 0 );


  //for(;;)
  while ( !fiDeVideo AND volSeguirUsuari )
  {
    // El métode sobrecarregat de lectura, 
    //cap >> frame; // get a new frame from camera
    // inclou "graba i decodifica == grab + retrieve"
    if ( !cap.grab() ) // Congela la imatge a la càmera
      fiDeVideo = true;
    else
    {
      cap.retrieve(frame, 0 ); // Decodes and returns the grabbed video frame.

      cvtColor(frame, edges, COLOR_BGR2GRAY);

      GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
      Canny(edges, edges, 0, 30, 3);

      cvtColor(frame, grays, COLOR_BGR2GRAY);

      imshow(fGrises, grays);
      imshow(fOriginal, frame);
      imshow(fResultat, edges); //         imshow("edges", edges);

      if( gravantVideo ) {
        printf("."); fflush( stdout );
        elFitxerDeVideo.write( edges );
      } 

      tecla = waitKey(25) & 255;  // Espera una tecla los milisegundos que haga falta      
      now = time(0); // http://answers.opencv.org/question/138871/how-to-add-date-to-a-recorded-video/
      ltm = localtime(&now);
      fetxaString = FETXA_HORA(ltm);
	
      switch ( tecla )
      {
        case ESC:
        case 'q':
        case 'Q':    // Si 'ESC', q ó Q, ¡acabar!
          volSeguirUsuari = false;
          break;

        case 'h': // Recordar tecles que es poden gastar
          std::cout << hot_keys;
          break;
          
        case 'v': // Grava vídeo
          if( !gravantVideo )
          {
            rutaFitxerVideo = "video__" + fetxaString + ".avi"; // ".mpg";
            printf( "Començant a gravar en video: %s\n", rutaFitxerVideo.c_str() );
            codec = VideoWriter::fourcc('M','J','P','G'); //  is a motion-jpeg codec

            elFitxerDeVideo.open( rutaFitxerVideo, 
                codec, 
                5, //30,
                cv::Size( frame.cols, frame.rows),
                edges.channels() == 3 ); //true); // isColor	     			  
            //			 fps, Size frameSize, bool isColor=true)
            if ( !elFitxerDeVideo.isOpened() ) {
              printf("Error al obrir %s\n", rutaFitxerVideo.c_str() );
              gravantVideo = false;
            }
            else
            {
              gravantVideo = true;
            }
          }
          else
          {
            elFitxerDeVideo.release();
            printf( "Tancant la gravació en video: %s\n", rutaFitxerVideo.c_str() );
              // elFitxerDeVideo = "";
              gravantVideo = false;
          }
          //gravantVideo = !gravantVideo;	   	   
          break;

        case 'f':
        case 'F': // Pren una foto del original
          try {
            imwrite("bin/ejercicio_5_img/original__"+ fetxaString +".jpg", frame, compression_paramsJPEG);
          }
          catch (std::runtime_error& ex) {
            fprintf(stderr, "Exception converting image to JPEG format: %s\n", ex.what());
            return( ERROR_CONVERSIOJPEG );
          }
          printf("Guardat:: bin/ejercicio_5_img/original__%s.jpg\n", fetxaString.c_str() );	  
          break;  
        
        case 'l':
        case 'L': // Pren una foto del processat
          try {
            imwrite("bin/ejercicio_5_img/linies__"+ fetxaString +".png", edges, compression_paramsPNG);
          }
          catch (std::runtime_error& ex) 
          {
            fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
            return( ERROR_CONVERSIOPNG );
          }
          printf("Guardat:: bin/ejercicio_5_img/linies__%s.jpg\n", fetxaString.c_str() );
          break;

        case 'g':
        case 'G': // Pren una foto del processat
          try {
            imwrite("bin/ejercicio_5_img/grays__"+ fetxaString +".png", grays, compression_paramsPNG);
          }
          catch (std::runtime_error& ex) 
          {
            fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
            return( ERROR_CONVERSIOPNG );
          }
          printf("Guardat:: bin/ejercicio_5_img/grays__%s.jpg\n", fetxaString.c_str() );
          break;

          default: break;    
          } // Fin de "switch ( tecla )" 
        
            }// Fi de if ( !cap.grab() )
          }
      return( FI_APLICACIO_OK ) ;
}

//
// Fi de "opencv_camara.cpp"
//
