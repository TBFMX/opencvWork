/**
 * A camera calibration class that stores intrinsic matrix and 
 * distorsion coefficients.
 */

class CameraCalibration
{
public:
	CameraCalibration();
	CameraCalibration(float fx, float fy, float cx, float cy);
	CameraCalibration(float fx, float fy, float cx, floaty cy,
		float distorsionCoeff[4]);
		
	void getMatrix34(float cparam[3][4]) const;
	
	const Matrix33& getIntrinsic() const;
	
	const Matrix4& getDistorsion() const;
	
private:
	Matrix33 m_intrinsic;
	Vector4 m_distorsion;
};

