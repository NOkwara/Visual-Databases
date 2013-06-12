/*
 *  NiaveBayesianClassifier.cpp
 *  VisualDatabasesProject
 *
 *  Created by Nnenna Okwara on 5/10/12.
 *  Copyright 2012 Columbia University. All rights reserved.
 *
 */

#include "NaiveBayesianClassifier.h"
#define PI 3.1415926535897932384626433

double calculate_average(vector<float> values){
	double total = 0;
	for(int i = 0; i < values.size(); i++)
		total += values[i];
	return total / values.size();
}
double calculate_variance(vector<float> values, double average){
	double expectedV = 0;
	for(int i = 0; i < values.size(); i++){
		if(abs(values[i] - average) < 10)
		expectedV += pow(values[i] - average, 2.0);
	}
	return expectedV / values.size();
}
		
void compute_training_average_variance(char *directory){
	vector<float> bright, contrast, huecount, clarity, lighting, simplicity, composition, harmony;
	float bri, hue, clar, light, simp, comp, harm;
	int con;
	DIR *pDIR;
	struct dirent *entry;
	if(pDIR = opendir(directory)){
		while(entry = readdir(pDIR)){
			if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".DS_Store") != 0){
				char name[200];
				strcpy(name, directory);
				strcat(name, "/");
				strcat(name, entry->d_name);
				char* imageN = name;
				//cout << name << endl;
				Mat3b image = imread(imageN, CV_LOAD_IMAGE_COLOR);
				Mat image2 = imread(imageN, CV_LOAD_IMAGE_COLOR);
				if(!image.data) cout<<"no image file detected"<<endl;
				compute_contrast_brightness(image, con, bri);
				contrast.push_back(con);
				bright.push_back(bri);
				hue = hue_count_quality(image);
				huecount.push_back(hue);
				//computeAestheticPhotoQuality(image2, image, clar, light, simp, comp, harm);
				clarity.push_back(clar);
				lighting.push_back(light);
				simplicity.push_back(simp);
				composition.push_back(comp);
				harmony.push_back(harm);
			}
		}
	}
	
	double average;
	average = calculate_average(contrast);
	cout << "contrast: average = " << average << " variance = " << calculate_variance(contrast, average) << endl;
	average = calculate_average(bright);
	cout << "bright: average = " << average << " variance = " << calculate_variance(bright, average) << endl;
	average = calculate_average(huecount);
	cout << "huecount: average = " << average << " variance = " << calculate_variance(huecount, average) << endl;
	average = calculate_average(clarity);
	cout << "clarity: average = " << average << " variance = " << calculate_variance(clarity, average) << endl;
	average = calculate_average(lighting);
	cout << "lighting: average = " << average << " variance = " << calculate_variance(lighting, average) << endl;
	average = calculate_average(simplicity);
	cout << "simplicity: average = " << average << " variance = " << calculate_variance(simplicity, average) << endl;
	average = calculate_average(composition);
	cout << "composition: average = " << average << " variance = " << calculate_variance(composition, average) << endl;
	average = calculate_average(harmony);
	cout << "harmony: average = " << average << " variance = " << calculate_variance(harmony, average) << endl;
	
}
void color_harmony_training_run(char *directory){
	vector<unsigned int> hue(50, 0);
	vector<unsigned int> sat(50, 0);
	vector<unsigned int> val(50, 0);
	DIR *pDIR;
	struct dirent *entry;
	int total = 0;
	if(pDIR = opendir(directory)){
		while(entry = readdir(pDIR)){
			if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".DS_Store") != 0){
				char name[200];
				strcpy(name, directory);
				strcat(name, "/");
				strcat(name, entry->d_name);
				char* imageN = name;
				cout << name << endl;
				Mat3b image = imread(imageN, CV_LOAD_IMAGE_COLOR);
				if(!image.data) cout<<"no image file detected"<<endl;
				color_harmony_training(image, hue, sat, val);
				total++;
			}
		}
	}
	cout <<"HUE"<<endl;
	for(int i = 0; i < 50; i++){
		hue[i] /= total;
		cout << hue[i] << endl;
	}
	
	cout <<"SAT"<<endl;
	
	for(int i = 0; i < 50; i++){
		sat[i] /= total;
		cout << sat[i] << endl;
	}
	
	cout <<"VAL"<<endl;
	
	for(int i = 0; i < 50; i++){
		val[i] /= total;
		cout << val[i] << endl;
	}
}
int determine_image_quality(Mat3b image, Mat image2){
	float bri, hue, clar, light, simp, comp, harm;
	int con;
	compute_contrast_brightness(image, con, bri);
	hue = hue_count_quality(image);
	computeAestheticPhotoQuality(image2, image, clar, light, simp, comp, harm);
	
	//given all these values classify picture using precomputed mean and variances for each metric
	
	float uconG = 235.801, uconB = 222.672, ubriG = 0.3882, ubriB = 0.404799, uhueG = 9.01013,
	 uhueB = 8.97046, uclarG = 3.52285, uclarB = 1.73277, ulightG = 0.676726, ulightB = 0.680048,
	 usimpG = 1.38886, usimpB = 1.86324,ucompG = 0.209378, ucompB = 0.216073,
	 uharmG = 1.01968, uharmB = 0.692022;
	float vconG = 1200.85336, vconB = 1271.11, vbriG = 0.0312439, vbriB = 0.0262843, vhueG = 29.4919,
	 vhueB = 28.0725, vclarG = 6.35728, vclarB = 9.409, vlightG = 0.0593259, vlightB = 0.0458658,
	 vsimpG = 1.93987, vsimpB = 4.31618, vcompG = 0.0006308, vcompB = 0.00058922,
	 vharmG = 1.73589, vharmB = 1.77352;
	float pconG, pconB, pbriG, pbriB, phueG, phueB, pclarG, pclarB, plightG, plightB, psimpG,
	 psimpB, pcompG, pcompB, pharmG, pharmB;
	float pGood = .5, pBad = .5;
	
	pconG = pow(sqrt(2.0 * PI * vconG), -1.0) * exp(-1.0 * ((con - uconG) * (con - uconG)) / (2.0 * vconG));
	pconB = pow(sqrt(2.0 * PI * vconB), -1.0) * exp(-1.0 * ((con - uconB) * (con - uconB)) / (2.0 * vconB));
	pbriG = pow(sqrt(2.0 * PI * vbriG), -1.0) * exp(-1.0 * ((bri - ubriG) * (bri - ubriG)) / (2.0 * vbriG));
	pbriB = pow(sqrt(2.0 * PI * vbriB), -1.0) * exp(-1.0 * ((bri - ubriB) * (bri - ubriB)) / (2.0 * vbriB));
	phueG = pow(sqrt(2.0 * PI * vhueG), -1.0) * exp(-1.0 * ((hue - uhueG) * (hue - uhueG)) / (2.0 * vhueG));
	phueB = pow(sqrt(2.0 * PI * vhueB), -1.0) * exp(-1.0 * ((hue - uhueB) * (hue - uhueB)) / (2.0 * vhueB));
	pclarG = pow(sqrt(2.0 * PI * vclarG), -1.0) * exp(-1.0 * ((clar - uclarG) * (clar - uclarG)) / (2.0 * vclarG));
	pclarB = pow(sqrt(2.0 * PI * vclarB), -1.0) * exp(-1.0 * ((clar - uclarB) * (clar - uclarB)) / (2.0 * vclarB));
	plightG = pow(sqrt(2.0 * PI * vlightG), -1.0) * exp(-1.0 * ((light - ulightG) * (light - ulightG)) / (2.0 * vlightG));
	plightB = pow(sqrt(2.0 * PI * vlightB), -1.0) * exp(-1.0 * ((light - ulightB) * (light - ulightB)) / (2.0 * vlightB));
	psimpG = pow(sqrt(2.0 * PI * vsimpG), -1.0) * exp(-1.0 * ((simp - usimpG) * (simp - usimpG)) / (2.0 * vsimpG));
	psimpB = pow(sqrt(2.0 * PI * vsimpB), -1.0) * exp(-1.0 * ((simp - usimpB) * (simp - usimpB)) / (2.0 * vsimpB));
	pcompG = pow(sqrt(2.0 * PI * vcompG), -1.0) * exp(-1.0 * ((comp - ucompG) * (comp - ucompG)) / (2.0 * vcompG));
	pcompB = pow(sqrt(2.0 * PI * vcompB), -1.0) * exp(-1.0 * ((comp - ucompB) * (comp - ucompB)) / (2.0 * vcompB));
	pharmG = pow(sqrt(2.0 * PI * vharmG), -1.0) * exp(-1.0 * ((harm - uharmG) * (harm - uharmG)) / (2.0 * vharmG));
	pharmB = pow(sqrt(2.0 * PI * vharmB), -1.0) * exp(-1.0 * ((harm - uharmB) * (harm - uharmB)) / (2.0 * vharmB));
	
	//calculate posterior number for Good and Bad
	float posteriorG, posteriorB;
	posteriorG = pGood * pconG * pbriG * phueG * pclarG * plightG * psimpG * pcompG * pharmG;
	posteriorB = pBad * pconB * pbriB * phueB * pclarB * plightB * psimpB * pcompB * pharmB;
	
	//print out 1 if number is good and zero otherwise
	if(posteriorB > posteriorG) return 0;
	else return 1;
	//return 0;
}
	
void image_quality_run(char *directory){
	int good = 0, bad = 0;
	DIR *pDIR;
	struct dirent *entry;
	int zeros = 0, ones = 0, quality;
	if(pDIR = opendir(directory)){
		while(entry = readdir(pDIR)){
			if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".DS_Store") != 0){
				char name[200];
				strcpy(name, directory);
				strcat(name, "/");
				strcat(name, entry->d_name);
				char* imageN = name;
				//cout << name << endl;
				Mat3b image = imread(imageN, CV_LOAD_IMAGE_COLOR);
				Mat image2 = imread(imageN, CV_LOAD_IMAGE_COLOR);
				if(!image.data) cout<<"no image file detected"<<endl;
				quality = determine_image_quality(image, image2);
				cout << name << " quality: " << quality << endl;
				if(quality == 0) zeros++;
				else ones++;
			}
		}
	}
	cout << "Zeros: " << zeros << "\nOnes: " << ones << endl;
	
	
}