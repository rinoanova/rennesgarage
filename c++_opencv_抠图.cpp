#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cvaux.h>
#include "opencv2/imgproc/imgproc.hpp"
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

Mat src; Mat src_gray;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

/// Function header
void thresh_callback(int, void* );

bool GreaterSort (vector<Point> a, vector<Point> b) { return (a.size()>b.size()); }

/** @function main */
int main( int argc, char** argv )
{
    /// 加载源图像
    src = imread( "/Users/renne/Desktop/3mini.png", 1 );
    
    /// 转成灰度并模糊化降噪
    cvtColor( src, src_gray, CV_BGR2GRAY );
    blur( src_gray, src_gray, Size(3,3) );
    
    /// 创建窗体
    char* source_window = "Source";
    namedWindow( source_window, CV_WINDOW_AUTOSIZE );
    imshow( source_window, src );
    
    createTrackbar( " Canny thresh:", "Source", &thresh, max_thresh, thresh_callback );
    thresh_callback( 0, 0 );
    
    waitKey(0);
    return(0);
}

/** @function thresh_callback */
void thresh_callback(int, void* )
{
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    /// 用Canny算子检测边缘
    Canny( src_gray, canny_output, thresh, thresh*2, 3 );
    /// 寻找轮廓
    findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    
    /// 绘出轮廓
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    
    // 按轮廓长度降序排列
    sort(contours.begin(), contours.end(), GreaterSort);
    //vector<vector<Point> >::iterator iter;
    /*
    vector<vector<Point> >::iterator iter;
    for(iter=contours.begin();iter!=contours.end();iter++){
        printf("%d\n",iter->size());
    }
     */
    
    // 找到镂空
    vector<vector<Point> > polyContours(contours.size());
    int minArea = 300; //constant
    int j = 0;
    for(int i=0;i<contours.size();i++){
        if(contourArea(contours[i])>minArea){
            approxPolyDP(contours[i], polyContours[j], 10, false);
            j++;
        }
    }
    for(int i=0; i<j;i++){
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours(drawing, polyContours, i, color, 2);
    }
    drawContours(drawing, contours, 0, Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) ), 2);
    
    //for( int i = 0; i< contours.size(); i++ )
    /*
    for(int i=0; i<15; i++)
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }
    */
    /// 在窗体中显示结果
    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    imshow( "Contours", drawing );
}
