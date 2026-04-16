#include <string>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>

using namespace std;
using namespace cv;

namespace {
const char* about = "Create an ArUco marker image";
const char* keys  =
        "{@outfile |<none> | Output image }"
        "{id       |       | Marker id in the dictionary }";
}

int main(int argc, char *argv[]) {
    CommandLineParser parser(argc, argv, keys);
    parser.about(about);

    if(argc < 3) {
        parser.printMessage();
        return 0;
    }

    int IDsList[] = {25, 33, 30, 23};
    int dictionaryId = 16;
    int markerId = parser.get<int>("id");
    int borderBits = 1;
    int markerSize = 200;

    String out = parser.get<String>(0);

    if(markerId >= 5)
    {
        cout << "[DEBUG] Valor no valido 1..4" << endl;
        return 0;
    }

    if(!parser.check()) {
        parser.printErrors();
        return 0;
    }

    Ptr<aruco::Dictionary> dictionary =
        aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

    Mat markerImg;
    aruco::drawMarker(dictionary, IDsList[markerId-1], markerSize, markerImg, borderBits);

    imwrite(out, markerImg);

    return 0;
}