#ifndef __OPTIM_THREADS_H__
#define __OPTIM_THREADS_H__

/**
 * All optimized algorithms are defined in this file
 */

#include <opencv2/opencv.hpp>

namespace optimAlgos
{
    /**
     * Uses 4 threads to do convolutions more rapidly on the image
     */
    void SobelMultiThread(cv::Mat* frame, int* filterGX, int* filterGY, int size, int limit, cv::Mat* out);

    /**
     * Precompute values of sin and cos from 0 to 'max' (not included).
     * Takes two arrays of size 'max' as parameter and fill them.
     */
    void computeTrigValues(double* cosValues, double* sinValues, int max);

    void houghMemoized(cv::Mat frame, cv::Mat* acc, double* cosValues, double* sinValues, int thetaStep, cv::Mat* f);
}

#endif // __OPTIM_THREADS_H__