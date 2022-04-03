#include "metrics.h"

using namespace cv;

int metrics::diff_ms(timeval t1, timeval t2)
{
    return (((t1.tv_sec - t2.tv_sec) * 1000000) +
        (t1.tv_usec - t2.tv_usec)) / 1000;
}

double metrics::differenceBetween(cv::Mat image1, cv::Mat image2)
{
    long pixelCount = image1.rows * image1.cols;
    long identicalPixelCount = 0l;
    for (int y = 0; y < image1.rows; y++)
    {
        for (int x = 0; x < image1.cols; x++)
        {
            auto pixel1 = image1.at<cv::Vec3b>(Point(x, y));
            auto pixel2 = image2.at<cv::Vec3b>(Point(x, y));
            if (pixel1 == pixel2)
            {
                identicalPixelCount++;
            }
        }
    }
    return (double)identicalPixelCount / (double)pixelCount;
}