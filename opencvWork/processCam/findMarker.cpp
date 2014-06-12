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

template<typename T> class Matrix33;
typedef Matrix33<float>   Mat33f;
typedef Matrix33<double>  Mat33d;


class CameraCalibration{
public:
	CameraCalibration();
	CameraCalibration(float fx, float fy, float cx, float cy);
	CameraCalibration(float fx, float fy, float cx, float cy, float_t	
						distorsionCoeff[4]);
	void getMatrix34(float cparam[3][4]) const;
	const Matrix33& getIntrinsic() const;
	const Vector4& getDistorsion() const;
private:
	cv::Matrix33 m_intrinsic;
	Vector4 m_distorsion;
};


class MarkerDetector{
public:
	MarkerDetector(CameraCalibration calibration){};
	void processFrame(const BGRAVideoFrame& frame);
	const std::vector<Transformation>& getTransformation() const;
	
protected:
	bool findMarkers(const BGRAVideoFrame& frame, std::vector<Marker>& detectedMarkers);
	void prepareImage(const cv::Mat& bgradMat, cv::Mat& grayscale);
	void performThreshold(const cv::Mat& grayscale, cv::Mat& thresholdImg);
	void findContours(const cv::Mat& thresholdImg, 
						std::vector<std::vector<cv::Point> >& contours,
						int minContourPointsAllowed);
	void findMarkerCandidates(const std::vector<std::vector<cv::Point> >
						&contours, std::vector<Marker>& detectedMarkers);
	void detectMarkers(const cv::Mat& grayscale, std::vector<Marker>& detectedMarkers);
	void estimatePosition(std::vector<Marker>& detetedMarkers);
private:
};
