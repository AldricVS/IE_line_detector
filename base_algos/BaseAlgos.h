#ifndef __BASE_ALGOS_H__
#define __BASE_ALGOS_H__

#include <opencv2/opencv.hpp>

/**
 * All algorithms given at start of the project
 */

namespace baseAlgos
{
    void simpleHough(cv::Mat frame, cv::Mat* acc, cv::Mat* f, int thetaStep);
    void Sobel1(cv::Mat frame, int* filterGX, int* filterGY, int size, cv::Mat* out, int limit);
    void SobelED(cv::Mat frame, cv::Mat* out, int limit);
}

#endif // __BASE_ALGOS_H__