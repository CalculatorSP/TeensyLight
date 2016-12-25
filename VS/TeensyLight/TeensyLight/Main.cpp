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
        Vec4b color = frame.at<Vec4b>(i, 0);
        controller.setPixel(index++, color[2], color[1], color[0]);
    }
    for (int i = 1; i < frame.cols - 1; ++i)
    {
        Vec4b color = frame.at<Vec4b>(topRow, i);
        controller.setPixel(index++, color[2], color[1], color[0]);
    }
    for (int i = 1; i < frame.rows; ++i)
    {
        Vec4b color = frame.at<Vec4b>(i, frame.cols - 1);
        controller.setPixel(index++, color[2], color[1], color[0]);
    }

    controller.show();
}

static int _getTopRow(const Mat& frame)
{
    int blackCount = 0;
    for (int i = 0; i < frame.cols; ++i)
        for (int c = 0; c < frame.channels(); ++c)
            if (frame.at<uint8_t>(0, i, c) == 0x00)
                ++blackCount;

    return (blackCount > 5 * frame.channels()) ? 1 : 0;
}