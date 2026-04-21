#include <iostream>
#include <thread>
#include <chrono>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


float markerLength = 0.1f; // In meters
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

Mat image, imageCopy, imageDraw;
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
        "{image    |<none>| input image }"
        "{video    |<none>| input video }"
        "{iOverlay |<none>| imatge per a sobrepossar (overlay) }"
        "{vOverlay |<none>| vídeo per a sobrepossar (overlay) }";
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

void showVideoFunc();

bool init(int argc, char *argv[])
{
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about(about);

    if (parser.has("help")) {
        parser.printMessage();
        return false;
    }

    if (parser.has("video")) {
        routeVideo = parser.get<String>("video");
        if (!routeVideo.empty()) 
        {
            inputVideo.open(routeVideo);
            isVideo = true;
        } 
        else
        {
            cout << "[DEBUG] Ruta de vídeo vacía" << endl;
            return false;
        }
    }
    else
    {
        inputVideo.open(0);
    }

    if (parser.has("image")) {
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
        cout << "[DEBUG] Opening image route: " << routeShowedImg << endl;
    }
    else
    {
        routeShowedImg = "code/images/lorax.png";
    }
    
    if(parser.has("vOverlay"))
    {
        routeShowedVideo = parser.get<String>("vOverlay");
        if (!routeShowedVideo.empty()) {
            cout << "[DEBUG] Opening video route: " << routeShowedVideo << endl; 
            outputVideo.open(routeShowedVideo);
            showVideo = true;
        }
        else
        {
            cout << "[DEBUG] Error opening video route" << endl; 
        }
    }
    else
    {
        showVideo = false;
    }

    return true;
}


int searchIDPos(int num)
{
    for(int i=0; i<ids.size(); i++)
        if(ids[i] == num) return i;

    return -1;
}

void detectMarkers()
{
    // Detect markers and estimate pose
    aruco::detectMarkers(image, dictionary, corners, ids, detectorParams, rejected);
    if(ids.size() > 0)
        aruco::estimatePoseSingleMarkers(corners, markerLength, camMatrix, distCoeffs, rvecs, tvecs);

    image.copyTo(imageCopy);
    if(ids.size() > 0) {
        aruco::drawDetectedMarkers(imageCopy, corners, ids);

        for(unsigned int i = 0; i < ids.size(); i++)
            drawFrameAxes(imageCopy, camMatrix, distCoeffs, rvecs[i], tvecs[i],
                            markerLength * 0.5f);
    }

    image.copyTo(imageDraw);

    if(ids.size() >= 4)
    {
        float halfMarkerLength = markerLength / 2;

        int idx1 = searchIDPos(25);
        int idx2 = searchIDPos(30);

        if (idx1 < 0 || idx2 < 0)
            return;

        Vec3d t1 = tvecs[idx1];
        Vec3d t2 = tvecs[idx2];
        
        float w = abs(t2[0] - t1[0]) + halfMarkerLength;
        float h = abs(t2[1] - t1[1]) + halfMarkerLength;
        
        // Plano 3D del overlay (en el sistema del marcador)
        vector<Point3f> overlay3D = {
            { -halfMarkerLength, halfMarkerLength, 0},  // Top left
            { w, halfMarkerLength, 0},                  // Top right
            { w, -h, 0},                                // Bottom right
            { -halfMarkerLength, -h, 0}                 // Bottom left
        };
        
        // Proyección con la pose real
        vector<Point2f> overlay2D;
        for(int i=0; i<ids.size(); i++)
            projectPoints(
                overlay3D,
                rvecs[idx1],
                tvecs[idx1],
                camMatrix,
                distCoeffs,
                overlay2D
            );

        // Cargar overlay
        Mat overlay;
        if (showVideo) {
            if (!outputVideo.read(overlay)) {
                outputVideo.open(routeShowedVideo);
                outputVideo.read(overlay);
            }
        } else {
            overlay = imread(routeShowedImg);
        }
        if (overlay.empty()) return;

        // Homografía
        vector<Point2f> src = {
            {0, 0},
            {(float)overlay.cols, 0},
            {(float)overlay.cols, (float)overlay.rows},
            {0, (float)overlay.rows}
        };

        Mat H = findHomography(src, overlay2D);

        // Warp con perspectiva real
        Mat warped;
        warpPerspective(overlay, warped, H, imageDraw.size());

        // Máscara y composición
        Mat mask;
        cvtColor(warped, mask, COLOR_BGR2GRAY);
        threshold(mask, mask, 1, 255, THRESH_BINARY);
        warped.copyTo(imageDraw, mask);
    }
}

char showFunc()
{
    char key;

    imshow("out", image);
    // imshow("DEBUG window", imageCopy);
    imshow("draw", imageDraw);

    if(isVideo || showVideo)
    {
        key = waitKey(1);
    }
    else
    {
        key = waitKey(0);
    }
    return key;
}

int main(int argc, char *argv[]) 
{
    char key;

    if (!init(argc, argv))
        return -1;
    
    if(isVideo)
    {
        while(inputVideo.grab()) {
            inputVideo.retrieve(image);
            detectMarkers();
            key = showFunc();
            if(key == 27) break;
        }
    }
    else
    {
        if(showVideo)
        {
            while(key != 27)
            {
                image = imread(routeImage);
                detectMarkers();
                key = showFunc();
            }
        }
        else
        {
            image = imread(routeImage);
            detectMarkers();
            key = showFunc();
        }
    }

    cout << "[DEBUG] Hasta pronto..." << endl;

    return 0;
}

