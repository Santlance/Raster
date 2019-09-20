#ifndef LINE_H
#define LINE_H
#include <opencv2/opencv.hpp>
class Line {
public:
    // Ax + By + C = 0
    double A;
    double B;
    double C;

    // y = kx + b
    double k;
    double b;

    // x = ty + c
    double t;
    double c;

    // points, assum the pts.x < pte.x
    cv::Point pts;
    cv::Point pte;
    cv::Point y_s;
    cv::Point y_e;

    Line(cv::Point p0, cv::Point p1) {
        pte = p1.x > p0.x ? p1 : p0;
        pts = p0.x < p1.x ? p0 : p1;
        y_s = p0.y < p1.y ? p0 : p1;
        y_e = p1.y > p0.y ? p1 : p0;
        A = p1.y - p0.y;
        B = p0.x - p1.x;
        C = p0.y * p1.x - p0.x * p1.y;
        if (p0.x != p1.x) {
            k = double((p1.y - p0.y)) / (p1.x - p0.x);
            b = p1.y - k * p1.x;
            t = double(p1.x - p0.x) / (p1.y - p0.y);
            c = t * p0.x - p0.y;
        } else {
            k = 2;
            b = 0;
            t = double(p1.x - p0.x) / (p1.y - p0.y);
            c = t * p0.x - p0.y;
        }
    }
};

#endif
