#include<opencv2/opencv.hpp>
#include<opencv2/stitching/stitcher.hpp>
#include<opencv2/stitching/warpers.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/calib3d/calib3d.hpp>
#include<opencv2/flann/miniflann.hpp>

#define DATA_FOLDER_3 "data"
#define TRAIN_FOLDER "data/train_images"
#define TEMPLATE_FOLDER "data/tempates"

class perspective_transformer {
private:
	cv::Mat im, im_transformed, im_perspective_transformed;
	std::vector<cv::Point2f> points, points_transformed;
	cv::Mat M;
public:
	perspective_transformer();
	void estimate_perspective();
	void show_diff();
};

perspective_transformer::perspective_transformer(){
	im = cv::imread(DATA_FOLDER_3 + std::string("/image.jpg"));
	im_transformed = cv::imread(DATA_FOLDER_3 + std::string("/transformed.jpg"));
}

void perspective_transformer::estimate_perspective(){
	// Match ORB features to point correspondences between the images
	std::vector<cv::KeyPoint> kp, t_kp;
	cv::Mat desc, t_desc, im_g, t_im_g;
	
	cv::cvtColor(im, im_g, CV_BGR2GRAY);
	cv::cvtColor(im_transformed, t_im_g, CV_BGR2GRAY);
	
	cv::OrbFeatureDetector featureDetector;
	cv::OrbDescriptorExtractor featureExtractor;
	
	featureDetector.detect(im_g, kp);
	featureDetector.detect(t_im_g, t_kp);
	
	featureExtractor.compute(im_g, kp, desc);
	featureExtractor.compute(t_im_g, t_kp, t_desc);
	
	cv::flann::Index flannIndex(desc, cv::flann::LshIndexParams(12,20,1), cvflann::FLANN_DIST_HAMMING);
	cv::Mat match_idx(t_desc.rows, 2, CV_32SC1), match_dist(t_desc.rows, 2, CV_32FC1);
	flannIndex.knnSearch(t_desc, match_idx, match_dist, 2, cv::flann::SearchParams());
	
	std::vector<cv::DMatch> good_matches;
	for(int i = 0; i < match_dist.rows; i++){
		if(match_dist.at<float>(i,0) < 0.6 * match_dist.at<float>(i, 1)){
			cv::DMatch dm(i, match_idx.at<int>(i,0), match_dist.at<float>(i,0));
			good_matches.push_back(dm);
			points.push_back((kp[dm.trainIdx]).pt);
			points_transformed.push_back((t_kp[dm.queryIdx]).pt);
		}
	}
	
	cv::Mat im_show;
	drawMatches(im_transformed, t_kp, im, kp, good_matches, im_show);
	imshow("ORB matches", im_show);
	
	M = cv::findHomography(points, points_transformed, CV_RANSAC, 2);
	std::cout << "Estimated Perspective transform = " << M << std::endl;
	
	cv::warpPerspective(im, im_perspective_transformed, M, im_transformed.size());
	cv::imshow("Estimated Perspective transform", im_perspective_transformed);
}

void perspective_transformer::show_diff(){
	cv::imshow("Difference", im_transformed - im_perspective_transformed);
}

int main(){
	perspective_transformer a;
	a.estimate_perspective();
	std::cout << "Press 'd' to show difference, 'q' to end" << std::endl;
	if(char(cv::waitKey(0)) =='d' ){
		a.show_diff();
		std::cout << "Press 'q' to end" << std::endl;
		if(char(cv::waitKey(0))=='q') return 0;
	}else
		return 0;
}
