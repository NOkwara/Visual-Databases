/*
 *  HueCount_ColorDistribution.h
 *  VisualDatabasesProject
 *
 *  Created by Nnenna Okwara on 4/3/12.
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

int hue_count_quality(Mat3b image);
float evaluate_noise(const float Eplus, float expectedE);
float evaluate_blur(const float Eminus, float expectedE);
void get_blur_noise_quality(Mat image, float &noise, float &blur);