#include <stdio.h>
#include <iostream>
#include <sys/time.h>

#include "opencv2/opencv.hpp"

#include "base_algos/BaseAlgos.h"

using namespace cv;

#define thetaStep 4

#define VIDEO 0

int diff_ms(timeval t1, timeval t2)
{
    return (((t1.tv_sec - t2.tv_sec) * 1000000) +
        (t1.tv_usec - t2.tv_usec)) / 1000;
}

// Convert RGB image to grayscale using the luminosity method
void RGBtoGrayScale(Mat rgb, Mat* grayscale)
{
    std::vector<Mat> channels(3);
    split(rgb, channels);
    *grayscale = (0.07 * channels[0] + 0.72 * channels[1] + 0.21 * channels[2]);
}

int main(int argc, char** argv)
{

    timeval start, end;
    printf("OK !\n");
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
    baseAlgos::Sobel1(grayscale, filterGX, filterGY, convSize, &sobel, 25);
    //Canny( grayscale, sobel, 60, 60*3,3);
    //SobelED(grayscale,&sobel,20);
    baseAlgos::simpleHough(sobel, &acc, &frame, thetaStep);

    imshow("Color Frame", frame);  // Show our image with hough line detection
    imshow("Sobel Frame", sobel);
    imshow("Acc Frame", acc);
    waitKey(0); //wait for key pressed in order to propely close all opencv windows
#else //if video selected, no needs of arguments in the program call
    for (;;)
    {
        Mat res;
        gettimeofday(&start, NULL);
        cap >> frame; // get a new frame from camera
        acc = Mat::zeros(pt, CV_16UC1); //16 bits for accumulatio matrix
        RGBtoGrayScale(frame, &grayscale);
        GaussianBlur(grayscale, grayscale, Size(9, 9), 1.5, 1.5);
        // baseAlgos::Sobel1(grayscale,filterGX,filterGY,3, &sobel, 20); //sobel filter, not optimized though
        baseAlgos::SobelED(grayscale, &sobel, 20); //simple filter use for testing only, not mean to be used with hough transform
        //Canny( grayscale, sobel, 60, 60*3,3); //opencv canny filter, use to compare performances
        baseAlgos::simpleHough(sobel, &acc, &frame);
        gettimeofday(&end, NULL);
        int ms = diff_ms(end, start);
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
