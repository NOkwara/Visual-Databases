/*
 *  ImageProcessing.h
 *  VisualDatabasesProject
 *
 *  Created by Nnenna Okwara on 4/10/12.
 *  Copyright 2012 Columbia University. All rights reserved.
 *
 */

#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

struct HSV {
	//hue 0-360; saturation 0-1; value 0-1
	float hue, saturation, value;
	HSV(float h, float s, float v) : hue(h), saturation(s), value(v) {}
	HSV(){}
};

struct BBox{
	int minx, maxx, miny, maxy;
	BBox(int mix, int miy, int max, int may) : minx(mix), miny(miy), maxx(max), maxy(may){}
	BBox(){};
};

HSV RGBtoHSV(float r, float g, float b);
HSV** convert_to_HSV(const Mat image);
vector<int> hue_historgram(const Mat3b image, int bins);
void compute_colorHistorgrams(Mat3b image, int bins, vector<int> &red_hist, vector<int> &green_hist, vector<int> &blue_hist);
vector<float> combine_histogram(int bins, vector<int> &red_hist, vector<int> &green_hist, vector<int> &blue_hist);
BBox subjectRegionExtraction(Mat3b image);