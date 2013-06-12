/*
 *  DiscreteFourierTransform.h
 *  VisualDatabasesProject
 *
 *  Created by Nnenna Okwara on 4/3/12.
 *  Copyright 2012 Columbia University. All rights reserved.
 *
 */
//#include "opencv2/core/mat.hpp"
#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

int createDFT(Mat org, Mat &magI);
void createCFD(const Mat MagI, vector<float> &energy_j);
void evaluate_Eplus_Eminus(const vector<float> energy_j, float &Eplus, float &Eminus);