#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <mutex>
#include <sys/time.h>

#include "opencv2/opencv.hpp"

#include "optim/optim.h"
#include "metrics/metrics.h"

using namespace cv;

#define thetaStep 1

#define VIDEO 1

// Convert RGB image to grayscale using the luminosity method
void RGBtoGrayScale(Mat rgb, Mat* grayscale)
{
    std::vector<Mat> channels(3);
    split(rgb, channels);
    *grayscale = (0.07 * channels[0] + 0.72 * channels[1] + 0.21 * channels[2]);
}

int main(int argc, char** argv)
{
    double cosValues[180], sinValues[180];
    optimAlgos::computeTrigValues(cosValues, sinValues, 180);

    timeval start, end;
    char name[50];
    Mat frame;
    Mat canny;

#if VIDEO
    VideoCapture cap(0); // open the default camera
    if (!cap.isOpened())
    {  // check if we succeeded
        printf("Error capture\n");
        return -1;
    }
    cap >> frame;
#endif
#if VIDEO==0
    //Mat frame;
    frame = imread(argv[1], IMREAD_COLOR);

    if (!frame.data)                              // Check for invalid input
    {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }
#endif
    printf("setup\n");
    int x = frame.rows;
    int y = frame.cols;

    Size pt = Size(sqrt(pow(x, 2) + pow(y, 2)) * 2, ceil(180 / thetaStep));
    printf("rows : %d, cols : %d \n", frame.rows, frame.cols);
    Mat acc = Mat::zeros(pt, CV_16UC1);
    /* example of creating image and save it using opencv
    Mat exmp = Mat::zeros(Size(200,200),CV_16UC1);
    exmp.at<uchar>(Point(20,5))=255;
    exmp.at<uchar>(Point(100,25))=255;

    exmp.at<uchar>(Point(20,5))=255;
    exmp.at<uchar>(Point(100,25))=255;
    imwrite( "./test5.jpg", exmp );
    */
    printf("rho : %d, theta : %d \n", acc.cols, acc.rows); //size of the accumulation matrix
    Mat grayscale = Mat::zeros(Size(frame.rows, frame.cols), CV_8UC1);
    Mat sobel = Mat::zeros(Size(frame.cols, frame.rows), CV_8UC1);
    int filterGX[9] = { -1,0,1,-2,0,2,-1,0,1 };
    int filterGY[9] = { -1,-2,-1,0,0,0,1,2,1 };
    int convSize = 3;
    namedWindow("Color Frame", 1);
    namedWindow("Sobel Frame", 1);

#if VIDEO==0 //if image mode selected, must have image path as first argument
    RGBtoGrayScale(frame, &grayscale);
    GaussianBlur(grayscale, grayscale, Size(9, 9), 2, 2);
    SobelMultiThread(&grayscale, filterGX, filterGY, 3, 20, &sobel);
    //Canny( grayscale, sobel, 60, 60*3,3);
    //SobelED(grayscale,&sobel,20);
    simpleHough(sobel, &acc, &frame);

    imshow("Color Frame", frame);  // Show our image with hough line detection
    imshow("Sobel Frame", sobel);
    imshow("Acc Frame", acc);
    waitKey(0); //wait for key pressed in order to propely close all opencv windows
#else //if video selected, no needs of arguments in the program call

    // Threads threads;
    // pthread_create(&threads.thread_sobelTopLeft, NULL, thread_sobelTopLeft, NULL);
    // pthread_create(&threads.thread_sobelTopRight, NULL, thread_sobelTopRight, NULL);
    // pthread_create(&threads.thread_sobelBottomLeft, NULL, thread_sobelBottomLeft, NULL);
    // pthread_create(&threads.thread_sobelBottomRight, NULL, thread_sobelBottomRight, NULL);

    for (;;)
    {
        Mat res;
        gettimeofday(&start, NULL);
        cap >> frame; // get a new frame from camera
        acc = Mat::zeros(pt, CV_16UC1); //16 bits for accumulatio matrix


        // Split image in 4
        int midRows = frame.rows / 2;
        int midCols = frame.cols / 2;

        RGBtoGrayScale(frame, &grayscale);
        GaussianBlur(grayscale, grayscale, Size(9, 9), 1.5, 1.5);
        // Sobel1(grayscale, 1, frame.rows - 1, 1, frame.cols - 1, filterGX, filterGY, 3, &sobel, 20); //sobel filter, not optimized though

        optimAlgos::SobelMultiThread(&grayscale, filterGX, filterGY, 3, 20, &sobel);

        // SobelED(grayscale, &sobel, 20); //simple filter use for testing only, not mean to be used with hough transform
        //Canny( grayscale, sobel, 60, 60*3,3); //opencv canny filter, use to compare performances
        // simpleHough(sobel, &acc, &frame);
        optimAlgos::houghMemoized(sobel, &acc, cosValues, sinValues, thetaStep, &frame);
        gettimeofday(&end, NULL);
        int ms = metrics::diff_ms(end, start);
        //normalize(acc,acc,0,255,NORM_MINMAX, CV_16UC1); //normalize mat, use at your discretion
        sprintf(name, "fps : %f", 1 / ((double)ms / 1000));
        putText(frame, name, cv::Point(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(20, 20, 25), 1, LINE_AA);

        imshow("Color Frame", frame);  // Show image with Hough line detection
        imshow("Sobel Frame", sobel);  // show filter image
        imshow("Acc Frame", acc);
        if (waitKey(33) == 27) break;
    }
#endif
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
