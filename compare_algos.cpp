
#include <iostream>
#include <sys/time.h>

#include "base_algos/BaseAlgos.h"
#include "optim/optim.h"
#include "metrics/metrics.h"

using namespace cv;
using namespace std;

int filterGX[9] = { -1,0,1,-2,0,2,-1,0,1 };
int filterGY[9] = { -1,-2,-1,0,0,0,1,2,1 };
int convSize = 3;
int limit = 20;

#define thetaStep 4


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

void RGBtoGrayScale(Mat rgb, Mat* grayscale)
{
    std::vector<Mat> channels(3);
    split(rgb, channels);
    *grayscale = (0.07 * channels[0] + 0.72 * channels[1] + 0.21 * channels[2]);
}

int main(int argc, char const *argv[])
{
    double cosValues[180], sinValues[180];
    optimAlgos::computeTrigValues(cosValues, sinValues, 180);

    timeval start, end;
    cout << "Read image " << argv[1] << endl;
    Mat baseAlgoFrame = readImage(argv[1]);
    Mat optimisedAlgoframe = readImage(argv[1]);
    int x = baseAlgoFrame.cols, y = baseAlgoFrame.rows;
    Size pt = Size(sqrt(pow(x, 2) + pow(y, 2)) * 2, ceil(180 / thetaStep));

    // No need to create them multilpe times
    Mat grayscale = Mat::zeros(Size(baseAlgoFrame.rows, baseAlgoFrame.cols), CV_8UC1);
    Mat sobel = Mat::zeros(Size(baseAlgoFrame.cols, baseAlgoFrame.rows), CV_8UC1);
    Mat acc = createAccMatrix(pt);

    namedWindow(WINDOW_COLOR_NAME);
    namedWindow(WINDOW_SOBEL_NAME);
    namedWindow(WINDOW_ACC_NAME);

    // BASE ALGORITHM
    printf("-> Process base algorithm\n");
    gettimeofday(&start, NULL);

    acc = createAccMatrix(pt);
    RGBtoGrayScale(baseAlgoFrame, &grayscale);
    GaussianBlur(grayscale, grayscale, Size(9, 9), 2, 2);
    baseAlgos::Sobel1(grayscale, filterGX, filterGY, convSize, &sobel, limit);
    baseAlgos::simpleHough(sobel, &acc, &baseAlgoFrame, thetaStep);

    gettimeofday(&end, NULL);
    int timeMs = metrics::diff_ms(end, start);
    printf("Process done in %d milliseconds\n", timeMs);

    imshow(WINDOW_COLOR_NAME, baseAlgoFrame);  // Show our image with hough line detection
    imshow(WINDOW_SOBEL_NAME, sobel);
    //imshow(WINDOW_ACC_NAME, acc);
    waitKey(); //wait for key pressed in order to propely close all opencv windows

    // OPTIMISED ALGO
    timeval startoptim, endoptim;

    printf("-> Starting optimised algos\n");
    acc = createAccMatrix(pt);
    gettimeofday(&startoptim, NULL);
    RGBtoGrayScale(optimisedAlgoframe, &grayscale);
    GaussianBlur(grayscale, grayscale, Size(9, 9), 2, 2);
    optimAlgos::SobelMultiThread(&grayscale, filterGX, filterGY, 3, 20, &sobel);
    optimAlgos::houghMemoized(sobel, &acc, cosValues, sinValues, 16,&optimisedAlgoframe);
    gettimeofday(&endoptim, NULL);
    int optimisedtimeMs = metrics::diff_ms(endoptim, startoptim);
    printf("Process done in %d milliseconds\n", optimisedtimeMs);


    imshow("Color Frame", optimisedAlgoframe);  // Show our image with hough line detection
    imshow("Sobel Frame", sobel);
    //imshow("Acc Frame", acc);
    waitKey(0); //wait for key pressed in order to propely close all opencv windows


    // Comparison of both images
    printf("-> Precision of the optimised algorithm\n");
    double precision = metrics::differenceBetween(baseAlgoFrame,optimisedAlgoframe);
    printf("Precision is : %lf\n", precision);

    return 0;
}
