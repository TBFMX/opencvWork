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

#include<iomanip>
#include<vector>

#define DATA_FOLDER_3 "data"
#define TRAIN_FOLDER "data/train_images"
#define TEMPLATE_FOLDER "data/tempates"

// Listing 9-3
void on_mouse(int event, int x, int y, int, void * _p){
	cv::Point2f* p = (cv::Point2f *)_p;
	if(event == CV_EVENT_LBUTTONUP){
		p->x = x;
		p->y = y;
	}
}

class perspective_transformer{
private:
	cv::Mat im, im_transformed, im_perspective_transformed, im_show, im_transformed_show;
	std::vector<cv::Point2f> points, points_transformed;
	cv::Mat M;
	cv::Point2f get_click(std::string, cv::Mat);
public:
	perspective_transformer();
	void estimate_perspective();
	void show_diff();
};

perspective_transformer::perspective_transformer(){
	im = cv::imread(DATA_FOLDER_3 + std::string("/image.jpg"));
	im_transformed = cv::imread(DATA_FOLDER_3 + std::string("/transformed.jpg"));
	std::cout << DATA_FOLDER_3 + std::string("/transformed.jpg") << std::endl;
}

cv::Point2f perspective_transformer::get_click(std::string window_name, cv::Mat im){
	cv::Point2f p(-1,-1);
	cv::setMouseCallback(window_name, on_mouse, (void *)&p);
	while(p.x == -1 && p.y == -1){
		cv::imshow(window_name, im);
		cv::waitKey(20);
	}
	return p;
}

void perspective_transformer::estimate_perspective(){
	imshow("Original",im);
	imshow("Transformed",im_transformed);
	std::cout << "To estimate the Perspective transform between the original and transformed images you\
	will have to click on 8 matching pairs of points" <<std::endl;
	im_show = im.clone();
	im_transformed_show = im_transformed.clone();
	cv::Point2f p;
	for(int i = 0; i < 8; i++){
		std::cout << "POINT " << i << std::endl;
		std::cout << "Click on a distinguished point in the ORIGINAL image"  << std::endl;
		p = get_click("Original",im_show);
		std::cout << p << std::endl;
		points.push_back(p);
		cv::circle(im_show, p, 2, cv::Scalar(0,255,0), -1);
		cv::imshow("Transformed", im_transformed_show);
		
		std::cout << "Click on a distinguished point in the TRANSFORMED image" << std::endl;
		p = get_click("Transformed", im_transformed_show);
		std::cout<< p << std::endl;
		points_transformed.push_back(p);
		cv::circle(im_transformed_show, p, 2, cv::Scalar(0,255,0), -1);
		cv::imshow("Transformed", im_transformed_show);
	}
	
	// Estimate perspective transform
	M = findHomography(points, points_transformed, CV_RANSAC, 2);
	std::cout << "Estimated Perspective transform = " << M << std::endl;
	
	// Apply estimated perspective transform
	cv::warpPerspective(im, im_perspective_transformed, M, im_transformed.size());
	cv::imshow("Estimated Perspective transform", im_perspective_transformed);
}

void perspective_transformer::show_diff() {
	cv::imshow("Difference", im_transformed - im_perspective_transformed);
}

int main(){
	perspective_transformer a;
	a.estimate_perspective();
	std::cout << "Press 'd' to show difference, 'q' to end" << std::endl;
	if(char(cv::waitKey(0)) == 'd'){
		a.show_diff();
		std::cout << "Press 'q' to end" << std::endl;
		if(char(cv::waitKey(0)) == 'q') return 0;
	}else
		return 0;
}
