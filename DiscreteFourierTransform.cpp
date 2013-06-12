/*
 *  DiscreteFourierTransform.cpp
 *  VisualDatabasesProject
 *
 *  Created by Nnenna Okwara on 4/3/12.
 *  Copyright 2012 Columbia University. All rights reserved.
 *
 */

#include "DiscreteFourierTransform.h"

/*
 Code to create image DFT is from:
 http://opencv.itseez.com/doc/tutorials/core/discrete_fourier_transform/discrete_fourier_transform.html
Mat I must be greyscaled
 */
//rings is used for creating CFD below
int createDFT(Mat org, Mat &magI){
	//Fourier Transformation to be done with Gray Scale of the image
	//Mat I = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
	Mat I;
	cvtColor(org, I, CV_RGB2GRAY);
	if(I.empty())
		return -1;
	
	Mat padded;
	int m = getOptimalDFTSize(I.rows);
	int n = getOptimalDFTSize(I.cols);
	copyMakeBorder(I, padded, 0, m-I.rows, 0, n-I.cols, BORDER_CONSTANT, Scalar::all(0));
	
	Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
	Mat complexI;
	merge(planes, 2, complexI);
	
	dft(complexI, complexI);
	
	split(complexI, planes);
	magnitude(planes[0], planes[1], planes[0]);
	magI = planes[0];
	
	magI += Scalar::all(1);
	log(magI, magI);
	
	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
	
	int cx = magI.cols/2;
	int cy = magI.rows/2;
	
    Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant 
    Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right
	
    Mat tmp;                           // swap quadrants
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
	
    q1.copyTo(tmp);                    // swap quadrant
    q2.copyTo(q1);
    tmp.copyTo(q2);
	
	/////////////////////////////////
    //normalize(magI, magI, 0, 1, CV_MINMAX); //histogram equalization
	Mat out, ino;
	magI.convertTo(out, CV_8UC1);
	equalizeHist(out, ino);
	
	ino.convertTo(out, CV_32F);
	magI  = out;
	 normalize(magI, magI, 0, 1, CV_MINMAX); //histogram equalization
	//return 1;
	// ONLY TO TEST DISPLAY IMAGE
    //imshow("Input Image"       , org   );    // Show the result
   // imshow("spectrum magnitude", magI);
	return 1;
}
///////////////////////////
//REST OF PAGE NO LONGER USED--> originally was to go image quality assessment of blurred and noise images.
//did not work properly so I scrapped it
/*
 Computes andd returns Cumulative Frequency Distribution given Fourier Transformation
 this algorithm comes from:
 */
void createCFD(const Mat MagI, vector<float> &energy_j){
	//v will do rows and u will do cols
	int rings = energy_j.size();
	float radius_u, radius_v, du = (MagI.cols / 2.0) / rings, dv = (MagI.rows / 2.0) / rings;
	int center_u = MagI.cols / 2, center_v = MagI.rows / 2;
	float Etotal = 0;
	
	//calculate total energy
	for(int i = 0; i < MagI.cols; i++) for(int j = 0; j < MagI.rows; j++){
		Etotal += pow(MagI.at<float>(j, i), 2);
	}
	//cout << Etotal << endl;
	//calculate energy in the rings
	//for each ring sum rings from 1 to (RINGS-j+1)--> so at ring 1 sum rings from 1 to 63 
	//ring 63 sum rings from 1 to 1
	radius_u = 0; radius_v = 0;
	//equation of an ellipse or circle a=b (x/a)^2 + (y/b)^2 = 1
	for(int i = rings - 1; i >= 0; i--){
		energy_j[i] = 0;
		radius_u += du; radius_v += dv;
		Rect roi(center_u - radius_u, center_v - radius_v, 2 * radius_u, 2 * radius_v);
		Mat picture = MagI(roi);
		imshow("pic", picture);
		for(int u = center_u - radius_u; u < center_u + radius_u; u++){
			for(int v = center_v - radius_v; v < center_v + radius_v; v++){
					//decide if point is in ellipse
					if ((pow((u - center_u) / radius_u, 2) + pow((v - center_v) / radius_v, 2)) <= 1)
						if(MagI.at<float>(v, u) >= 0 && MagI.at<float>(v, u) <= 1)
								energy_j[i] += pow(MagI.at<float>(v, u), 2);
			}
		}
		cout << energy_j[i] << endl;
	}
	
	/*for(int i = 0; i < rings; i++){
		cout << energy_j[i]<<endl;
	}*/
	
	float amount = energy_j[0], temp;
	cout << "energy" << endl;
	//normalize by dividing by cumulative energy in all rings
	for(int i = 0; i < rings; i++){
		energy_j[i] /= amount;
		cout << energy_j[i] <<endl;
	}
}	

/*
 using equation y = -(1/Hx)x + 1 find out if bar is on top og the line or below
 do this assuming Hx = RINGS
 */
void evaluate_Eplus_Eminus(const vector<float> energy_j, float &Eplus, float &Eminus){
	float rings = energy_j.size();
	float value;
	cout << value<<endl;
	for(float i = 0.0; i < rings; i++){
		value = -1 * (i + 1) / rings + 1;
		cout << value<<endl;
		if(energy_j[i] > value)
			Eplus += energy_j[i] - value;
		else 
			Eminus += value - energy_j[i];
	}
	float area = rings / 2;	//area of triangle of energy for cumulative frequency
	Eplus /= area;
	Eminus /= area;
}

