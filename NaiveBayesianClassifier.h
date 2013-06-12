/*
 *  NiaveBayesianClassifier.h
 *  VisualDatabasesProject
 *
 *  Created by Nnenna Okwara on 5/10/12.
 *  Copyright 2012 Columbia University. All rights reserved.
 *
 */
#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "DiscreteFourierTransform.h"
#include "AestheticPhotoQuality.h"
#include "High_Level_Features.h"
#include "Low_Level_Features.h"
#include <vector>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <dirent.h>

double calculate_average(vector<float> values);
double calculate_variance(vector<float> values, double average);
void compute_training_average_variance(char *directory);
void color_harmony_training_run(char *directory);
int determine_image_quality(Mat3b image, Mat image2);
void image_quality_run(char *directory);