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
	cv::Mat im, im_transformed, im_perspective_transformed, im_augmented, im_perspective_augmented;
	std::vector<cv::Point2f> points, points_transformed;
	cv::Mat M;
	unsigned gbFrameImgRows, gbFrameImgCols;
	std::vector<cv::DMatch> good_matches;
	std::vector<cv::KeyPoint> kp, t_kp;
public:
	perspective_transformer(cv::Mat, cv::Mat);
	void estimate_perspective();
	void getAugmented(cv::Mat *);
	bool letAugmentedBeShown();
};

perspective_transformer::perspective_transformer(cv::Mat markerImg, cv::Mat frameImg){
	im = markerImg;
	im_transformed = frameImg;
	im_augmented = cv::imread("images/dragon.jpg");

	gbFrameImgRows = frameImg.rows;
	gbFrameImgCols = frameImg.cols;
}

void perspective_transformer::estimate_perspective(){
	// Match ORB features to point correspondences between the images
	
	cv::Mat desc, t_desc, im_g, t_im_g;
	
	cv::cvtColor(im, im_g, CV_BGR2GRAY);
	cv::cvtColor(im_transformed, t_im_g, CV_BGR2GRAY);
	
	cv::OrbFeatureDetector featureDetector;
	cv::OrbDescriptorExtractor featureExtractor;
	
	featureDetector.detect(im_g, kp);
	featureDetector.detect(t_im_g, t_kp);
	
	featureExtractor.compute(im_g, kp, desc);
	featureExtractor.compute(t_im_g, t_kp, t_desc);
	
	//~ cv::flann::Index flannIndex(desc, cv::flann::LshIndexParams(12,20,1), cvflann::FLANN_DIST_EUCLIDEAN);
	cv::flann::Index flannIndex(desc, cv::flann::LshIndexParams(12,20,1), cvflann::FLANN_DIST_HAMMING);
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
	if (good_matches.size() >= 10){
		M = cv::findHomography(points, points_transformed, CV_RANSAC, 2);
		cv::warpPerspective(im_augmented, im_perspective_augmented, M, im_transformed.size());
	}
}

void perspective_transformer::getAugmented(cv::Mat *image){
	(*image)= im_transformed;
	if(letAugmentedBeShown()){
		cv::warpPerspective(im, im_perspective_transformed, M, im_transformed.size());
		
		for(int y=0;y<(*image).rows;y++)
			for(int x=0;x<(*image).cols;x++)
			{
				if( im_perspective_augmented.at<uchar[3]>(y,x)[0] > 0 ||
					im_perspective_augmented.at<uchar[3]>(y,x)[1] > 0 ||
					 im_perspective_augmented.at<uchar[3]>(y,x)[2] > 0 ){
					(*image).at<uchar[3]>(y,x)[0] = im_perspective_augmented.at<uchar[3]>(y,x)[0];
					(*image).at<uchar[3]>(y,x)[1] = im_perspective_augmented.at<uchar[3]>(y,x)[1];
					(*image).at<uchar[3]>(y,x)[2] = im_perspective_augmented.at<uchar[3]>(y,x)[2];
				}
			}
		}
}

bool perspective_transformer::letAugmentedBeShown(){
		return (M.cols > 0 && (abs(M.at<double>(0,2)) <= gbFrameImgRows) && (abs(M.at<double>(1,2)) <= gbFrameImgCols)
						 &&	(M.at<double>(0,1) * M.at<double>(1,0) <= 0.0) 
						&& ((M.at<double>(0,0)*M.at<double>(0,0) + M.at<double>(1,0) * M.at<double>(1,0)) <= 1.0)
					   && ((M.at<double>(0,1)*M.at<double>(0,1) + M.at<double>(1,1) * M.at<double>(1,1)) <= 1.0)
					    );
};

void doomyTest(){
	// VideoCapture object
	cv::VideoCapture cap(0);
	cv::Mat sourceImg = cv::imread("images/template.jpg");
	//~ cv::Mat sourceImg = cv::imread("images/hiro.jpg");
	//~ cv::Mat sourceImg = cv::imread("images/abelHand.jpg");
	//~ cv::Mat sourceImg = cv::imread("images/whiteBox.jpg");

	cv::Mat goodTest = sourceImg;
	while(char(cv::waitKey(30)) != 'q' && cap.isOpened()){
		cv::Mat test;
		cv::Mat image;
		cap >> test;
		if(test.empty()){
			continue;
		}else{
			perspective_transformer a(sourceImg,test);
			a.estimate_perspective();
			a.getAugmented(&image);
			cv::imshow("Augmented Perspective transform",image);
		}
	}
}

int main(){
	doomyTest();
	return 0;
}
