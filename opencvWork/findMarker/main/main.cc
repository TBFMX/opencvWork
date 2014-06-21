#include <iostream>
#include "GeometryTypes.h"
#include "MarkerDetector.h"
#include "CameraCalibration.h"
using namespace std;

void showIntrinsic(Matrix33 intrinsic){
	for(int i = 0; i < 3; ++i){
		for(int j = 0; j < 3; ++j)
			cout << intrinsic.mat[i][j] << ",";
		cout << endl;
	}
}

void showDistorsion(Vector4 distorsion){
	for(int i = 0; i < 4; ++i)
			cout << distorsion.data[i] << ",";
	cout << endl;
}


void testMarkerDetector(){
	Matrix33 intrinsic;
	Vector4 distorsion;
	
	
	float fx = 695.4521167717107;
	float cx = 337.2059936807979;
	float fy = 694.5519610122569;
	float cy = 231.1645822893514;

	//~ distCoeffs[4] ={0.1307141734428741, -0.1324767531982809, -0.01078250514591116, 0.01015178201016525, -0.6546831239744588}
	float distCoeffs[4] ={0.1307141734428741, -0.1324767531982809, -0.01078250514591116, 0.01015178201016525};
	CameraCalibration calibration(fx,fy,cx,cy,distCoeffs);
	
	//~ cout << "old right" << endl;
	//~ distorsion = calibration.getDistorsion();
	//~ intrinsic = calibration.getIntrinsic();
	//~ showIntrinsic(intrinsic);
	//~ showDistorsion(distorsion);	
	
	cv::Mat frame1 = cv::imread("images/frame1.jpg");
	//~ cv::Mat frame1Gray;
	//~ cv::namedWindow("Frame1");
	//~ cv::imshow("Frame1", frame1);
	//~ cv::waitKey();
	MarkerDetector myMarkerDetector(calibration);
	// prepare image
	myMarkerDetector.processFrame(frame1);
	
}



int main(){
	testMarkerDetector();
	
	return 0;
}
