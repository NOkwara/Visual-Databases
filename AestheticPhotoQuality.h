/*
 *  AestheticPhotoQuality.h
 *  VisualDatabasesProject
 *
 *  Created by Nnenna Okwara on 4/21/12.
 *  Copyright 2012 Columbia University. All rights reserved.
 *
 */
#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "ImageProcessing.h"
#include "DiscreteFourierTransform.h"
#include <vector>

float clarity_contrast(Mat image, Mat subject);
float lighting_feature(Mat3b image, Mat3b subject, BBox box);
float simplicity_feature(Mat3b image, Mat subject, BBox box);
void color_harmony_training(Mat3b image, vector<unsigned int> &hue, vector<unsigned int> &sat, vector<unsigned int> &val);
float composition_geometry_feature(Mat image, BBox box);
void computeAestheticPhotoQuality(Mat3b image, Mat image2, float &clarity, float &lighting, float &simplicity, float &composition, float &harmony);