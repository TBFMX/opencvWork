#include<opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/video/tracking.hpp>
#include<opencv2/features2d/features2d.hpp>
#include<opencv2/legacy/legacy.hpp>
#include<opencv2/flann/miniflann.hpp>
#include<opencv2/stitching/stitcher.hpp>
#include<opencv2/stitching/warpers.hpp>
#include<opencv2/calib3d/calib3d.hpp>
#include<iostream>

#include "cartoon.h"

int main(int argc, char *argv[]){
	int cameraNumber = 0;
	if(argc > 1)
		cameraNumber = atoi(argv[1]);
	
	// Get access to the camera.
	cv::VideoCapture camera;
	camera.open(cameraNumber);
	if(!camera.isOpened()){
		std::cerr <<"ERROR: Could not access the camera or video!" << std::endl;
		exit(1);
	}
	
	// Try to set the camera resolution.
	//~ camera.set(cv::CV_CAP_PROPe_FRAME_WIDTH, 640);
	//~ camera.set(cv::CV_CAP_PROP_FRAME_HEIGHT, 480);
	
	while(true){
		// Grab the next camera frame.
		cv::Mat cameraFrame;
		camera >> cameraFrame;
		if(cameraFrame.empty()){
			std::cerr  << "ERROR: Couldn't grab a camera frame." << std::endl;
			exit(1);
		}
		// Create a blank output image, that we will draw onto.
		cv::Mat displayedFrame(cameraFrame.size(), CV_8UC3);
		
		// Run the cartoonifier filter on the camera frame.
		cartoonifyImage(cameraFrame, displayedFrame);
		
		// Display the processed image onto the screen.
		cv::imshow("Cartoonifier", displayedFrame);
		// IMPORTANT: Wait for at least 20 milliseconds,
		// so that the image can be displayed on the screen!
		// Also checks if a key was pressed in the GUI window.
		// Note that it should be a "char" to support Linux.
		char keypress = cv::waitKey(20); // Need this to see anything!
		if(keypress == 'q') { // Escape key
			//Quit the program!
			break;
		}
	}

	return 0;
}
