#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/video/tracking.hpp>
#include<opencv2/features2d/features2d.hpp>
#include<opencv2/legacy/legacy.hpp>
#include<iostream>
#include<iomanip>
#include<vector>

using namespace std;

const double PI = std::atan(1.0)*4;

void camTest(){
	// 0 id the ID of the built-in laptop camera, change if you want to use other camera
	cv::VideoCapture cap(0);
	// check if the file was opened properly
	if(!cap.isOpened()){
		cout << "Capture could not be opened successfully" << endl;
		return;
	}
	
	cv::namedWindow("Video");
	
	// Play the video in a loop till it ends
	while(char(cv::waitKey(1)) != 'q' && cap.isOpened()){
		cv::Mat frame;
		cap >> frame;
		// Check if the video is over
		if(frame.empty()){
			cout << "Video over" << endl;
			break;
		}
		cv::imshow("Video",frame);
	}
}

void otherTest(){
	cv::VideoCapture cap(0);
	if(!cap.isOpened())
		return;
	cv::namedWindow("webcam");
	for(;;){
		cv::Mat frame;
		cap >> frame;
		cv::imshow("webcam",frame);
		if(cv::waitKey(50) >= 0)
			break;
	}
}

void videoTest(){
	// Create a VideoCapture object to read from video file
	cv::VideoCapture cap("../videos/bike.mp4");
	// check if the file was opened properly
	if(!cap.isOpened()){
		cout << "Capture could not be opened successfully" << endl;
		return;
	}
	cv::namedWindow("Video");
	// Play the video in a loop till it ends
	while(char(cv::waitKey(1)) != 'q' && cap.isOpened()){
		cv::Mat frame;
		cap >> frame;
		// Check if the video is over
		if (frame.empty()){
			cout << "Video over" << endl;
			break;
		}
		cv::imshow("Video",frame);
	}
	return;
}

void writingVideoTest(){
	// 0 is the ID of the built-in laptop camera, change if you wat to use other camera
	cv::VideoCapture cap(0);
	// check if the file was opened properly
	if(!cap.isOpened()){
		cout << "Capture could not be opened successfully" << endl;
		return;
	}
	// Get size of frames
	cv::Size S = cv::Size((int) cap.get(CV_CAP_PROP_FRAME_WIDTH), (int)
				cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	// Make a video writer object and initialize it at 30 FPS
	cv::VideoWriter put("../videos/output.mpg", CV_FOURCC('M','P','E','G'),30,S);
	if(!put.isOpened()){
		cout << "File could not be created for writing. Check permissions" << endl;
		return;
	}
	cv::namedWindow("Video");
	// Play the video in a loop till it ends
	while(char(cv::waitKey(1)) != 'q' && cap.isOpened()){
		cv::Mat frame;
		cap >> frame;
		// Check if the video is over
		if(frame.empty()){
			cout << "Video over" << endl;
			break;
		}
		cv::imshow("Video", frame);
		put << frame;
	}
	return;
}

int main(){
	//~ camTest();
	//~ otherTest();
	videoTest();
	//~ writingVideoTest();
	return 0;
}
