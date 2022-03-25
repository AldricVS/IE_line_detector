#ifndef __METRICS_H__
#define __METRICS_H__

#include <sys/time.h>
#include <opencv2/opencv.hpp>

/**
 * All functions used to compute metrics
 */

namespace metrics
{
    int diff_ms(timeval t1, timeval t2);
    
    /**
     * Returns a number between 1 and 0 showing the pixel difference ratio between 
     * two RGB images. The difference is computed pixel by pixel.
     * 
     * The algorithm expect two images of the same size, else the behavior is undefined.
     */
    double differenceBetween(cv::Mat image1, cv::Mat image2);
}

#endif // __METRICS_H__