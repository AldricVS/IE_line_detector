#include "BaseAlgos.h"

using namespace cv;

void baseAlgos::Sobel1(cv::Mat frame, int* filterGX, int* filterGY, int size, cv::Mat* out, int limit)
{
    //Convolution Time ! (adding threshold to result might improve performances significantly)
    int step = std::floor(size / 2);
    float sumX, sumY;
    for (int y = 1;y < frame.cols - 1;y++)
    {
        for (int x = 1;x < frame.rows - 1;x++)
        {
            sumX = 0;
            sumY = 0;
            /*
            sumX = -frame.at<uchar>(Point(y-1,x-1))
                    -2*frame.at<uchar>(Point(y,x-1))
                    -frame.at<uchar>(Point(y+1,x-1))+
                    frame.at<uchar>(Point(y-1,x+1))+
                    2*frame.at<uchar>(Point(y,x+1))+
                    frame.at<uchar>(Point(y+1,x+1));
            sumY = -frame.at<uchar>(Point(y-1,x-1))
                    -2*frame.at<uchar>(Point(y-1,x))
                    -frame.at<uchar>(Point(y-1,x+1))+
                    frame.at<uchar>(Point(y+1,x-1))+
                    2*frame.at<uchar>(Point(y+1,x))+
                    frame.at<uchar>(Point(y+1,x+1));
            */
            for (int i = 0;i < size;i++)
            {
                for (int j = 0;j < size;j++)
                {
                    sumX += filterGX[i * size + j] * frame.at<uchar>(Point(y - step + i, x - step + j));
                    sumY += filterGY[i * size + j] * frame.at<uchar>(Point(y - step + i, x - step + j));
                }
            }
            out->at<uchar>(Point(y, x)) = sqrt(pow(sumX, 2) + pow(sumY, 2)) / 4;

            if (sqrt(pow(sumX, 2) + pow(sumY, 2)) / 4 < limit)
                out->at<uchar>(Point(y, x)) = 0;
            //else
              //out->at<uchar>(Point(y,x)) = 0;

        }
    }
}


void baseAlgos::SobelED(Mat frame, Mat* out, int limit)
{

    if (limit > 255 || limit < 0)
        limit = 255;
    float sumX, sumY;
    for (int y = 1;y < frame.cols - 1;y++)
    {
        for (int x = 1;x < frame.rows - 1;x++)
        {
            sumX = 0;
            sumY = 0;
            sumX = frame.at<uchar>(Point(y, x + 1)) - frame.at<uchar>(Point(y, x - 1));
            sumY = frame.at<uchar>(Point(y + 1, x)) - frame.at<uchar>(Point(y - 1, x));

            if (sqrt(pow(sumX, 2) + pow(sumY, 2)) > limit)
            {
                out->at<uchar>(Point(y, x)) = 255;
            }

            else
            {
                out->at<uchar>(Point(y, x)) = 0;
            }
        }
    }
}

void baseAlgos::simpleHough(Mat frame, Mat* acc, Mat* f, int thetaStep)
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
                    rho = j * cos((double)theta * M_PI / 180) + i * sin((double)theta * M_PI / 180);
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
    double theta;
    acc->data[max_loc_old.y * stepacc + max_loc_old.x] = 0;
    for (int i = 0;i < 40;i++)
    {
        cv::minMaxLoc(*acc, &min, &max, &min_loc, &max_loc);
        if (abs(max_loc_old.x - max_loc.x) > 5 || abs(max_loc_old.y - max_loc.y) > 5)
        { //might be interesting to use that ....
            theta = (double)max_loc.y * thetaStep;
            a = cos(theta * M_PI / 180); //compute hough inverse transform from polar to cartesian
            b = sin(theta * M_PI / 180);
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