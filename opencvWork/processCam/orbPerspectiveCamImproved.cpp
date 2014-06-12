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

class homographyFinder{
private:
	cv::Mat im, gb_im_transformed, gb_im_augmented, gb_im_perspective_augmented;
	std::vector<cv::Point2f> points, points_transformed;
	cv::Mat M;
	unsigned gbFrameImgRows, gbFrameImgCols;

	unsigned frameNumber;
	std::vector<cv::DMatch> good_matches;
	std::vector<cv::KeyPoint> kp, t_kp;
	void showMatchesController(cv::Mat *im_show);
	cv::Mat unityM;
	void showEstimatedPerspectiveController(cv::Mat *im_perspective_transformed);
	void showAugmentedController(cv::Mat *im_augmented);
public:
	homographyFinder(cv::Mat, cv::Mat);
	void showMatches();
	void showEstimatedPerspective();
	void estimatePerspective();
	bool isGoodHomographyMatrix();
	cv::Mat getHomographyMatrix();
	void setHomographyMatrix(cv::Mat newM);
	void showAugmented();
};


homographyFinder::homographyFinder(cv::Mat markerImg, cv::Mat frameImg){
	im = markerImg;
	gb_im_transformed = frameImg;
	gb_im_augmented = cv::imread("images/dragon.jpg");
	unityM =  cv::Mat::zeros(3,3,CV_64F);
	unityM.at<double>(0,0) = 1.0;
	unityM.at<double>(1,1) = 1.0;
	unityM.at<double>(2,2) = 1.0;
	unityM.at<double>(0,2)= frameImg.rows;
	unityM.at<double>(1,2)= frameImg.cols;
	
	gbFrameImgRows = frameImg.rows;
	gbFrameImgCols = frameImg.cols;
	//~ std::cout << unityM << std::endl;
}


void homographyFinder::estimatePerspective(){
	// Match ORB features to point correspondences between the images
	// clearing kp and t_kp keypoints
	//~ kp.clear();
	//~ t_kp.clear();
	//~ frameNumber = 1;
	//~ good_matches.clear();
	
	cv::Mat desc, t_desc, im_g, t_im_g;
	
	cv::cvtColor(im, im_g, CV_BGR2GRAY);
	cv::cvtColor(gb_im_transformed, t_im_g, CV_BGR2GRAY);
	
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
	//~ points.clear();
	//~ points_transformed.clear();
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
	}
	std::cout << "Estimated Perspective transform = " << M << std::endl;
}

bool homographyFinder::isGoodHomographyMatrix(){
	return (M.cols > 0 && (abs(M.at<double>(0,2)) <= gbFrameImgRows) && (abs(M.at<double>(1,2)) <= gbFrameImgCols)
						 &&	(M.at<double>(0,1) * M.at<double>(1,0) <= 0.0) 
						&& ((M.at<double>(0,0)*M.at<double>(0,0) + M.at<double>(1,0) * M.at<double>(1,0)) <= 1.0)
					   && ((M.at<double>(0,1)*M.at<double>(0,1) + M.at<double>(1,1) * M.at<double>(1,1)) <= 1.0)
					    );
};

cv::Mat homographyFinder::getHomographyMatrix(){
	return M;
}

void homographyFinder::setHomographyMatrix(cv::Mat newM){
	M = newM;
}

void homographyFinder::showMatchesController(cv::Mat *im_show){
	cv::drawMatches(gb_im_transformed, t_kp, im, kp, good_matches, *im_show);
	std::cout << "controller im_show "<< (*im_show).rows << "," << (*im_show).cols << std::endl;
}

void homographyFinder::showEstimatedPerspectiveController(cv::Mat *im_perspective_transformed){
	
	if(isGoodHomographyMatrix())
		cv::warpPerspective(im, *im_perspective_transformed, M, gb_im_transformed.size());
	else{
		cv::warpPerspective(im, *im_perspective_transformed, unityM, gb_im_transformed.size());		
	}
}

void homographyFinder::showAugmentedController(cv::Mat *im_perspective_transformed){
	//~ cv::Mat im_perspective_transformed;
	showEstimatedPerspectiveController(im_perspective_transformed);
	if(isGoodHomographyMatrix()){
		// this image is shown in perspective
		cv::warpPerspective(gb_im_augmented, gb_im_perspective_augmented, M, gb_im_transformed.size());
		
		cv::warpPerspective(im, *im_perspective_transformed, M, gb_im_transformed.size());
	}else{
		cv::warpPerspective(im, *im_perspective_transformed, unityM, gb_im_transformed.size());
	}
}

void homographyFinder::showMatches(){
	cv::Mat im_show;
	showMatchesController(&im_show);
	std::cout << "im_show "<< im_show.rows << "," << im_show.cols << std::endl;
	imshow("ORB matches", im_show);
}

void homographyFinder::showEstimatedPerspective(){
	cv::Mat im_perspective_transformed;
	showEstimatedPerspectiveController(&im_perspective_transformed);
	cv::imshow("Estimated Perspective transform", im_perspective_transformed);
}

void homographyFinder::showAugmented(){
	cv::Mat im_perspective_augmented;
	showAugmentedController(&im_perspective_augmented);
	//~ cv::imshow("Augmented Perspective transform", gb_im_transformed + im_perspective_augmented);
	cv::imshow("Augmented Perspective transform", gb_im_transformed + gb_im_perspective_augmented);
}



//~ bool homographyFinder::newPersistence(){
	//~ if (lastFramePersistance >= clearImageAfterNFrames){
		//~ lastFramePersistance = 0;
		//~ return true;
	//~ }
	//~ return false;
//~ }


void doomyTest(){
	// VideoCapture object
	cv::VideoCapture cap(0);
	cv::Mat sourceImg = cv::imread("images/template.jpg");
	//~ cv::Mat sourceImg = cv::imread("images/hiro.jpg");
	//~ cv::Mat sourceImg = cv::imread("images/abelHand.jpg");
	//~ cv::Mat sourceImg = cv::imread("images/whiteBox.jpg");
	//~ if (sourceImg.empty())
		//~ std::cout << "problem reading source image" << std::endl;
	//~ else
		//~ return;
	//~ //unsigned int frame_count = 0;
	cv::Mat goodTest = sourceImg;
	cv::Mat unityM;
	unityM =  cv::Mat::zeros(3,3,CV_64F);
	unityM.at<double>(0,0) = 1.0;
	unityM.at<double>(1,1) = 1.0;
	unityM.at<double>(2,2) = 1.0;
	unsigned clearImageAfterNFrames = 90;
	unsigned lastFramePersistance;
	unsigned frameNumber = 30;
	int WAITINGTIME = 30;
	double timeForFrame = 1.0/frameNumber;
	cv::Mat remainingMatrix = unityM;
	double cumulativeTime = 0.0;
	while(char(cv::waitKey(WAITINGTIME)) != 'q' && cap.isOpened()){
		double t0 = (double)cv::getTickCount();
		cv::Mat test;
		cap >> test;
		if(test.empty()){
			continue;
		}else{
			unityM.at<double>(0,2)= test.rows;
			unityM.at<double>(1,2)= test.cols;			
			homographyFinder a(sourceImg,test);
			// if maintaining homography matrix 
			a.estimatePerspective();
			cv::Mat homographyM = a.getHomographyMatrix();
			if(a.isGoodHomographyMatrix()){
				remainingMatrix = homographyM;
			}
			if (cumulativeTime >= timeForFrame*clearImageAfterNFrames){
				cumulativeTime = 0.0;
				std::cout << timeForFrame*clearImageAfterNFrames << std::endl;
				remainingMatrix = unityM;
			}else{
				a.setHomographyMatrix(remainingMatrix);
			}
			// if new homography matrix change bestHomography and find new homography
			
			// else mantain the same homograpy but find a new one
			
			// show frame according to the frame rate and homography
			a.showMatches();
			a.showEstimatedPerspective();
			a.showAugmented();
			//~ goodTest = test;
		}
		t0 = ((double)cv::getTickCount() - t0) / cv::getTickFrequency();
		//~ t0 = (double)cv::getTickCount() - t0;
		cumulativeTime += t0;
		std::cout << "Frame rate = " << t0 << std::endl;
		std::cout << "time = " << t0 << std::endl;
	}
}

int main(){
	//~ matchTest();
	doomyTest();
	return 0;
}
