/*
 *  Low_Level_Features.h
 *  VisualDatabasesProject
 *
 *  Created by Nnenna Okwara on 4/12/12.
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

using namespace cv;
using namespace std;

void compute_contrast_brightness(Mat3b image, int &contrast_quality, float &brightness_quality);
