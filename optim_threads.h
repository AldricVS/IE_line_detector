#ifndef __OPTIM_THREADS_H__
#define __OPTIM_THREADS_H__

#include <opencv2/opencv.hpp>

/**
 * Uses 4 threads to do convolutions more rapidly on the image 
 */
void SobelMultiThread(cv::Mat* frame, int* filterGX, int* filterGY, int size, int limit, cv::Mat* out);

#endif // __OPTIM_THREADS_H__