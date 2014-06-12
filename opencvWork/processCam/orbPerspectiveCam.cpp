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


using namespace std;

const double PI = std::atan(1.0)*4;

class perspective_transformer {
private:
	unsigned clearImageAfterNFrames,lastFramePersistance;
	unsigned frameNumber;
	cv::Mat im, im_transformed, im_perspective_transformed, im_augmented, im_perspective_augmented;
	std::vector<cv::Point2f> points, points_transformed;
	cv::Mat M;
	cv::Mat persistentM;
	std::vector<cv::DMatch> good_matches;
	std::vector<cv::KeyPoint> kp, t_kp;
public:
	perspective_transformer(cv::Mat, cv::Mat);
	void estimate_perspective();
	void show_diff();
	void showMatches();
	void showEstimatedPerspective();
	void showAugmented();
	bool letAugmentedBeShown();
	bool newPersistence();
};

perspective_transformer::perspective_transformer(cv::Mat markerImg, cv::Mat frameImg){
	im = markerImg;
	im_transformed = frameImg;
	im_augmented = cv::imread("images/dragon.jpg");
	clearImageAfterNFrames = 90;
	lastFramePersistance = clearImageAfterNFrames;
	frameNumber = 1;
}

void perspective_transformer::estimate_perspective(){
	// Match ORB features to point correspondences between the images
	// clearing kp and t_kp keypoints
	if(frameNumber >= clearImageAfterNFrames){
		kp.clear();
		t_kp.clear();
		frameNumber = 1;
		good_matches.clear();
	}else{
		++frameNumber;
	}
	
	cv::Mat desc, t_desc, im_g, t_im_g;
	
	cv::cvtColor(im, im_g, CV_BGR2GRAY);
	cv::cvtColor(im_transformed, t_im_g, CV_BGR2GRAY);
	
	cv::OrbFeatureDetector featureDetector;
	cv::OrbDescriptorExtractor featureExtractor;
	
	featureDetector.detect(im_g, kp);
	featureDetector.detect(t_im_g, t_kp);
	
	featureExtractor.compute(im_g, kp, desc);
	featureExtractor.compute(t_im_g, t_kp, t_desc);
	
	cv::flann::Index flannIndex(desc, cv::flann::LshIndexParams(12,20,1), cvflann::FLANN_DIST_EUCLIDEAN);
	//~ cv::flann::Index flannIndex(desc, cv::flann::LshIndexParams(12,20,1), cvflann::FLANN_DIST_HAMMING);
	cv::Mat match_idx(t_desc.rows, 2, CV_32SC1), match_dist(t_desc.rows, 2, CV_32FC1);
	flannIndex.knnSearch(t_desc, match_idx, match_dist, 2, cv::flann::SearchParams());
	
	
	//~ std::vector<cv::DMatch> good_matches;
	points.clear();
	points_transformed.clear();
	//~ good_matches.clear();
	for(int i = 0; i < match_dist.rows; i++){
		if(match_dist.at<float>(i,0) < 2.0 * match_dist.at<float>(i, 1)){
			cv::DMatch dm(i, match_idx.at<int>(i,0), match_dist.at<float>(i,0));
			good_matches.push_back(dm);
			points.push_back((kp[dm.trainIdx]).pt);
			points_transformed.push_back((t_kp[dm.queryIdx]).pt);
		}
	}
	// this line was added
	//~ if (good_matches.size() >= 20 ){
	std::cout << good_matches.size() << std::endl;
	if (good_matches.size() >= 10){
		M = cv::findHomography(points, points_transformed, CV_RANSAC, 2);
		if (newPersistence()){
			persistentM = M;
			cv::warpPerspective(im_augmented, im_perspective_augmented, persistentM, im_transformed.size());
		}
	}
	std::cout << "Estimated Perspective transform = " << M << std::endl;
	++lastFramePersistance;
	if(lastFramePersistance > 3*lastFramePersistance)
		lastFramePersistance = 2*lastFramePersistance;
}

void perspective_transformer::show_diff(){
	cv::imshow("Difference", im_transformed + im_perspective_transformed);
}

void perspective_transformer::showMatches(){
	cv::Mat im_show;
	drawMatches(im_transformed, t_kp, im, kp, good_matches, im_show);
	imshow("ORB matches", im_show);
}

void perspective_transformer::showEstimatedPerspective(){
	if(letAugmentedBeShown()) {
		cv::warpPerspective(im, im_perspective_transformed, persistentM, im_transformed.size());
		cv::imshow("Estimated Perspective transform", im_perspective_transformed);
	}
}

void perspective_transformer::showAugmented(){
	if(letAugmentedBeShown() && !newPersistence()){
		cv::warpPerspective(im, im_perspective_transformed, persistentM, im_transformed.size());
		cv::imshow("Augmented Perspective transform", im_transformed + im_perspective_augmented);
	}
	else
		cv::imshow("Augmented Perspective transform", im_transformed + im_perspective_augmented);
		//~ cv::imshow("Augmented Perspective transform", im_transformed);
}

bool perspective_transformer::letAugmentedBeShown(){
	return (M.cols > 0 && (M.at<double>(0,1) * M.at<double>(1,0) <= 0.0) 
						&& ((M.at<double>(0,0)*M.at<double>(0,0) + M.at<double>(1,0) * M.at<double>(1,0)) <= 1.0)
					   && ((M.at<double>(0,1)*M.at<double>(0,1) + M.at<double>(1,1) * M.at<double>(1,1)) <= 1.0)
					    );
};

bool perspective_transformer::newPersistence(){
	if (lastFramePersistance >= clearImageAfterNFrames){
		lastFramePersistance = 0;
		return true;
	}
	return false;
}

void matchTest(){
	// VideoCapture object
	cv::VideoCapture cap(0);
	cv::Mat sourceImg = cv::imread("images/template.jpg");
	//~ //unsigned int frame_count = 0;
	cv::Mat goodTest = sourceImg;
	while(char(cv::waitKey(-1)) != 'q' && cap.isOpened()){
		//~ //double t0 = cv::getTickCount();
		cv::Mat test;
		cap >> test;
		if(test.empty()){
			test = goodTest;
			continue;
		}else{
			perspective_transformer a(sourceImg,test);
			a.estimate_perspective();
			goodTest = test;
		}
		//~ //cout << "Frame rate = " << cv::getTickFrequency() / (cv::getTickCount() - t0) << endl;
	}	
}

void doomyTest(){
	// VideoCapture object
	cv::VideoCapture cap(0);
	//~ cv::Mat sourceImg = cv::imread("images/template.jpg");
	cv::Mat sourceImg = cv::imread("images/hiro.jpg");
	//~ cv::Mat sourceImg = cv::imread("images/abelHand.jpg");
	//~ cv::Mat sourceImg = cv::imread("images/whiteBox.jpg");
	//~ if (sourceImg.empty())
		//~ std::cout << "problem reading source image" << std::endl;
	//~ else
		//~ return;
	//~ //unsigned int frame_count = 0;
	cv::Mat goodTest = sourceImg;
	while(char(cv::waitKey(30)) != 'q' && cap.isOpened()){
		//~ //double t0 = cv::getTickCount();
		cv::Mat test;
		cap >> test;
		if(test.empty()){
			//~ test = goodTest;
			continue;
		}else{
			
			perspective_transformer a(sourceImg,test);
			
			// if maintaining homography matrix 
			a.estimate_perspective();
			// if new homography matrix change bestHomography and find new homography
			
			// else mantain the same homograpy but find a new one
			
			// show frame according to the frame rate and homography
			a.showMatches();
			a.showEstimatedPerspective();
			//~ a.show_diff();
			a.showAugmented();
			//~ goodTest = test;
		}
		//~ //cout << "Frame rate = " << cv::getTickFrequency() / (cv::getTickCount() - t0) << endl;
	}
}

int main(){
	//~ matchTest();
	doomyTest();
	return 0;
}
