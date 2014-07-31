/*****************************************************************************
*   Markerless AR desktop application.
******************************************************************************
*   by Khvedchenia Ievgen, 5th Dec 2012
*   http://computer-vision-talks.com
******************************************************************************
*   Ch3 of the book "Mastering OpenCV with Practical Computer Vision Projects"
*   Copyright Packt Publishing 2012.
*   http://www.packtpub.com/cool-projects-with-opencv/book
*****************************************************************************/

#ifndef DrawingContext_HPP
#define DrawingContext_HPP

////////////////////////////////////////////////////////////////////
// File includes:
#include "GeometryTypes.hpp"
#include "geometryStructs.hpp"
#include "CameraCalibration.hpp"

#include "SolidSphere.hpp"
////////////////////////////////////////////////////////////////////
// Standard includes:
#include <opencv2/opencv.hpp>

class DrawingContext
{
public:
	DrawingContext();
	DrawingContext(cv::Size frameSize, const CameraCalibration& c);
	~DrawingContext();

	bool                isAPatternPresent;
	bool				isTigerPresent;
	bool 				isLightPresent;
	bool 				isFurnishPresent;
	Transformation      furnishPose;
	Transformation      tigerPose;
	Transformation      lightPose;


	//! Set the new frame for the background
	void updateBackground(const cv::Mat& frame);
	void updateFurnishImage();
	void updateTigerImage();
	void updateWindow();
	//! Render entire scene in the OpenGl window
	void draw();
	void drawPersistance();
	void setHeight(unsigned);
	void setWidth(unsigned);
	void createTexture();
	void destroyTexture();
	void renderToFrame();
	void renderToScreen();
	void destroyFramebuffer();
	bool isWindowUpdated();
	void validatePatternPresent();
	//! Draws the background with video
	void drawCameraFrame(unsigned int, int,int,bool);
	//! Draws the coordinate axis 
	void drawCoordinateAxis();
	//! Draws the AR
	void drawAugmentedScene(float lPosition[]);
	
	void drawAugmentedPersistance(float lPosition[]);
	
	//! Draw furnish
	void drawFurnish(float lPosition[]);
	
	//! Draw tiger
	void drawTiger(float lPosition[]);
	
	void drawTigerShadow(float lPosition[]);
	
	void drawSpot(float lPosition[]);
	
	bool isThereAPattern();
	
	void setObjectToDraw(int objectId);
	int objectToDraw();
	
private:
  //! Builds the right projection matrix from the camera calibration for AR
  void buildProjectionMatrix(const CameraCalibration& calibration, int w, int h, Matrix44& result);

  //! Draw the cube model
  void drawCubeModel();

  bool               m_isBackgroundTextureInitialized;
  bool 				 m_isFurnishTextureInitialized;
  bool 				 m_isTigerTextureInitialized;
  CameraCalibration  m_calibration;
  cv::Mat            m_backgroundImage;
  cv::Mat			 m_furnishImage;
  cv::Mat 			 m_tigerImage;
  unsigned int 		 m_width;
  unsigned int 		 m_height;
  unsigned int 		 m_textureId[3];
	GLuint		 	 m_framebufferName;
	GLuint			 m_depthrenderbuffer;
  unsigned int 		 m_isWindowUpdated;
  bool				 m_isAPatternPresent;
  bool 				 m_isLightPresent;
  bool				 m_isTigerPresent;
  bool 				 m_isFurnishPresent;
  int 				 m_objectToDraw;
  Matrix44			 m_persistentTigerPose;
  Matrix44			 m_persistentTigerProjection;
  Matrix44			 m_persistentFurnishPose;
  Matrix44			 m_persistentFurnishProjection;
  Matrix44			 m_persistentLightPose;
  Matrix44			 m_persistentLightProjection;
  double 			 m_angle;
  point3			 m_cameraPositionLight1;
  point3			 m_cameraPositionTiger;
  //~ std::string        m_windowName;
};

#endif
