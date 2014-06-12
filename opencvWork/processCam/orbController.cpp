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
	double theta;
	unsigned gbFrameImgRows, gbFrameImgCols;
	std::vector<cv::DMatch> good_matches;
	std::vector<cv::KeyPoint> kp, t_kp;
public:
	perspective_transformer(cv::Mat*, cv::Mat*);
	void estimate_perspective(char thisChar);
	void getAugmented(cv::Mat *);
	void updateFrame(cv::Mat);
	bool letAugmentedBeShown();
};

perspective_transformer::perspective_transformer(cv::Mat *markerImg, cv::Mat *frameImg){
	im = *markerImg;
	im_transformed = *frameImg;
	im_augmented = cv::imread("images/template.jpg");
	//~ im_augmented = cv::imread("images/dragon.jpg");

	gbFrameImgRows = (*frameImg).rows;
	gbFrameImgCols = (*frameImg).cols;
	theta = 0.0;
	M = cv::Mat(3, 3, CV_64F);
	
	M.at<double>(0,0) = cos(theta);
	M.at<double>(0,1) = -sin(theta);
	M.at<double>(0,2) =0.0;//gbFrameImgCols;
		
	M.at<double>(1,0) = sin(theta);
	M.at<double>(1,1) = cos(theta);
	M.at<double>(1,2) =0.0;//gbFrameImgRows;
	
	M.at<double>(2,0)= 0.0;
	M.at<double>(2,1)= 0.0;
	M.at<double>(2,2)=1.0;

}

void perspective_transformer::estimate_perspective(char thisChar){
	// Match ORB features to point correspondences between the images
	
	if(thisChar == 'c')
		theta += 0.1;
	if(thisChar == 'r')
		theta -= 0.1;
	M.at<double>(0,0) = cos(theta);
	M.at<double>(0,1) = -sin(theta);
	if (thisChar == 'f')
		M.at<double>(0,2)+=10;//gbFrameImgCols;
	if(thisChar == 'b')
		M.at<double>(0,2)-=10;//gbFrameImgCols;
		
	M.at<double>(1,0) = sin(theta);
	M.at<double>(1,1) = cos(theta);
	if(thisChar == 'u')
		M.at<double>(1,2)-=10;//gbFrameImgRows;
	if(thisChar == 'd')
		M.at<double>(1,2)+=10;//gbFrameImgRows;

	if(thisChar == 's'){
		M.at<double>(2,0)+= 0.00001;//g
		M.at<double>(2,1)+= 0.00001;//i
	}
		
	if(thisChar =='t'){
		M.at<double>(2,0)-= 0.00001; //h
		M.at<double>(2,1)-= 0.00001;//j
	}
	
	if(thisChar =='v'){
		M.at<double>(2,0)+= 0.00001; //h
		M.at<double>(2,1)-= 0.00001;//j
	}
	
	if(thisChar =='w'){
		M.at<double>(2,0)-= 0.00001; //h
		M.at<double>(2,1)+= 0.00001;//j
	}
	M.at<double>(2,2)=1.0;
	
	cout << M<< endl;
	
	if(M.cols>0){
	cv::warpPerspective(im_augmented, im_perspective_augmented, M, im_transformed.size());
	//cout << M<< endl;
	cout << "rows " << gbFrameImgRows <<" cols " << gbFrameImgCols << endl;
	}
	return;
}

void perspective_transformer::getAugmented(cv::Mat *image){
	(*image)= im_transformed;
	if(letAugmentedBeShown()){
		//cv::warpPerspective(im, im_perspective_transformed, M, im_transformed.size());
		
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
		return (M.cols > 0 && ((abs(M.at<double>(0,2)) <= 2*gbFrameImgRows)||(abs(M.at<double>(0,2)) <= 2*gbFrameImgCols))
							&& ((abs(M.at<double>(1,2)) <= 2*gbFrameImgRows) ||(abs(M.at<double>(1,2)) <= 2*gbFrameImgCols))
						 &&	(M.at<double>(0,1) * M.at<double>(1,0) <= 0.0) 
						&& ((M.at<double>(0,0)*M.at<double>(0,0) + M.at<double>(1,0) * M.at<double>(1,0)) <= 1.0)
					   && ((M.at<double>(0,1)*M.at<double>(0,1) + M.at<double>(1,1) * M.at<double>(1,1)) <= 1.0)
					    );
};

void perspective_transformer::updateFrame(cv::Mat frameImg){
	im_transformed = frameImg;
	gbFrameImgRows = frameImg.rows;
	gbFrameImgCols = frameImg.cols;
}

void doomyTest(){
	// VideoCapture object
	cv::VideoCapture cap(0);
	cv::Mat sourceImg = cv::imread("images/template.jpg");
	//~ cv::Mat sourceImg = cv::imread("images/hiro.jpg");
	//~ cv::Mat sourceImg = cv::imread("images/abelHand.jpg");
	//~ cv::Mat sourceImg = cv::imread("images/whiteBox.jpg");

	cv::Mat goodTest = sourceImg;
	char thisChar;
	cv::Mat firstFrame;
	cap >> firstFrame;
	perspective_transformer a(&sourceImg,&firstFrame);
	while((thisChar=char(cv::waitKey(30))) != 'q' && cap.isOpened()){
		cv::Mat test;
		cv::Mat image;
		cap >> test;
		if(test.empty()){
			continue;
		}else{
			a.updateFrame(test);
			a.estimate_perspective(thisChar);
			a.getAugmented(&image);
			cv::imshow("Augmented Perspective transform",image);
		}
	}
}

int main(){
	doomyTest();
	return 0;
}
