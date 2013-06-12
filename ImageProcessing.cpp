/*
 *  ImageProcessing.cpp
 *  VisualDatabasesProject
 *
 *  Created by Nnenna Okwara on 4/10/12.
 *  Copyright 2012 Columbia University. All rights reserved.
 *
 */

#include "ImageProcessing.h"
#include "opencv2/core/core.hpp"

#define hue_size 360

//algorithm gotten from 
//http://coecsl.ece.illinois.edu/ge423/spring05/group8/FinalProject/HSV_writeup.pdf

//NO LONGER BEING USED
HSV RGBtoHSV(float r, float g, float b){
//originally 0-255 make 0 to 1
	r /= 255.0; g /= 255.0; b /= 255.0;
	
	float min, max, delta;
	HSV hsv;
	
	min = MIN(r, MIN(g, b));
	max = MAX(r, MAX(g, b));
	
	hsv.value= max;
	delta = max - min;
	
	if( max != 0)
		hsv.saturation = delta / max;
	else { 
		//when pixel is black
		hsv.saturation = 0;
		hsv.hue = -1;
		return hsv;
	}
	
	if( r == max)
		hsv.hue = (g - b) / delta;
	else if (g == max)
		hsv.hue = 2 + (b - r) / delta;
	else
		hsv.hue = 4 + (r - g) / delta;
	
	hsv.hue *= 60;
	
	if(hsv.hue < 0)
		hsv.hue += 360;
	
	return hsv;

}

//NO LONGER BEING USED
HSV** convert_to_HSV(const Mat image){
	
	HSV **image_hsv = new HSV*[image.cols];
	for(int i = 0; i < image.cols; i++){
		image_hsv[i] = new HSV[image.rows];
	}
	
	Vec3b pixel;
	
	for(int x = 0; x < image.cols; x++) for(int y = 0; y < image.rows; y++) {
		pixel = image.at<Vec3b>(y, x); //in BGR order
		image_hsv[x][y] = RGBtoHSV(pixel[2], pixel[1], pixel[0]);
	}
	
	return image_hsv;
}

/*
 create hue histogram but only if saturation is above .2 for the pixel
 also value is between .15 and .95
 Rule is gotten from possible implmentation paper
 */
vector<int> hue_historgram(const Mat3b image, int bins){
	vector<int> histogram(bins, 0);
	
	//HSV** image_hsv = convert_to_HSV(image);
	Mat3b image_hsv;
	cvtColor(image, image_hsv, CV_BGR2HSV);
	
	//hue varies 0 to 179
	//saturation value 0 to 255
	int delta = 180 / bins;
	
	for(int x = 0; x < image.cols; x++) for(int y = 0; y < image.rows; y++){
		Point point(x, y);
		Vec3b pixel = image_hsv(point);
		if( pixel[1] > 51 && pixel[2] >= 38.25 && pixel[2] <= 242.25)
			histogram[(int) pixel[0] / delta ]++;
	}
	
	return histogram;	
}

void compute_colorHistorgrams(Mat3b image, int bins, vector<int> &red_hist, vector<int> &green_hist, vector<int> &blue_hist){
	int delta = 255 / bins; //255 max number for a color;
	
	//create historgram fo red blue and green channels
	for(int x = 0; x < image.cols; x++) for(int y = 0; y < image.rows; y++){
		Point point(x, y);
		Vec3b &pixel = image(point);
		red_hist[(int) pixel[2] / delta]++;
		green_hist[(int) pixel[1] / delta]++;
		blue_hist[(int) pixel[0] / delta]++;
	}
}

vector<float> combine_histogram(int bins, vector<int> &red_hist, vector<int> &green_hist, vector<int> &blue_hist){
	vector<float> histogram(bins+1, 0);
	
	float total = 0;
	
	for(int i = 0; i < bins; i++){
		histogram[i] = (float) red_hist[i] + green_hist[i] + blue_hist[i];
		total += (float) red_hist[i] + green_hist[i] + blue_hist[i];
	}
	
	//normalize historgram
	for(int i = 0; i < bins; i++)
		if(histogram[i] != 0)
			histogram[i] /= total;
	
	return histogram;
}

/*
following methods are image processing algorithms for Aesthetic implementatoin
done in grayscale
 */

BBox subjectRegionExtraction(Mat3b image){
	
	//IplImage* img = cvLoadImage(filename);
	//IplImage* out = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
	Mat3b out, img;
	Mat3b derivx, derivy;
	Mat U = Mat::zeros(image.rows, image.cols, CV_32F);
	Mat maxlk = Mat::zeros(image.rows, image.cols, CV_32F);//keep trak of mak
	cvtColor(image, img, CV_RGB2GRAY);
	//*very slow kuv9
	for(int k = 0; k < 50; k++){
		//cvSmooth(image, image, CV_BLUR, k, k);//box kernel blur
		Size kernel(k+1, k+1);
		boxFilter(image, out, -1, kernel);
		//filter image twice using kernel of [1,-1] and [1,-1]T
		
		Mat dx = Mat::Mat(1, 2, CV_32F);
		dx.at<float>(0,0) = 1; dx.at<float>(0,1) = -1;
		
		Mat dy = Mat::Mat(2, 1, CV_32F);
		dy.at<float>(0,0) = 1; dy.at<float>(1,0) = -1;
		
		filter2D(out, derivx, img.depth(), dx);
		
		filter2D(out, derivy, img.depth(), dy);
		
		//get the log likehoood for each pixel using window of size 3x3
		
		Mat lk = Mat::zeros(image.rows, image.cols, CV_32F);
		
		for(int u = 0; u < image.rows; u++) for(int v = 0; v < image.cols; v++){
			//the 3x3 window--> might want to split this to two n^3 trips much faster
			for(int i = u - 1; i <= u + 1; i++) for(int j = v - 1; j <= v + 1; j++){
				Point pt2(j, u);
				if(i > 0 && i < image.rows && j > 0 && j < image.cols){
					Vec3b dxP = derivx(pt2);
					Vec3b dyP = derivy(pt2);
					float magX = (dxP[0] + dxP[1] + dxP[2]) / 3.0;
					float magY = (dyP[0] + dyP[1] + dyP[2]) / 3.0;
					//float magY = 1;
					if(magX == 0) magX = 1;
					if(magY == 0) magY = 1;
					//cout << log(magX) + log(magY)<<endl;
					lk.at<float>(u,v) += log(magX) + log(magY);
				}
			}
		}
		for(int u = 0; u < image.rows; u++) for(int v = 0; v < image.cols; v++){
			if(lk.at<float>(u,v) > maxlk.at<float>(u,v)){
				maxlk.at<float>(u, v) = lk.at<float>(u, v);
				if(k != 1)
					U.at<float>(u, v) = 1;
			}
		}
	}
	
	vector<int> Ux(image.cols, 0);
	vector<int> Uy(image.rows, 0);
	//project U onto both x and y axes and get total energy
	int total = 0;
	for(int u = 0; u < image.rows; u++) for(int v = 0; v < image.cols; v++){
		if(U.at<float>(u, v) == 1){
			Ux[v]++;
			Uy[u]++;
			total++;
		}
	}
	float alpha = 0.9;//percent of energy to encase in the subject region
	//total to ignore
	int ignore = total * (1 - alpha) / 2.0;
	
	BBox box;
	
	int count = 0;
	// do x first
	for(int i = 0; i < image.cols; i++){
		count += Ux[i];
		if(count > ignore){
			box.minx = i + 1;
			break;
		}
	}
	count = 0;
	for(int i = image.cols - 1; i >= 0; i--){
		count += Ux[i];
		if(count > ignore){
			box.maxx = i - 1;
			break;
		}
	}
	count  = 0;
	//now do y
	for(int j = 0; j < image.rows; j++){
		count += Uy[j];
		if(count > ignore){
			box.miny = j + 1;
			break;
		}
	}
	count = 0;
	for(int j = image.rows - 1; j >= 0; j--){
		count += Uy[j];
		if(count > ignore){
			box.maxy = j - 1;
			break;
		}
	}
	imshow("U", U);	
	return box;	
}
	
	
			
		
