#include <opencv2/opencv.hpp>
#include "ScreenVideoCapture.h"

#define KEY_ESC (27)

using namespace cv;

static const char* comport = "COM3";

int main(int argc, const char** argv)
{
    ScreenVideoCapture cap(0);
    Mat frame;
    namedWindow("TeensyLight", WINDOW_NORMAL);

    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;

        if (waitKey(1) == KEY_ESC)
            break;


    }

    return 0;
}