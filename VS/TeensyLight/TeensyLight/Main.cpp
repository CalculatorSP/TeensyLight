#include <stdint.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>

#include "ScreenVideoCapture.h"
#include "TeensyLightController.h"

#define KEY_ESC     (27)
#define HOR_PIXELS  (36)
#define VER_PIXELS  (21)

using namespace cv;

static const LPCWSTR comport = L"COM7";

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

        uint8_t index = 0;
        for (int i = VER_PIXELS - 1; i > 0; --i)
        {
            Vec4b color = frame.at<Vec4b>(i, 0);
            controller.setPixel(index++, color[2], color[1], color[0], 0);
        }
        for (int i = 1; i < HOR_PIXELS - 1; ++i)
        {
            Vec4b color = frame.at<Vec4b>(0, i);
            controller.setPixel(index++, color[2], color[1], color[0], 0);
        }
        for (int i = 1; i < VER_PIXELS; ++i)
        {
            Vec4b color = frame.at<Vec4b>(i, HOR_PIXELS - 1);
            controller.setPixel(index++, color[2], color[1], color[0], 0);
        }

        controller.show();
    }

    return 0;
}