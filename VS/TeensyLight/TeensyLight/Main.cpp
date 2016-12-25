#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <stdint.h>
#include <opencv2/opencv.hpp>

#include "ScreenVideoCapture.h"
#include "TeensyLightController.h"

#define HOR_PIXELS      (36)
#define VER_PIXELS      (21)
#define SPACE_FILTER    (3)
#define TIME_FILTER     (2)

#define COMPORT         (L"COM7")
#define CAP_DEVICE      (0)

using namespace cv;

static void _updatePixels(const Mat& frame, std::deque<std::vector<Vec3b> >& pixelHist, TeensyLightController& controller);
static int _getTopRow(const Mat& frame);
static void _spaceFilter(const std::vector<Vec3b>& input, std::vector<Vec3b>& output);
static void _timeFilter(const std::vector<Vec3b>& input, std::vector<Vec3b>& output);

int main(int argc, const char** argv)
{
    TeensyLightController controller;
    ScreenVideoCapture cap(CAP_DEVICE);
    Mat frame;

    std::deque<std::vector<Vec3b> > pixelHist;
    while (true)
    {
        if (!controller.isConnected())
            controller.connect(COMPORT);

        cap >> frame;
        if (frame.empty())
            break; 

        cvtColor(frame, frame, COLOR_BGRA2BGR);
        resize(frame, frame, Size(HOR_PIXELS, VER_PIXELS), INTER_LINEAR);
        _updatePixels(frame, pixelHist, controller);
    }

    return 0;
}

static void _updatePixels(const Mat& frame, std::deque<std::vector<Vec3b> >& pixelHist, TeensyLightController& controller)
{
    int topRow = _getTopRow(frame);

    std::vector<Vec3b> pixels;
    for (int i = frame.rows - 1; i > 0; --i)
        pixels.push_back(frame.at<Vec3b>(i, 0));

    for (int i = 1; i < frame.cols - 1; ++i)
        pixels.push_back(frame.at<Vec3b>(topRow, i));

    for (int i = 1; i < frame.rows; ++i)
        pixels.push_back(frame.at<Vec3b>(i, frame.cols - 1));

    std::vector<Vec3b> spaceFiltered;
    _spaceFilter(pixels, spaceFiltered);

    std::vector<Vec3b> timeFiltered;
    _timeFilter(spaceFiltered, timeFiltered);

    for (int i = 0; i < timeFiltered.size(); ++i)
        controller.setPixel(i, timeFiltered[i][2], timeFiltered[i][1], timeFiltered[i][0]);

    controller.show();
}

static int _getTopRow(const Mat& frame)
{
    for (int topRow = 0; topRow < 10; ++topRow)
    {
        int blackCount = 0;
        for (int i = 0; i < frame.cols; ++i)
            for (int c = 0; c < 3; ++c)
                if (frame.at<Vec3b>(topRow, i)[c] <= 30)
                    ++blackCount;

        if (blackCount < 20 * 3)
            return topRow;
    }

    return 0;
}

static void _spaceFilter(const std::vector<Vec3b>& input, std::vector<Vec3b>& output)
{
    for (int i = 0; i < input.size(); ++i)
    {
        float b = 0.0f;
        float g = 0.0f;
        float r = 0.0f;
        int count = 0;
        size_t start = max(i - SPACE_FILTER, 0);
        size_t end = min(i + SPACE_FILTER, input.size() - 1);
        for (size_t j = start; j <= end; ++j)
        {
            b += input[j][0];
            g += input[j][1];
            r += input[j][2];
            ++count;
        }

        output.push_back(Vec3b((uint8_t)(b / count), (uint8_t)(g / count), (uint8_t)(r / count)));
    }
}

static void _timeFilter(const std::vector<Vec3b>& input, std::vector<Vec3b>& output)
{
    static std::deque<std::vector<Vec3b> > hist;

    hist.push_back(input);

    while (hist.size() > TIME_FILTER * 2 + 1)
        hist.pop_front();

    for (int i = 0; i < input.size(); ++i)
    {
        float b = 0.0f;
        float g = 0.0f;
        float r = 0.0f;
        for (int j = 0; j < hist.size(); ++j)
        {
            b += hist[j][i][0];
            g += hist[j][i][1];
            r += hist[j][i][2];
        }

        output.push_back(Vec3b((uint8_t)(b / hist.size()), (uint8_t)(g / hist.size()), (uint8_t)(r / hist.size())));
    }
}