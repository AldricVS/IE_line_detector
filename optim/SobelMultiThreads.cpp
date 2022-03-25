#include "optim.h"

#include <pthread.h>

using namespace cv;

/**
 * All infos that a thread need to have in order to process
 */
typedef struct thread_process_infos_t
{
    int startRow;
    int endRow;
    int startCol;
    int endCol;

    int* filterGX;
    int* filterGY;

    int size;

    int limit;

    cv::Mat* inputImage;
    cv::Mat* outputImage;
} ThreadProcessInfos;

void SobelPartial(Mat* frame, int startRow, int endRow, int startCol, int endCol, 
    int* filterGX, int* filterGY, int size, Mat* out, int limit)
{
    //Convolution Time ! (adding threshold to result might improve performances significantly)
    int step = std::floor(size / 2);
    float sumX, sumY;
    for (int y = startCol;y < endCol;y++)
    {
        for (int x = startRow;x < endRow;x++)
        {
            sumX = 0;
            sumY = 0;
            for (int i = 0;i < size;i++)
            {
                for (int j = 0;j < size;j++)
                {
                    sumX += filterGX[i * size + j] * frame->at<uchar>(Point(y - step + i, x - step + j));
                    sumY += filterGY[i * size + j] * frame->at<uchar>(Point(y - step + i, x - step + j));
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

void* threadSobel(void* thread_process_infos)
{
    ThreadProcessInfos* threadProcessInfos = (struct thread_process_infos_t*)thread_process_infos;

    SobelPartial(
        threadProcessInfos->inputImage,
        threadProcessInfos->startRow,
        threadProcessInfos->endRow,
        threadProcessInfos->startCol,
        threadProcessInfos->endCol,
        threadProcessInfos->filterGX,
        threadProcessInfos->filterGY,
        threadProcessInfos->size,
        threadProcessInfos->outputImage,
        threadProcessInfos->limit
    );

    return NULL;
}

void optimAlgos::SobelMultiThread(cv::Mat* frame, int* filterGX, int* filterGY, int size, int limit, cv::Mat* out)
{
    int midRows = frame->rows / 2;
    int midCols = frame->cols / 2;
    // Create thread infos for each of them
    ThreadProcessInfos infosTopLeft{
        1, midRows, 1, midCols, filterGX, filterGY, 3, 20, frame, out
    };
    ThreadProcessInfos infosTopRight{
        1, midRows, midCols, frame->cols - 1, filterGX, filterGY, 3, 20, frame, out
    };
    ThreadProcessInfos infosBottomLeft{
        midRows, frame->rows - 1, 1, midCols, filterGX, filterGY, 3, 20, frame, out
    };
    ThreadProcessInfos infosBottomRight{
        midRows, frame->rows - 1, midCols, frame->cols - 1, filterGX, filterGY, 3, 20, frame, out
    };

    pthread_t thread_sobelTopLeft;
    pthread_t thread_sobelTopRight;
    pthread_t thread_sobelBottomLeft;
    pthread_t thread_sobelBottomRight;

    pthread_create(&thread_sobelTopLeft, NULL, threadSobel, &infosTopLeft);
    pthread_create(&thread_sobelTopRight, NULL, threadSobel, &infosTopRight);
    pthread_create(&thread_sobelBottomLeft, NULL, threadSobel, &infosBottomLeft);
    pthread_create(&thread_sobelBottomRight, NULL, threadSobel, &infosBottomRight);
    
    pthread_join(thread_sobelTopLeft, NULL);
    pthread_join(thread_sobelTopRight, NULL);
    pthread_join(thread_sobelBottomLeft, NULL);
    pthread_join(thread_sobelBottomRight, NULL);
}