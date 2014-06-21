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
#include<cstdlib>
#include<iostream>
#include<vector>
#include<string>

class CameraCalibrator {
	// input points:
	// the points in world coordinates
	std::vector<std::vector<cv::Point3f> > objectPoints;
	// the point positions in pixels
	std::vector<std::vector<cv::Point2f> > imagePoints;
	// output Matrices
	cv::Mat cameraMatrix;
	cv::Mat distCoeffs;
	// flag to specify how calibration is done
	int flag;
	// used in image undistortion
	cv::Mat map1, map2;
	bool mustInitUndistort;
	
public:
	CameraCalibrator(): flag(0), mustInitUndistort(true) {};
	int addChessboardPoints(const std::vector<std::string>& filelist, cv::Size & boardSize);
	void addPoints(const std::vector<cv::Point2f>&imageCorners, const std::vector<cv::Point3f>& objectCorners);	
	double calibrate(cv::Size &imageSize);
	cv::Mat remap(const cv::Mat &image);
	cv::Mat getCameraMatrix();
	cv::Mat getDistCoeffs();
};

cv::Mat CameraCalibrator::getCameraMatrix(){
	return cameraMatrix;
}

cv::Mat CameraCalibrator::getDistCoeffs(){
	return distCoeffs;
}

// Open chessboard images and extract corner points
int CameraCalibrator::addChessboardPoints(
		const std::vector<std::string>& filelist, cv::Size & boardSize){
	// the points on the chessboard
	std::vector<cv::Point2f> imageCorners;
	std::vector<cv::Point3f> objectCorners;
	// 3D Scene Points:
	// Initialize the chessboard corners
	// in the chessboard reference frame
	// The corners are at 3D location (X,Y,Z)= (i,j,0)
	for (int i = 0; i < boardSize.height; i++){
		for (int j= 0; j < boardSize.width; j++){
			objectCorners.push_back(cv::Point3f(i,j,0.0f));
		}
	}
	
	// 2D Image points:
	cv::Mat image; // to contain chessboard image
	int successes = 0;
	// for all viewpoints
	for(unsigned i = 0; i < filelist.size(); i++) {
		// Open the image
		image = cv::imread(filelist[i], 0);
		// Get the chessboard corners
		bool found = cv::findChessboardCorners(image, boardSize, imageCorners);
		
		// Get subpixel accuracy on the corners
		cv::cornerSubPix(image, imageCorners, cv::Size(5,5), 
			cv::Size(-1,-1),
			cv::TermCriteria(cv::TermCriteria::MAX_ITER +
			cv::TermCriteria::EPS, 
			30, // max number of iterations
			0.1)); // min accuracy
		// If we have a good board, add it to our data
		if(imageCorners.size() == boardSize.area()){
			// Add image and scene points from one view
			addPoints(imageCorners, objectCorners);
			successes++;
		}
	}
	
	return successes;
}

// Add scene points and corresponding image points
void CameraCalibrator::addPoints(const std::vector<cv::Point2f>&
	imageCorners, const std::vector<cv::Point3f>& objectCorners){
		// 2D image points from one view
		imagePoints.push_back(imageCorners);
		// corresponding 3D scene points
		objectPoints.push_back(objectCorners);
}
	
// Calibrate the camera
// returns the re-projection error
double CameraCalibrator::calibrate(cv::Size &imageSize){
	// udistorter must be reinitialized
	mustInitUndistort =  true;
	
	// Output rotations and translations
	std::vector<cv::Mat>rvecs, tvecs;
	//start calibration
	return
		calibrateCamera(objectPoints, // the 3D points
		imagePoints, // the image points
		imageSize, // image size
		cameraMatrix, // output camera matrix
		distCoeffs, // output distortion matrix
		rvecs, tvecs, // Rs, Ts
		flag); // set options
}

// remove distortion in an image (after calibration)
cv::Mat CameraCalibrator::remap(const cv::Mat &image){
	cv::Mat undistorted;
	if(mustInitUndistort){
		cv::initUndistortRectifyMap(
			cameraMatrix, // computed camera matrix
			distCoeffs, // computed distortion matrix
			cv::Mat(), // optimal rectification (none)
			cv::Mat(), // camera matrix to generate undistorted 
			image.size(), // size of undistorted
			CV_32FC1, // type of output map
			map1, map2); // the x and y mapping functions
		mustInitUndistort = false;
	}
	
	// Apply mapping functions
	cv::remap(image, undistorted, map1, map2,
		cv::INTER_LINEAR); // interpolation type
	return undistorted;
}

void testOnePicture(){
	//~ std::string pathToFile = "/home/abel/chess74.jpg";
	std::string pathToFile = "boards/chess15.jpg";
	cv::Mat image = cv::imread(pathToFile,0);

	// output vectors of image points
	std::vector<cv::Point2f> imageCorners;
	std::vector<cv::Point3f> objectCorners;
	// number of corners on the chessboard
	cv::Size boardSize(7,4);
	// Get the chessboard corners
	bool found = cv::findChessboardCorners(image, boardSize, imageCorners);
	 
	 if (found==true)
		std::cout << "founded" << std::endl;
	// Draw the corners
	cv::drawChessboardCorners(image,boardSize,imageCorners,found); // corners have been found
	cv::namedWindow("Calibration");
	cv::imshow("Calibration",image);
	cv::waitKey();	
}

void fillPictureList(std::vector<std::string> *picList){
	
	(*picList).push_back("boards/chess01.jpg");
	(*picList).push_back("boards/chess02.jpg");
	(*picList).push_back("boards/chess03.jpg");
	(*picList).push_back("boards/chess04.jpg");
	(*picList).push_back("boards/chess05.jpg");
	(*picList).push_back("boards/chess06.jpg");
	(*picList).push_back("boards/chess07.jpg");
	(*picList).push_back("boards/chess08.jpg");
	(*picList).push_back("boards/chess09.jpg");
	(*picList).push_back("boards/chess10.jpg");
	(*picList).push_back("boards/chess11.jpg");
	(*picList).push_back("boards/chess12.jpg");
	(*picList).push_back("boards/chess13.jpg");
	(*picList).push_back("boards/chess14.jpg");
	(*picList).push_back("boards/chess15.jpg");
}

void testSeveralPictures( ){
	cv::Size boardSize(7,4); // internal corners of the chessboard
	// names of the picture with a 9x6 internal corners chessboard
	std::vector<std::string>picList; 
	fillPictureList(&picList);
	
	CameraCalibrator myCalibrator;
	int successes = myCalibrator.addChessboardPoints(picList,boardSize);
	std::cout << "successes "<< successes << std::endl;
	
	cv::Mat original = cv::imread("calibratedChess.jpg");
	//~ cv::Mat original = cv::imread("chess74.jpg");
	cv::Size s = original.size();
	

		
	double errorCalibration = myCalibrator.calibrate(s);
	
	cv::Mat distCoeffs = myCalibrator.getDistCoeffs();
	cv::Mat cameraMatrix = myCalibrator.getCameraMatrix();
	
	std::cout << "distCoeffs " << distCoeffs << std::endl;
	std::cout << "cameraMatrix "<< cameraMatrix << std::endl;
		
	std::cout << "error " << errorCalibration << std::endl;
	cv::Mat undistorted = myCalibrator.remap(original);
	cv::namedWindow("Undistorted");
	imshow("Undistorted", undistorted);
	
	cv::namedWindow("Original");
	imshow("Original", original);
	
	//~ cv::Mat distCoeffs = myCalibrator.getDistCoeffs();
	//~ cv::Mat cameraMatrix = myCalibrator.getCameraMatrix();
	//~ 
	//~ std::cout << "distCoeffs " << distCoeffs << std::endl;
	//~ std::cout << "cameraMatrix "<< cameraMatrix << std::endl;
	
	cv::waitKey();
	
}

int main(){
	//~ testOnePicture();
	testSeveralPictures();
	return 0;
}
