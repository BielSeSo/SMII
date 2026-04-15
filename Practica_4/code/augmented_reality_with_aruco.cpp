#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/core.hpp>


using namespace std;
using namespace cv;


float markerLength = 0.1f;
Ptr<aruco::DetectorParameters> detectorParams;
String routeVideo, routeImage;
VideoCapture inputVideo;

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

Mat image, imageCopy, imageDraw;
vector< int > ids;
vector< vector< Point2f > > corners, rejected;
vector< Vec3d > rvecs, tvecs;
bool isVideo = false;

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

    return true;
}


void detectMarkers()
{
    // Detect markers and estimate pose
    aruco::detectMarkers(image, dictionary, corners, ids, detectorParams, rejected);
    if(ids.size() > 0)
        aruco::estimatePoseSingleMarkers(corners, markerLength, camMatrix, distCoeffs, rvecs,
                                        tvecs);
    // draw results
    image.copyTo(imageCopy);
    if(ids.size() > 0) {
        aruco::drawDetectedMarkers(imageCopy, corners, ids);

        for(unsigned int i = 0; i < ids.size(); i++)
        {
            drawFrameAxes(imageCopy, camMatrix, distCoeffs, rvecs[i], tvecs[i], markerLength * 0.5f);
            cout << "[DEBUG] tvecs: " << tvecs[i] << endl;
        }
    }

    // TODO: Representar la imagen a partir de las marcas
}

int main(int argc, char *argv[]) {
    
    if (!init(argc, argv))
        return -1;
    
    if(isVideo)
    {
        while(inputVideo.grab()) {
            inputVideo.retrieve(image);

            detectMarkers();

            imshow("out", imageCopy);
            char key = (char)waitKey(1);
            if(key == 27) break;
        }
    }
    else
    {
        image = imread(routeImage);

        detectMarkers();

        imshow("out", imageCopy);
        char key = (char)waitKey(0);
        if(key == 27) return 0;
    }
    cout << "[DEBUG] Hasta pronto..." << endl;

    return 0;
}

