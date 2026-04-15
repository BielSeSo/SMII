#include <string>
#include <iostream>

#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>

using namespace std;
using namespace cv;

namespace {
const char* about = "Create an ArUco grid board image";
const char* keys  = 
        "{s        |       | Separation }";
}

int main(int argc, char *argv[]) {
    CommandLineParser parser(argc, argv, keys);
    parser.about(about);

    int markersX = 2;
    int markersY = 2;
    int markerLength = 100;
    int markerSeparation = parser.get<int>("s");
    int dictionaryId = 16;
    int margins = markerSeparation;

    int borderBits = 1;

    String out = "bin/marcas/board.png";

    if(!parser.check()) {
        parser.printErrors();
        return 0;
    }

    Size imageSize;
    imageSize.width = markersX * (markerLength + markerSeparation) - markerSeparation + 2 * margins;
    imageSize.height =
        markersY * (markerLength + markerSeparation) - markerSeparation + 2 * margins;

    Ptr<aruco::Dictionary> dictionary =
        aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

    Ptr<aruco::GridBoard> board = aruco::GridBoard::create(markersX, markersY, float(markerLength),
                                                      float(markerSeparation), dictionary);

    // show created board
    Mat boardImage;
    board->draw(imageSize, boardImage, margins, borderBits);
    
    imwrite(out, boardImage);

    return 0;
}
