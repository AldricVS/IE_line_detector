#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

const int WIDTH = 640;
const int HEIGHT = 480;

void lineWithAngle(Mat imgOutput, Point startPoint, int length, int angle, Scalar color = Scalar(0, 0, 0), int thickness = 1)
{
    Point endPoint;
    endPoint.x =  (int)round(startPoint.x + length * cos(angle * CV_PI / 180.0));
    endPoint.y =  (int)round(startPoint.y + length * sin(angle * CV_PI / 180.0));
    line(imgOutput, startPoint, endPoint, color, thickness);
}

void createImgStraightLines()
{
    Mat image(HEIGHT, WIDTH, CV_8UC3, Scalar(255, 255, 255));
    line(image, Point(WIDTH / 2, 0), Point(WIDTH / 2, HEIGHT - 1), Scalar(0, 0, 0), 5);
    line(image, Point(WIDTH / 4, HEIGHT / 2), Point(WIDTH / 4, HEIGHT - 1), Scalar(0, 0, 255), 10);
    imwrite("./imgs_test/test_line_1.jpg", image);
}

void createImgDiagonalLines1()
{
    Mat image(HEIGHT, WIDTH, CV_8UC3, Scalar(255, 255, 255));
    
    lineWithAngle(image, Point(0, 0), 2 * WIDTH / 3, 8, Scalar(255, 0, 0), 5);
    lineWithAngle(image, Point(WIDTH / 2, HEIGHT / 2), WIDTH / 4, 31, Scalar(0, 255, 0), 5);
    imwrite("./imgs_test/test_line_2.jpg", image);
}

void createImgDiagonalLines2()
{
    Mat image(HEIGHT, WIDTH, CV_8UC3, Scalar(255, 255, 255));
    
    lineWithAngle(image, Point(WIDTH / 10, HEIGHT / 10), WIDTH / 2, 1, Scalar(0, 0, 255), 2);
    lineWithAngle(image, Point(WIDTH / 2, HEIGHT / 4), HEIGHT / 2, 92, Scalar(0, 255, 0), 2);
    imwrite("./imgs_test/test_line_3.jpg", image);
}

int main(int argc, char const *argv[])
{
    // Create a black image
    createImgStraightLines();
    createImgDiagonalLines1();
    createImgDiagonalLines2();

    return 0;
}
