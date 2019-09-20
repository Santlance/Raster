#define RASTER_CPP
#ifdef RASTER_CPP
#include "Circle.h"
#include "Line.h"
#include "Polygon.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stack>

using namespace std;
using namespace cv;

// assum the picture is 1024 * 768, begin at (0,0)
Mat picture(768, 1024, CV_8UC3, Scalar(0, 0, 0));
Mat picture1(768, 1024, CV_8UC3, Scalar(0, 0, 0));
// judge the pixel is inside or outside the polygon
Mat symbol(786, 1024, CV_8UC1, Scalar(0));

void drawSymbol(int x, int y) {
    symbol.at<uchar>(x, y) = 1;
}

void drawpixel(int x, int y, Vec3b color, Mat picture) {
    picture.at<Vec3b>(x, y) = color;
    //cout << "( " << x << " , " << y << " )" << endl;
}
// ƽ�Ƶ�Ӧ��ȫ�� x y
void CirclePoints(int x, int y, cv::Point c, Vec3b color, Mat picture) {
    drawpixel(x + c.x, y + c.y, color, picture);
    drawpixel(y + c.x, x + c.y, color, picture);
    drawpixel(-x + c.x, y + c.y, color, picture);
    drawpixel(y + c.x, -x + c.y, color, picture);
    drawpixel(x + c.x, -y + c.y, color, picture);
    drawpixel(-y + c.x, x + c.y, color, picture);
    drawpixel(-x + c.x, -y + c.y, color, picture);
    drawpixel(-y + c.x, -x + c.y, color, picture);
}

// digital diffrential analyzer
// int(y + 0.5)��Ŀ����ȡ����������������ص�
// �������� |k| <= 1�����|k| > 1�� x y�÷�����.��Ϊk > 1����ֶϲ�
// �����㷨������Ӳ��ʵ��
// proof it right
void DDALine(Line line) {
    int x;
    double y, k;
    k = line.k;
    if (abs(k) <= 1.0) {
        y = line.pts.y;
        for (x = line.pts.x; x <= line.pte.x; x++) {
            drawpixel(x, int(y + 0.5), Vec3b(255, 255, 0), picture);
            y = y + k;
        }
    } else {
        k = line.t;
        y = line.y_s.x;
        for (x = line.y_s.y; x <= line.y_e.y; x++) {
            drawpixel(int(y + 0.5), x, Vec3b(255, 255, 0), picture);
            y = y + k;
        }
    }
}
void PolyDDALine(Line line) {
    int x;
    double y, k;
    k = line.k;
    if (abs(k) <= 1.0) {
        y = line.pts.y;
        for (x = line.pts.x; x <= line.pte.x; x++) {
            drawpixel(x, int(y + 0.5), Vec3b(255, 255, 0), picture);
            drawSymbol(x, int(y + 0.5));
            y = y + k;
        }
    } else {
        k = line.t;
        y = line.y_s.x;
        for (x = line.y_s.y; x <= line.y_e.y; x++) {
            drawpixel(int(y + 0.5), x, Vec3b(255, 255, 0), picture);
            drawSymbol(int(y + 0.5), x);
            y = y + k;
        }
    }
}
// middle point analyzer
// Ҫ�� k <= 1 �� > 1�����������ΪҪ��֤ÿ�ζ����Һ���������ѡ��
void MidPointLine(Line line) {
    double k = line.k;
    int d, d1, d2, x, y, a, b;
    if (abs(k) <= 1) {
        a = line.pts.y - line.pte.y;
        b = line.pte.x - line.pts.x;
        d = 2 * a + b;
        d1 = 2 * a;
        d2 = 2 * (a + b);
        x = line.pts.x;
        y = line.pts.y;
        drawpixel(x, y, Vec3b(255, 255, 0), picture);
        while (x < line.pte.x) {
            //cerr << d << endl;
            if (d < 0) {
                x++;
                y++;
                d += d2;
            } else {
                x++;
                d += d1;
            }
            drawpixel(x, y, Vec3b(255, 255, 0), picture);
        }
    } else {
        a = line.pts.x - line.pte.x;
        b = line.pte.y - line.pts.y;
        d = 2 * a + b;
        d1 = 2 * a;
        d2 = 2 * (a + b);
        x = line.pts.y;
        y = line.pts.x;
        drawpixel(x, y, Vec3b(255, 255, 0), picture);
        while (y < line.pte.y) {
            //cerr << d << endl;
            if (d < 0) {
                x++;
                y++;
                d += d2;
            } else {
                y++;
                d += d1;
            }
            drawpixel(x, y, Vec3b(255, 255, 0), picture);
        }
    }
}

// ����ֻ��Ҫe�ķ��ţ����� e' = 2 * e * dx�滻�Ӷ��Ľ�Ϊ�����㷨
void BresenhamLine(Line line) {
    int x, y;
    double k, e;
    k = line.k;
    e = -0.5;
    if (abs(k) <= 1) {
        x = line.pts.x;
        y = line.pts.y;
        for (; x <= line.pte.x; x++) {
            drawpixel(x, y, Vec3b(255, 255, 0), picture);
            e = e + k;
            if (e >= 0) {
                y++;
                e = e - 1;
            }
        }
    } else {
        k = 1.0 / k;
        x = line.pts.x;
        y = line.pts.y;
        for (; y <= line.pte.y; y++) {
            drawpixel(x, y, Vec3b(255, 255, 0), picture);
            e = e + k;
            if (e >= 0) {
                e = e - 1;
                x++;
            }
        }
    }
}
// ���forѭ���Ĵ�С����xΪ��������x������yΪ��������y����
void IntegerBresenhamLine(Line line) {
    int x, y, dx, dy, e;
    static int color = 0;
    dx = line.pte.x - line.pts.x;
    dy = line.pte.y - line.pts.y;
    e = -dx;
    x = line.pts.x;
    y = line.pts.y;
    for (int i = 0; i <= dx; i++) {
        drawpixel(x, y, Vec3b(255, color, 255 - color), picture);
        x++;
        e = e + 2 * dy;
        if (e >= 0) {
            y++;
            e = e - 2 * dx;
        }
    }
    color += 20;
}

void MidPointCircle(Circle circle) {
    int x, y, r;
    cv::Point c = circle.center;
    double d;
    r = circle.radius;
    x = 0;
    y = r;
    d = 1.25 - r;
    CirclePoints(x, y, c, Vec3b(255, 255, 0), picture);
    while (x <= y) {
        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
        CirclePoints(x, y, c, Vec3b(255, 255, 0), picture);
    }
}

// d' = 4 * d, r' = int(r + 0.5)
void IntegerMidPointCircle(Circle circle) {
    int x, y, d, r;
    cv::Point c = circle.center;
    r = int(circle.radius + 0.5);
    x = 0;
    y = r;
    d = 5 - 4 * r;
    CirclePoints(x, y, c, Vec3b(255, 255, 0), picture);
    while (x <= y) {
        if (d < 0) {
            d += 8 * x + 12;
        } else {
            d += 8 * (x - y) + 20;
            y--;
        }
        x++;
        CirclePoints(x, y, c, Vec3b(255, 255, 0), picture);
    }
}

void drawPolygonLine(Polygon polygon) {
    int s = polygon.points->size();
    cout << s << endl;
    vector<Point> p = *polygon.points;
    for (int i = 0; i < s; i++) {
        Line l(p[i % s], p[(i + 1) % s]);
        cout << p[i % s].x << " " << p[i % s].y << endl;
        PolyDDALine(l);
    }
}
//�ñ߽�����־������ȱ©
void edgemark_fill() {
    for (int y = 0; y < 1024; y++) {
        bool inside = false;
        for (int x = 0; x < 768; x++) {
            if (symbol.at<uchar>(x, y) == 1) {
                inside = !inside;
            }
            if (inside) {
                drawpixel(x, y, Vec3b(255, 100, 100), picture);
            }
        }
    }
}

void BoundaryFill4(int x, int y) {
    if (symbol.at<uchar>(x, y) == 0) {
        drawpixel(x, y, Vec3b(255, 100, 100), picture);
        symbol.at<uchar>(x, y) = 2;
        BoundaryFill4(x, y + 1);
        BoundaryFill4(x, y - 1);
        BoundaryFill4(x - 1, y);
        BoundaryFill4(x + 1, y);
    }
}

//�����ӵݹ�ã��ķ���ݹ�
//�������
void scanLineFill4(int x, int y) {
    int xl, xr, i;
    bool spanNeedFill;
    cv::Point seed;
    stack<cv::Point> stack;
    seed.x = x;
    seed.y = y;
    stack.push(seed); // ��ǰ�����ɵ�����ѹ��ջ
    while (!stack.empty()) {
        seed = stack.top();
        stack.pop();
        x = seed.x;
        y = seed.y;
        while (symbol.at<uchar>(x, y) == 0) {//������䲢���
            drawpixel(x, y, Vec3b(255, 100, 100), picture);
            symbol.at<uchar>(x, y) = 1;
            x++;
        }
        xr = x - 1;//����ѭ����ʱ��xӦ�Ǳ߽磬��xr ���ұ߽�Ϊx - 1
        x = seed.x - 1;//��ʱ�������ӵ�����
        while (symbol.at<uchar>(x, y) == 0) {//�������
            drawpixel(x, y, Vec3b(255, 100, 100), picture);
            symbol.at<uchar>(x, y) = 1;
            x--;
        }
        xl = x + 1;//����ѭ����ʱ��x�Ǳ߽磬��xl����߽�Ϊx + 1
		//��Ϊ��դ��ͼ���У�ֱ���������ģ�x+ 1��Ȼ�Ǳ߽�����ڲ�
		//����seed֮�ϵ�һ��ɨ����
        x = xl;//
        y = y + 1;
        while (x <= xr) {///ͨ�����ѭ��������һ��ɨ���ߵ�����
            spanNeedFill = false;
            while (symbol.at<uchar>(x, y) == 0) { //�ҵ����ҵ�seed
                spanNeedFill = true;
                x++;
            }
            if (spanNeedFill) {
                seed.x = x - 1;
                seed.y = y;
                stack.push(seed);
                spanNeedFill = false;
            }
            while (symbol.at<uchar>(x, y) != 0 && x <= xr)//�������Ҳ�����һֱ����
                x++;
        }
		//����ԭ��seed�������ɨ���ߣ�����Ҫ - 2
        x = xl;
        y = y - 2;
        while (x <= xr) {
            spanNeedFill = false;
            while (symbol.at<uchar>(x, y) == 0) {
                spanNeedFill = true;
                x++;
            }
            if (spanNeedFill) {
                seed.x = x - 1;
                seed.y = y;
                stack.push(seed);
                spanNeedFill = false;
            }
            while (symbol.at<uchar>(x, y) != 0 && x <= xr)
                x++;
        }
    }
}

int main() {
    for (int i = 0; i < 768; i++) {
        symbol.at<uchar>(i, 0) = 1;
        symbol.at<uchar>(i, 1023) = 1;
    }
    for (int i = 0; i < 1024; i++) {
        symbol.at<uchar>(0, i) = 1;
        symbol.at<uchar>(767, i) = 1;
    }
    namedWindow("DDApicture");
    vector<cv::Point> p { cv::Point(464, 512), cv::Point(424, 542), cv::Point(424, 592), cv::Point(384, 562), cv::Point(344, 592), cv::Point(344, 542),
        cv::Point(304, 512), cv::Point(344, 482), cv::Point(344, 432), cv::Point(384, 462), cv::Point(424, 432), cv::Point(424, 482) };
    Polygon polygon(&p);
    Line line(Point(0, 0), Point(767, 1023));
    Circle circle(cv::Point(384, 512), 50);
    DDALine(line);
    drawPolygonLine(polygon);

    scanLineFill4(384, 512);
    //BoundaryFill4(384, 512);
    MidPointCircle(circle);
    cout << "end" << endl;
    //edgemark_fill();
    imshow("DDApicture", picture);
    //namedWindow("Midpicture");
    //imshow("Midpicture", picture1);
    waitKey(0);
    return 0;
}

#endif