#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


float markerLength = 0.1f;
Ptr<aruco::DetectorParameters> detectorParams;
String routeVideo, routeImage, routeShowedImg, routeShowedVideo;
VideoCapture inputVideo, outputVideo;

/* dictionary: 
    DICT_4X4_50=0, DICT_4X4_100=1, DICT_4X4_250=2
    DICT_4X4_1000=3, DICT_5X5_50=4, DICT_5X5_100=5, 
    DICT_5X5_250=6, DICT_5X5_1000=7, DICT_6X6_50=8, 
    DICT_6X6_100=9, DICT_6X6_250=10, DICT_6X6_1000=11, 
    DICT_7X7_50=12, DICT_7X7_100=13, DICT_7X7_250=14, 
    DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL = 16
*/
int dictionaryId = 16;
Ptr<aruco::Dictionary> dictionary;

Mat camMatrix, distCoeffs;
string calibrationRoute = "bin/calibracio_hercules.yml";

Mat image, imageDraw;
vector< int > ids;
vector< vector< Point2f > > corners, rejected;
vector< Vec3d > rvecs, tvecs;
bool isVideo = false,
     showVideo = false;

/* Debido a que el trabajo se realiza desde un WSL no puede trabajar con cámaras del equipo
    esto porque no tiene acceso directo a este*/
namespace {
const char* about = "Basic marker detection";
const char* keys  =
        "{help h usage ? | | Usage examples: \n\t\t./augmented_reality_with_aruco.out --image=test.jpg \
        \n\t\t./augmented_reality_with_aruco.out --video=test.mp4}"
        "{image i |<none>| input image }"
        "{video v |<none>| input video }"
        "{iOverlay y |<none>| imatge per a sobrepossar (overlay) }"
        "{vOverlay o |<none>| vídeo per a sobrepossar (overlay) }";
}


static bool readCameraParameters(string filename, Mat &camMatrix, Mat &distCoeffs) {
    FileStorage fs(filename, FileStorage::READ);
    if(!fs.isOpened())
        return false;
    fs["camera_matrix"] >> camMatrix;
    fs["distortion_coefficients"] >> distCoeffs;
    return true;
}


static bool readDetectorParameters(string filename, Ptr<aruco::DetectorParameters> &params) {
    FileStorage fs(filename, FileStorage::READ);
    if(!fs.isOpened())
        return false;
    fs["adaptiveThreshWinSizeMin"] >> params->adaptiveThreshWinSizeMin;
    fs["adaptiveThreshWinSizeMax"] >> params->adaptiveThreshWinSizeMax;
    fs["adaptiveThreshWinSizeStep"] >> params->adaptiveThreshWinSizeStep;
    fs["adaptiveThreshConstant"] >> params->adaptiveThreshConstant;
    fs["minMarkerPerimeterRate"] >> params->minMarkerPerimeterRate;
    fs["maxMarkerPerimeterRate"] >> params->maxMarkerPerimeterRate;
    fs["polygonalApproxAccuracyRate"] >> params->polygonalApproxAccuracyRate;
    fs["minCornerDistanceRate"] >> params->minCornerDistanceRate;
    fs["minDistanceToBorder"] >> params->minDistanceToBorder;
    fs["minMarkerDistanceRate"] >> params->minMarkerDistanceRate;
    fs["cornerRefinementMethod"] >> params->cornerRefinementMethod;
    fs["cornerRefinementWinSize"] >> params->cornerRefinementWinSize;
    fs["cornerRefinementMaxIterations"] >> params->cornerRefinementMaxIterations;
    fs["cornerRefinementMinAccuracy"] >> params->cornerRefinementMinAccuracy;
    fs["markerBorderBits"] >> params->markerBorderBits;
    fs["perspectiveRemovePixelPerCell"] >> params->perspectiveRemovePixelPerCell;
    fs["perspectiveRemoveIgnoredMarginPerCell"] >> params->perspectiveRemoveIgnoredMarginPerCell;
    fs["maxErroneousBitsInBorderRate"] >> params->maxErroneousBitsInBorderRate;
    fs["minOtsuStdDev"] >> params->minOtsuStdDev;
    fs["errorCorrectionRate"] >> params->errorCorrectionRate;
    return true;
}


bool init(int argc, char *argv[])
{
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about(about);

    if (parser.has("help")) {
        parser.printMessage();
        return false;
    }

    if (parser.has("video") || parser.has("v")) {
        routeVideo = parser.get<String>("video");
        if (!routeVideo.empty()) {
            inputVideo.open(routeVideo);
            isVideo = true;
        } else {
            cout << "[DEBUG] Ruta de vídeo vacía" << endl;
            return false;
        }
    }

    if (parser.has("image") || parser.has("i")) {
        routeImage = parser.get<String>("image");
        if (routeImage.empty()) {
            cout << "[DEBUG] Ruta de imagen vacía" << endl;
            return false;
        }
        isVideo = false;
    }

    if (!parser.check()) {
        parser.printErrors();
        return false;
    }

    detectorParams = aruco::DetectorParameters::create();
    dictionary = aruco::getPredefinedDictionary(
        aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId)
    );

    if (!readCameraParameters(calibrationRoute, camMatrix, distCoeffs)) {
        cerr << "[DEBUG] Need calibration first !!!" << endl;
        return false;
    }

    if(parser.has("iOverlay"))
    {
        routeShowedImg = parser.get<String>("iOverlay");
    }
    else
    {
        routeShowedImg = "code/images/lorax.png";
    }
    
    if(parser.has("vOverlay"))
    {
        routeShowedVideo = parser.get<String>("vOverlay");
        if (!routeShowedVideo.empty()) {
            cout << "[DEBUG] Opening video rute: " << routeShowedVideo << endl; 
            outputVideo.open(routeShowedVideo);
            showVideo = true;
        }
        else
        {
            cout << "[DEBUG] Error opening video rute" << endl; 
        }
    }
    else
    {
        showVideo = false;
    }

    return true;
}


void detectMarkers()
{
    // Detect markers and estimate pose
    aruco::detectMarkers(image, dictionary, corners, ids, detectorParams, rejected);
    if(ids.size() > 0)
        aruco::estimatePoseSingleMarkers(corners, markerLength, camMatrix, distCoeffs, rvecs, tvecs);

    image.copyTo(imageDraw);

    if(corners.size() >= 4)
    {
        Point2f p1 = corners[1][0];
        Point2f p2 = corners[corners.size()/2][2];

        float markWide = 100.0f;

        int x1 = cvRound(p1.x);
        int y1 = cvRound(p1.y + markWide);
        int x2 = cvRound(p2.x);
        int y2 = cvRound(p2.y - markWide);

        // Rect válido independientemente del orden
        int x = min(x1, x2);
        int y = min(y1, y2);
        int w = abs(x2 - x1);
        int h = abs(y2 - y1);

        Rect roi(x, y, w, h);

        Mat overlay;
        if(showVideo)
        {
            // Carga vídeo
            if(outputVideo.grab())
            {
                outputVideo.retrieve(overlay);
            }
        }
        else
        {
            // Cargar imagen
            overlay = imread(routeShowedImg);
        }

        if (overlay.empty()) {
            cerr << "No se pudo cargar la imagen\n";
            return;
        }

        // Redimensionar y copiar
        Mat overlayResized;
        resize(overlay, overlayResized, roi.size());
        overlayResized.copyTo(imageDraw(roi));
    }
}

char showFunc()
{
    if(isVideo)
    {
        imshow("out", image);
        imshow("draw", imageDraw);
        char key = waitKey(1);
        return key;
    }
    else
    {
        imshow("out", image);
        imshow("draw", imageDraw);
        waitKey(0);
    }
}

int main(int argc, char *argv[]) {
    
    if (!init(argc, argv))
        return -1;
    
    if(isVideo)
    {
        while(inputVideo.grab()) {
            inputVideo.retrieve(image);
            detectMarkers();
            char key = showFunc();
            if(key == 27) break;
        }
    }
    else
    {
        image = imread(routeImage);
        detectMarkers();
        showFunc();
    }
    cout << "[DEBUG] Hasta pronto..." << endl;

    return 0;
}

