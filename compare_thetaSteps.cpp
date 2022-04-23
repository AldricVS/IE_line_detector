
#include <iostream>
#include <sys/time.h>
#include <chrono>
#include <thread>
#include <fstream>

#include "base_algos/BaseAlgos.h"
#include "optim/optim.h"
#include "metrics/metrics.h"

using namespace cv;
using namespace std;

int filterGX[9] = { -1,0,1,-2,0,2,-1,0,1 };
int filterGY[9] = { -1,-2,-1,0,0,0,1,2,1 };
int convSize = 3;
int limit = 20;

const string WINDOW_COLOR_NAME = "Color Frame";
const string WINDOW_SOBEL_NAME = "Sobel Frame";
const string WINDOW_ACC_NAME = "Acc Frame";

Mat readImage(const char* filename)
{
    Mat frame = imread(filename, IMREAD_COLOR);
    if (!frame.data)                              // Check for invalid input
    {
        std::cout << "Could not open or find the image" << std::endl;
        exit(EXIT_FAILURE);
    }
    return frame;
}

Mat createAccMatrix(Size pt)
{
    return Mat::zeros(pt, CV_16UC1);
}

Mat RGBtoGrayScale(Mat rgb)
{
    std::vector<Mat> channels(3);
    split(rgb, channels);
    return (0.07 * channels[0] + 0.72 * channels[1] + 0.21 * channels[2]);
}

int main(int argc, char const* argv[])
{
    if (argc < 3) 
    {
        cerr << "Need 2 arguments in order to run this : input image and output csv file" << endl;
        exit(EXIT_FAILURE);
    }

    double cosValues[180], sinValues[180];
    optimAlgos::computeTrigValues(cosValues, sinValues, 180);

    const char* imgPath = argv[1];
    const char* outputPath = argv[2];
    ofstream outputFile(outputPath);
    const char* sep = ";";
    outputFile << "thetaStep" << sep << "timeBaseMs" << sep << "timeOptimMs" << "\n";
    timeval start, end;
    for (int thetaStep = 1; thetaStep < 180; thetaStep++)
    {
        this_thread::sleep_for(std::chrono::milliseconds(3000));
        cout << "==== For theta = " << thetaStep << "====\n";
        Mat baseAlgoFrame = readImage(imgPath);
        Mat optimisedAlgoframe = readImage(imgPath);
        int x = baseAlgoFrame.cols, y = baseAlgoFrame.rows;
        Size pt = Size(sqrt(pow(x, 2) + pow(y, 2)) * 2, ceil(180.0 / (double)thetaStep));

        // Mat grayscale = Mat::zeros(Size(baseAlgoFrame.rows, baseAlgoFrame.cols), CV_8UC1);
        Mat sobel = Mat::zeros(Size(baseAlgoFrame.cols, baseAlgoFrame.rows), CV_8UC1);
        Mat acc = createAccMatrix(pt);

        // BASE ALGORITHM
        gettimeofday(&start, NULL);
        acc = createAccMatrix(pt);
        Mat grayscale = RGBtoGrayScale(baseAlgoFrame);
        GaussianBlur(grayscale, grayscale, Size(9, 9), 2, 2);
        baseAlgos::Sobel1(grayscale, filterGX, filterGY, convSize, &sobel, limit);
        baseAlgos::simpleHough(sobel, &acc, &baseAlgoFrame, thetaStep);
        gettimeofday(&end, NULL);
        int timeMs = metrics::diff_ms(end, start);
        printf("Base algorithm - Process done in %d milliseconds\n", timeMs);

        this_thread::sleep_for(std::chrono::milliseconds(1000));

        // OPTIMISED ALGO
        timeval startoptim, endoptim;
        gettimeofday(&startoptim, NULL);
        acc = createAccMatrix(pt);
        grayscale = RGBtoGrayScale(optimisedAlgoframe);
        GaussianBlur(grayscale, grayscale, Size(9, 9), 2, 2);
        optimAlgos::SobelMultiThread(&grayscale, filterGX, filterGY, 3, 20, &sobel);
        optimAlgos::houghMemoized(sobel, &acc, cosValues, sinValues, thetaStep, &optimisedAlgoframe);
        gettimeofday(&endoptim, NULL);
        int optimisedtimeMs = metrics::diff_ms(endoptim, startoptim);
        printf("Optim algorithm - Process done in %d milliseconds\n", optimisedtimeMs);

        outputFile << thetaStep << sep << timeMs << sep << optimisedtimeMs << "\n";
        string str = "/home/aldric-vs/COURS/M1/S8/IE/theta_steps/theta" + to_string(thetaStep) + ".png";
        imwrite(str.c_str(), optimisedAlgoframe);
    }
    outputFile.close();
    return 0;
}
