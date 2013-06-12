/*
 *  AestheticPhotoQuality.cpp
 *  VisualDatabasesProject
 *
 *  Created by Nnenna Okwara on 4/21/12.
 *  Copyright 2012 Columbia University. All rights reserved.
 *
 */

#include "AestheticPhotoQuality.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <dirent.h>

/*
 Measure the clarity of the subject vs region as a whole
 using Fast fourier Transform of the Image
 */
float clarity_contrast(Mat image, Mat subject){
	Mat dftI, dftS;
	createDFT(image, dftI);
	createDFT(subject, dftS);
	//imshow("dftI", dftI);
	//imshow("dftS", subject);
	//max frequency is 1 and beta is .2 add to MI if dftI[u,v] > .2
	float beta = .2;
	float maxI = 0, maxS = 0;
	float MI = 0, MS = 0;
	float areaS = subject.rows * subject.cols;
	float areaI = image.rows*image.cols;
	//find max dftI value
	for(int u = 0; u < image.cols; u++) for(int v = 0; v < image.rows; v++){
		if(maxI < dftI.at<float>(v, u)) maxI = dftI.at<float>(v, u);
		
	}
	//find max dftS value
	for(int u = 0; u < subject.cols; u++) for(int v = 0; v < subject.rows; v++){
		if(maxS < dftS.at<float>(v, u)) maxS = dftS.at<float>(v, u);
	}
	
	for(int u = 0; u < image.cols; u++) for(int v = 0; v < image.rows; v++){
		if(pow(dftI.at<float>(v, u),2) > beta * maxI)
			MI++;
	}
	
	for(int u = 0; u < subject.cols; u++) for(int v = 0; v < subject.rows; v++){
		if(pow(dftS.at<float>(v, u), 2) > beta * maxS)
			MS++;
	}
	MS /= areaS;
	MI /= areaI;
	
	
	return (float) MS / MI;
}

/*
 compare Brightness of the subject to brightness of the image
 */
float lighting_feature(Mat3b image, Mat3b subject, BBox box){
	Mat3b image_hsv, sub_hsv;
	cvtColor(image, image_hsv, CV_BGR2HSV);
	cvtColor(subject, sub_hsv, CV_BGR2HSV);
	float areaS = subject.rows * subject.cols, areaI = image.rows * image.cols;
	Vec3b pixel;
	float BI = 0, BS = 0;
	for(int u = 0; u < image.cols; u++) for(int v = 0; v < image.rows; v++){
		Point pt(u, v);
		pixel = image(pt);
		if((u > box.minx && u < box.maxx) && (v > box.miny && v < box.maxy)){
			//BS += sqrt(pixel[0]*pixel[0] + pixel[1]*pixel[1] + pixel[2] * pixel[2]);
			BS += pixel[2];
		}
		else{
			//BI += sqrt(pixel[0]*pixel[0] + pixel[1]*pixel[1] + pixel[2] * pixel[2]);
			BI += pixel[2];
		}
	}
	
	BI /= (float) (areaI);// - areaS);
	
	BS /= (float) areaS;
	
	return abs(log10(BS / BI));
}

/*
 Simplcity of the image by looking at the histogram 
 and counting bumber of bins over a certain threshold
 */
float simplicity_feature(Mat3b image, Mat subject, BBox box){
	//equation for bin (R/16)*255 + (G/16)*16 + B / 16
	int totalBins = 4096;
	vector<int> hist (totalBins, 0);
	Vec3b pixel;
	int bin;
	float gamma = 0.01;
	float total = 0;
	for(int u = 0; u < image.cols; u++) for(int v = 0; v < image.rows; v++){
		if((u < box.minx || u > box.maxx) && (v < box.miny || v < box.maxy)){
			Point pt (u , v);
			pixel = image(pt);
			bin = (pixel[2] / 16) * 256 + (pixel[1] / 16) * 16 + pixel[0] / 16;
			hist[bin]++;
		}
	}
	
	vector<int>::iterator it;
	int hmax = 0;
	
	for(it = hist.begin(); it < hist.end(); it++){
		if(*it > hmax)
			hmax = *it;
	}
	
	float S = 0;
	for(it = hist.begin(); it < hist.end(); it++){
		if(*it >= gamma * hmax)
			S++;
	}
	
	return (float) (S / totalBins) * 100.0;
}

/*
 Rule of thirds Check:
 how close is the subject to the 4 points in Rule of thirds ideal
 For subject placement
 */
float composition_geometry_feature(Mat image, BBox box){
	//get the centroid of the subject inside the image
	double Cx = box.minx + .5 * (box.maxx - box.minx);
	double Cy = box.miny + .5 * (box.maxy - box.miny);
	
	//now do the four lines to break of image into thirds
	double Ix1 = image.cols / 3;
	double Ix2 = image.cols * 2 / 3;
	
	double Iy1 = image.rows / 3;
	double Iy2 = image.rows * 2 / 3;
	
	//calculate four fm's
	
	float fm1 = sqrt(pow(Cx - Ix1, 2.0) / pow(image.cols, 2.0) + pow(Cy - Iy1, 2.0) / pow(image.rows, 2.0));
	float fm2 = sqrt(pow(Cx - Ix1, 2.0) / pow(image.cols, 2.0) + pow(Cy - Iy2, 2.0) / pow(image.rows, 2.0));
	float fm3 = sqrt(pow(Cx - Ix2, 2.0) / pow(image.cols, 2.0) + pow(Cy - Iy1, 2.0) / pow(image.rows, 2.0));
	float fm4 = sqrt(pow(Cx - Ix2, 2.0) / pow(image.cols, 2.0) + pow(Cy - Iy2, 2.0) / pow(image.rows, 2.0));
	
	return min(fm1, min(fm2, min(fm3, fm4)));
}

/*
 Training for Color harmony on good and bad image color coordinations
 */
void color_harmony_training(Mat3b image, vector<unsigned int> &hue, vector<unsigned int> &sat, vector<unsigned int> &val){
	//creating 50 bin histograms for HSV
	Mat3b image_hsv;
	cvtColor(image, image_hsv, CV_BGR2HSV);
	//h 0 to 179, sv 0 to 255
	float deltaH = 179.0 / 50.0;
	float deltaSV = 255.0 / 50.0;
	for(int u = 0; u < image.cols; u++) for(int v = 0; v < image.rows; v++){
		Point pt(u, v);
		Vec3b pix = image_hsv(pt);
		hue[pix[0] / deltaH]++;
		sat[pix[1] / deltaSV]++;
		val[pix[2] / deltaSV]++;
	}
}

/*
 Check the histogram difference for images in HSV
 */
float color_harmony_feature(Mat3b image){
	vector<unsigned int> hue, sat, val;
	hue.resize(50, 0); sat.resize(50, 0); val.resize(50, 0);
	//set up the high and low histograms precomputed values
	float highH[] = {55695, 9804, 9142, 20395, 15805, 17127, 10317, 4525, 7023, 4231, 4125, 2521,
		2698, 2625, 1197, 1562, 1404, 645, 1086, 626, 2176, 380, 1050, 1153, 1050, 1050, 1821, 
		2247, 2701, 5831, 8776, 7387, 7234, 4011, 4880, 2968, 1709, 1332, 1103, 492, 1467, 679,
		7225, 347, 876, 842, 739, 3036, 1686, 2609, 2500};
	float highS[] = {53000, 7391, 6077, 5754, 5444, 5227, 4956, 4806, 4453, 4339, 5121, 4298, 4145,
		4134, 3963, 3982, 4110, 3732, 3775, 3657, 4712, 3556, 3500, 3430, 3135, 4030, 3207, 3284,
		3073, 2909, 3808, 3002, 2718, 3353, 2520, 2835, 2538, 2730, 2470, 2260, 2975, 2575, 2506,
		2414, 2342, 2562, 2662, 2784, 3060, 3764};
	float highV[] = {19779, 6857, 5730, 5214, 4959, 4723, 4739, 4633, 4512, 4454, 5352, 4422, 4419,
		4361, 4382, 4376, 4402, 4354, 4351, 4377, 5294, 4273, 4203, 4182, 4160, 4118, 4142, 4131,
		4127, 4131, 5015, 4212, 4251, 4244, 4195, 4110, 4094, 4061, 4032, 4062, 4894, 4066, 4097,
		4169, 4058, 4053, 3985, 4094, 4201, 6096};
	float lowH[] = {43207, 9298, 9059, 17123, 12556, 13031, 9167, 4227, 7173, 4411, 4211, 2601, 2787,
		2769, 1344, 1758, 1377, 775, 1319, 871, 2561, 479, 1595, 1578, 1238, 2389, 2460, 3123, 5239,
		6549, 5185, 5268, 3095, 3782, 2676, 1431, 975, 874, 396, 1262, 745, 674, 316, 863, 834, 752,
		2457, 1872, 3129, 3291};
	float lowS[] = {40103, 7685, 6262, 6015, 5796, 5512, 5135, 5020, 4816, 4730, 5532, 4534, 4297, 4197,
		3992, 3858, 3892, 3534, 3519, 3399, 4284, 3214, 3146, 3070, 2813, 3550, 2819, 2815, 2595, 2549,
		3176, 2441, 2267, 2778, 2177, 2394, 2236, 2290, 2131, 1998, 2564, 2102, 2045, 1912, 1852, 2050,
		1949, 2046, 2293, 2566};
	float lowV[] = {12791, 5003, 4407, 4432, 4254, 4104, 4024, 3955, 3904, 3825, 4619, 3852, 3887, 3934,
		3926, 3971, 3919, 3906, 3854, 3831, 4643, 3907, 3945, 3969, 4072, 4036, 4006, 4009, 4007, 3982,
		4801, 3986, 3889, 3875, 3880, 3860, 3754, 3710, 3764, 3761, 4460, 3628, 3595, 3547, 3532, 3427,
		3456, 3657, 3902, 6336};
	//vector<int> fifth (myints, myints + sizeof(myints) / sizeof(int) );
	vector<float> Hhue (highH, highH + sizeof(highH) / sizeof(float));
	vector<float> Hsat (highS, highS + sizeof(highS) / sizeof(float));
	vector<float> Hval (highV, highV + sizeof(highV) / sizeof(float));
	vector<float> Lhue (lowH, lowH + sizeof(lowH) / sizeof(float));
	vector<float> Lsat (lowS, lowS + sizeof(lowS) / sizeof(float));
	vector<float> Lval (lowV, lowV + sizeof(lowV) / sizeof(float));
	color_harmony_training(image, hue, sat, val);
	
	float hueH, hueL, satH, satL, valH, valL;
	
	//calculate the distances between the histograms and the input image histograms
	//normalize histograms
	
	//1. calculate avergage of the each histogram
	float avgHH = 0, avgLH = 0, avgHS = 0, avgLS = 0, avgHV = 0, avgLV = 0, avgH = 0, avgS = 0, avgV = 0;
	for(int i  = 0; i < 50; i++){
		avgHH += Hhue[i]; avgLH += Lhue[i]; avgHS += Hsat[i];
		avgLS += Lsat[i]; avgHV += Hval[i]; avgLV += Lval[i];
		avgH += hue[i]; avgS += sat[i]; avgV += val[i];
	}
	//2. normalize histograms
	/*for(int i = 0; i < 50; i++){
	 Hhue[i] /= avgHH; Lhue[i] /= avgLH; Hsat[i] /= avgHS;
	 Lsat[i] /= avgLS; Hval[i] /= avgHV; Lval[i] /= avgLV;
	 hue[i] /= avgH; sat[i] /= avgS; val[i] /= avgV;
	 }*/
	
	avgHH /= 50.0; avgLH /= 50.0; avgHS /= 50.0; avgLS /= 50.0; avgHV /= 50.0; avgLV /= 50.0;
	avgH /= 50.0; avgS /= 50.0; avgV /= 50.0;
	
	//now calculate histogram correlation for each
	float sum1 = 0, sum4 = 0, sum2 = 0, sum3 = 0;
	for(int i = 0; i < 50; i++){
		if(i % 2 == 0)
			sum1 += (Hhue[i] - avgHH) * (hue[i] - avgH);
		else 
			sum4 += (Hhue[i] - avgHH) * (hue[i] - avgH);
		sum2 += pow(Hhue[i] - avgHH, 2);
		sum3 += pow(hue[i] - avgH, 2);
	}
	if(sum2 == 0) sum2 = 1; 
	if(sum3 == 0) sum1 = 1;
	hueH = (float) ((sum1 + sum4) / sqrt(sum2 * sum3));
	sum1 = 0; sum2 = 0; sum4 = 0;
	
	for(int i = 0; i < 50; i++){
		if(i % 2 == 0)
			sum1 += (Lhue[i] - avgLH) * (hue[i] - avgH);
		else
			sum4 += (Lhue[i] - avgLH) * (hue[i] - avgH);
		sum2 += pow(Lhue[i] - avgLH, 2);
	}
	if(sum2 == 0) sum2 = 1; 
	if(sum3 == 0) sum1 = 1;
	hueL = (float) ((sum1 + sum4) / sqrt(sum2 * sum3));
	sum1 = 0; sum2 = 0; sum3 = 0; sum4 = 0;
	
	for(int i = 0; i < 50; i++){
		if(i % 2 == 0)
			sum1 += (Hsat[i] - avgHS) * (sat[i] - avgS);
		else 
			sum4 += (Hsat[i] - avgHS) * (sat[i] - avgS);
		
		sum2 += pow(Hsat[i] - avgHS, 2);
		sum3 += pow(sat[i] - avgS, 2);
	}
	if(sum2 == 0) sum2 = 1; 
	if(sum3 == 0) sum1 = 1;
	satH = (float) ((sum1 + sum4) / sqrt(sum2 * sum3));
	sum1 = 0; sum2 = 0; sum4 = 0;
	
	for(int i = 0; i < 50; i++){
		if(i % 2 == 0)
			sum1 += (Lsat[i] - avgLS) * (sat[i] - avgS);
		else
			sum4 += (Lsat[i] - avgLS) * (sat[i] - avgS);
		sum2 += pow(Lsat[i] - avgLS, 2);
	}
	if(sum2 == 0) sum2 = 1; 
	if(sum3 == 0) sum1 = 1;
	satL = (float) ((sum1 + sum4) / sqrt(sum2 * sum3));
	sum1 = 0; sum2 = 0; sum3 = 0; sum4 = 0;
	
	for(int i = 0; i < 50; i++){
		if(i % 2 == 0)
			sum1 += (Hval[i] - avgHV) * (val[i] - avgV);
		else
			sum4 += (Hval[i] - avgHV) * (val[i] - avgV);
		sum2 += pow(Hval[i] - avgHV, 2);
		sum3 += pow(val[i] - avgV, 2);
	}
	if(sum2 == 0) sum2 = 1; 
	if(sum3 == 0) sum1 = 1;
	valH = (float) ((sum1 + sum4) / sqrt(sum2 * sum3));
	sum1 = 0; sum2 = 0; sum4 = 0;
	
	for(int i = 0; i < 50; i++){
		if(i % 2 == 0)
			sum1 += (Lval[i] - avgLV) * (val[i] - avgV);
		else
			sum4 += (Lval[i] - avgLV) * (val[i] - avgV);
		sum2 += pow(Lval[i] - avgLV, 2);
	}
	if(sum2 == 0) sum2 = 1; 
	if(sum3 == 0) sum1 = 1;
	valL = (float) ((sum1 + sum4) / sqrt(sum2 * sum3));
	
	return (hueH / hueL) * (satH / satL) * (valH / valL);
}

void computeAestheticPhotoQuality(Mat3b image, Mat image2, float &clarity, float &lighting, float &simplicity, float &composition, float &harmony){
	BBox box = subjectRegionExtraction(image);
	
	Rect roi(box.minx, box.miny, box.maxx-box.minx, box.maxy-box.miny);
	Mat3b subject = image2(roi);
	//imshow("lol",subject);
	Mat subject2 = image(roi);
	
	
	clarity = clarity_contrast(image2, subject2);
	
	lighting = lighting_feature(image, subject2, box);
	
	simplicity = simplicity_feature(image, subject, box);
	
	composition = composition_geometry_feature(image2, box);
	
	harmony = color_harmony_feature(image);
}

	