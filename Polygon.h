#ifndef POLYGON_H
#define POLYGON_H
#include <opencv2/opencv.hpp>
#include <vector>
using namespace cv;
using namespace std;

class Polygon {
public:
	//存首尾相连的点
    vector<cv::Point>* points;
    Polygon(vector<cv::Point> *p) {
        points = p;
	}
};

#endif
