#include <iostream>

#include <opencv2/opencv.hpp>
#include "Low_Level_Features.h"
#include "High_Level_Features.h"
//#include "AestheticPhotoQuality.h"
#include "NaiveBayesianClassifier.h"

//SUPPOSED TP BE IN ROWS<COLS FORM FOR MAT

int main (int argc, const char * argv[]){
	//To run it once
	Mat image = imread("/Users/NOkwara1/Pictures/photo-3.jpeg", CV_LOAD_IMAGE_COLOR);
	Mat3b image2 = imread("/Users/NOkwara1/Pictures/photo-3.jpeg", CV_LOAD_IMAGE_COLOR);

	if(!image.data) return -1;
	
	cout << determine_image_quality(image, image2) << endl;
	/*//low level features
	float brightness;
	int contrast = 0;
	
	compute_contrast_brightness(image2, contrast, brightness);
	
	cout << "contrast: " << contrast << "\nbrightness: " << brightness << endl;
	
	//high level features
	float hueCount, noise, blur;
	
	hueCount = hue_count_quality(image2);
	cout << "hueCount: " << hueCount << "\nnoise: " << noise << "\nblur: " << blur << endl;

	//aesthetic photo quality;
	float clarity, lighting, simplicity = 0, composition, harmony;
	
	computeAestheticPhotoQuality(image2, image, clarity, lighting, simplicity, composition, harmony);
	cout << "clarity: " << clarity << "\nlighting: " << lighting << endl;
	cout << "simplicity: " << simplicity << "\ncomposition: " << composition << "\nharmony: " << harmony << endl;*/
	//color_harmony_training_run("/Users/NOkwara/Downloads/CUHKPhotoQualityDatabase/TRAIN/Bad");
	
	//image_quality_run("/Users/NOkwara/Downloads/CUHKPhotoQualityDatabase/TEST/Good");
	
	 return 0;
	
}