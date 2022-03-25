#include "optim.h"

#include "math.h"

using namespace cv;

void optimAlgos::computeTrigValues(double* cosValues, double* sinValues, int max)
{
    for (int theta = 0; theta < 180; theta++)
    {
        double x = (double)theta * M_PI / 180;
        cosValues[theta] = cos(x);
        sinValues[theta] = sin(x);
    }
}

void optimAlgos::houghMemoized(Mat frame, Mat* acc, double* cosValues, double* sinValues, int thetaStep, Mat* f)
{
    int channels = frame.channels();
    int nRows = frame.rows;
    int nCols = frame.cols * channels;
    //const uchar* image = frame.ptr();
    int step = (int)frame.step;
    int stepacc = (int)acc->step;
    if (frame.isContinuous())
    {
        nCols *= nRows;
        nRows = 1;
    }

    int i, j;
    double rho;
    for (i = 0; i < frame.rows; i++)
    {
        for (j = 0; j < frame.cols; j++)
        {
            if (frame.data[i * step + j] != 0)
            {
                for (int theta = 0;theta < 180; theta += thetaStep)
                {
                    rho = j * cosValues[theta] + i * sinValues[theta];
                    if (rho != 0)
                        acc->at<ushort>(Point(cvRound(rho), (int)cvRound(theta / thetaStep))) += 1;
                }
            }
        }
    }
    cv::Point min_loc, max_loc;
    cv::Point min_loc_old, max_loc_old;
    double min, max;
    cv::minMaxLoc(*acc, &min, &max, &min_loc_old, &max_loc_old);

    Point pt1, pt2;
    double a, b;
    double x0, y0;
    int theta;
    acc->data[max_loc_old.y * stepacc + max_loc_old.x] = 0;
    for (int i = 0;i < 40;i++)
    {
        cv::minMaxLoc(*acc, &min, &max, &min_loc, &max_loc);
        if (abs(max_loc_old.x - max_loc.x) > 5 || abs(max_loc_old.y - max_loc.y) > 5)
        { //might be interesting to use that ....
            theta = max_loc.y * thetaStep;
            a = cosValues[theta]; //compute hough inverse transform from polar to cartesian
            b = sinValues[theta];
            x0 = a * max_loc.x;
            y0 = b * max_loc.x;
            pt1.x = cvRound(x0 + 1000 * (-b)); //compute first point belonging to the line
            pt1.y = cvRound(y0 + 1000 * (a));
            pt2.x = cvRound(x0 - 1000 * (-b)); //compute second point
            pt2.y = cvRound(y0 - 1000 * (a));
            line(*f, pt1, pt2, Scalar(0, 0, 255), 3, LINE_AA);
            acc->at<ushort>(Point(max_loc.x, max_loc.y)) = 0;
            max_loc_old.x = max_loc.x;
            max_loc_old.y = max_loc.y;
        }
        else
        {
            acc->at<ushort>(Point(max_loc.x, max_loc.y)) = 0;
            i--;
        }
    }
}