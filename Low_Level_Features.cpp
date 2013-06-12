/*
 *  Low_Level_Features.cpp
 *  VisualDatabasesProject
 *
 *  Created by Nnenna Okwara on 4/12/12.
 *  Copyright 2012 Columbia University. All rights reserved.
 *
 */

#include "Low_Level_Features.h"
#include "ImageProcessing.h"

void compute_contrast_brightness(Mat3b image, int &contrast_quality, float &brightness_quality){
	
	int bins = 255;
	
	 vector<int> red_hist(bins+1, 0);
	 vector<int> green_hist(bins+1, 0);
	 vector<int> blue_hist(bins+1, 0);
	
	
	compute_colorHistorgrams(image, bins, red_hist, green_hist, blue_hist);
	vector<float> histogram = combine_histogram(bins, red_hist, green_hist, blue_hist);
	
	//COMPUTE CONTRAST QUALITY
	
	//calculate contrast quality by expanding outward until you get
	//to .98  total and return the width as the contrast quality
	//find begining of histogram
	int beginning = 0;
	float count = 0;
	while(beginning < histogram.size()){
		count += histogram[beginning];
		if(count > .01)
			break;
		beginning++;
	}
	int end = bins;
	count = 0;
	while(end >= 0){
		count += histogram[end];
		if(count > .01)
			break;
		end--;
	}
	
	contrast_quality = end - beginning;
	
	//COMPUTE BRIGHTNESS QUALITY
	float brightness = 0;
	for(int u = 0; u < image.rows; u++) for(int v = 0; v <image.cols; v++){
		Point point(v, u);
		Vec3b pixel = image(point);
		brightness += (pixel[0] + pixel[1] + pixel[2]) / 3.0;
	}
	brightness /= (float) (image.rows * image.cols); // number of pixels
	brightness /= 255.0; //divide by max channel lize to get %gray	
	brightness_quality = brightness;
}
