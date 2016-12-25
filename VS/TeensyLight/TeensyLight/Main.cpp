#include <stdint.h>
#include <opencv2/opencv.hpp>

#include "ScreenVideoCapture.h"
#include "TeensyLightController.h"

#define KEY_ESC     (27)
#define HOR_PIXELS  (36)
#define VER_PIXELS  (21)

using namespace cv;

static const LPCWSTR comport = L"COM7";

static void _updatePixels(const Mat& frame, TeensyLightController& controller);
static int _getTopRow(const Mat& frame);

int main(int argc, const char** argv)
{
    TeensyLightController controller;
    controller.connect(comport);

    ScreenVideoCapture cap(0);
    Mat frame;

    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;

        cvtColor(frame, frame, COLOR_BGRA2BGR);
        resize(frame, frame, Size(HOR_PIXELS, VER_PIXELS), INTER_LINEAR);
        _updatePixels(frame, controller);
    }

    return 0;
}

static void _updatePixels(const Mat& frame, TeensyLightController& controller)
{
    int topRow = _getTopRow(frame);

    uint8_t index = 0;
    for (int i = frame.rows - 1; i > 0; --i)
    {
        Vec3b color = frame.at<Vec3b>(i, 0);
        controller.setPixel(index++, color[2], color[1], color[0]);
    }
    for (int i = 1; i < frame.cols - 1; ++i)
    {
        Vec3b color = frame.at<Vec3b>(topRow, i);
        controller.setPixel(index++, color[2], color[1], color[0]);
    }
    for (int i = 1; i < frame.rows; ++i)
    {
        Vec3b color = frame.at<Vec3b>(i, frame.cols - 1);
        controller.setPixel(index++, color[2], color[1], color[0]);
    }

    controller.show();
}

static int _getTopRow(const Mat& frame)
{
    int blackCount = 0;
    for (int i = 0; i < frame.cols; ++i)
        for (int c = 0; c < 3; ++c)
            if (frame.at<Vec3b>(0, i)[c] == 0)
                ++blackCount;

    return (blackCount > 5 * 3) ? 1 : 0;
}