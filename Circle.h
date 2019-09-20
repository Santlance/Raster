#ifndef CIRCLE_H
#define CIRCLE_H
#include <opencv2/opencv.hpp>
using namespace cv;

class Circle {
public:
    cv::Point center;
    double radius;

    Circle(cv::Point c, double r) {
        center = c;
        radius = r;
    }
};

#endif // ! CIRCLE_H
