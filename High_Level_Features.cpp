/*
 *  HueCount_ColorDistribution.cpp
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
#include "High_Level_Features.h"
#include "ImageProcessing.h"
#include "DiscreteFourierTransform.h"
#include <vector>

//rings is used for creating CFD below
#define alpha 0.05
//rings is used for creating CFD below
#define EPLUS_THRESHOLD 0.035
//threshold for when blur is detected
#define EMINUS_THRESHOLD 0.4

int hue_count_quality(Mat3b image){
	int bins = 20;
	vector<int> histogram = hue_historgram(image,bins);
	int max = 0, n = 0, total = 0;
	
	int i = 0;
	
	//find maximum bin value
	while (n < bins){
		if(max < histogram[n])
			max = histogram[n];
		n++;
	}
	
	n = 0;
	
	//dont include in huecount empty bins messes it up
	int zerobins = 0;
	
	while(n < bins){
		if(histogram[n] > alpha * max ) total++;
		if(histogram[n] == 0) zerobins++;
		n++;
	}
	
	
	return 20 - total - zerobins;
}

/*
 returns a value n to quantify the noisyness of an image. 
 -1 means there is no noise detected
 Values close to 0 coincide with relatively noisy images, 
 while numbers near 1 are more suggestive of noisy pictures
 */
float evaluate_noise(const float Eplus, float expectedE){
	
	if(Eplus < EPLUS_THRESHOLD)
		return 1.0;
	
	//noise is detected
	return expectedE / 255;
}

float evaluate_blur(const float Eminus, float expectedE){
	if(Eminus < EMINUS_THRESHOLD)
		return 1.0;
	
	//blur detected
	return expectedE / 255;
}

void get_blur_noise_quality(Mat image, float &noise, float &blur){
	Mat magI;
	createDFT(image, magI);
	int rings = min(sqrt(magI.rows), sqrt(magI.cols)) * 3.5;
	vector<float> energy_j(rings, 0);
	createCFD(magI, energy_j);
	
	float Eplus = 0, Eminus = 0;
	evaluate_Eplus_Eminus(energy_j, Eplus, Eminus);
	cout << Eplus << " " << Eminus<< endl;
	//calculate expected value of energy
	float expectedE = 0;
	for(int i = 0; i < rings; i++)
		expectedE += energy_j[i];
	
	expectedE /= rings;
	
	noise = evaluate_noise( Eplus, expectedE);
	blur = evaluate_blur(Eminus, expectedE);
}
