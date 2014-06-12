#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/video/tracking.hpp>
#include<opencv2/features2d/features2d.hpp>
#include<opencv2/legacy/legacy.hpp>
#include<opencv2/flann/miniflann.hpp>
#include<iostream>
#include<iomanip>
#include<vector>

using namespace std;

const double PI = std::atan(1.0)*4;

int main(){
	cv::Mat train = cv::imread("../images/template.jpg"), train_g;
	//~ cv::Mat train = cv::imread("../images/template2.jpg"), train_g;
	cv::cvtColor(train,train_g, CV_BGR2GRAY);
	
	// detect SIFT keypoints and extract descriptors in the train image
	vector<cv::KeyPoint> train_kp;
	cv::Mat train_desc;
	
	cv::OrbFeatureDetector featureDetector;
	featureDetector.detect(train_g, train_kp);
	cv::OrbDescriptorExtractor featureExtractor;
	featureExtractor.compute(train_g, train_kp, train_desc);
	cout << "Descriptor depth" << train_desc.depth() << endl;
	// FLANN based descriptor matcher object
	cv::flann::Index flannIndex(train_desc, cv::flann::LshIndexParams(12, 20,2),
						cvflann::FLANN_DIST_HAMMING);
						//~ cvflann::FLANN_DIST_EUCLIDEAN);
						
	// VideoCapture object
	cv::VideoCapture cap(0);
	
	//~ unsigned int frame_count = 0;
	
	while(char(cv::waitKey(1000)) != 'q' && cap.isOpened()){
		//~ double t0 = cv::getTickCount();
		cv::Mat test, test_g;
		cap >> test;
		if(test.empty())
			continue;
		
		cv::cvtColor(test, test_g, CV_BGR2GRAY);
		
		// detect SIFT keypoints and extract descriptors in the test image
		vector<cv::KeyPoint> test_kp;
		cv::Mat test_desc;
		featureDetector.detect(test_g, test_kp);
		featureExtractor.compute(test_g, test_kp, test_desc);
		
		// match train and test descriptors, getting 2 nearest neighbors for all test descriptors
		cv::Mat match_idx(test_desc.rows, 2, CV_32SC1), match_dist(test_desc.rows, 2, CV_32FC1);
		flannIndex.knnSearch(test_desc, match_idx, match_dist, 2, cv::flann::SearchParams());
		
		// filter for good matches according to Lowe's algorithm
		vector<cv::DMatch> good_matches;
		for(int i = 0; i < match_dist.rows; i++){
			if(match_dist.at<float>(i,0) < 0.6 * match_dist.at<float>(i, 1)){
				cv::DMatch dm(i, match_idx.at<int>(i, 0), match_dist.at<float>(i,0));
				good_matches.push_back(dm);
			}
		}
		
		cv::Mat img_show;
		cv::drawMatches(test, test_kp, train, train_kp, good_matches, img_show);
		cv::imshow("Matches", img_show);
		
		//~ cout << "Frame rate = " << cv::getTickFrequency() / (cv::getTickCount() - t0) << endl;
	}
	return 0;
}
