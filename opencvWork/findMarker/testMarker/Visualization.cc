/*****************************************************************************
 *   SimpleVisualizationController.mm
 *   Example_MarkerBasedAR
 ******************************************************************************
 *   by Khvedchenia Ievgen, 5th Dec 2012
 *   http://computer-vision-talks.com
 ******************************************************************************
 *   Ch2 of the book "Mastering OpenCV with Practical Computer Vision Projects"
 *   Copyright Packt Publishing 2012.
 *   http://www.packtpub.com/cool-projects-with-opencv/book
 *****************************************************************************/

#include "Visualization.h"
#include "GeometryTypes.h"
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include<iostream>

#define FALSE 0
#define TRUE 1
bool gbIsCameraTextureInitialized = false;
unsigned int gbCameraTexture = 0;

void getCameraOrigin(GLfloat mdl[16], point3 *camera_org){
    
    (*camera_org).x = -(mdl[0] * mdl[12] + mdl[1] * mdl[13] + mdl[2] * mdl[14]);
    (*camera_org).y = -(mdl[4] * mdl[12] + mdl[5] * mdl[13] + mdl[6] * mdl[14]);
    (*camera_org).z = -(mdl[8] * mdl[12] + mdl[9] * mdl[13] + mdl[10] * mdl[14]);

}

void buildProjectionMatrix(Matrix33 cameraMatrix, int screen_width, int screen_height, Matrix44& projectionMatrix)
{
    float near = 0.01;  // Near clipping distance
    float far  = 100;  // Far clipping distance
    
    // Camera parameters
    float f_x = cameraMatrix.data[0]; // Focal length in x axis
    float f_y = cameraMatrix.data[4]; // Focal length in y axis (usually the same?)
    float c_x = cameraMatrix.data[2]; // Camera primary point x
    float c_y = cameraMatrix.data[5]; // Camera primary point y
    
    projectionMatrix.data[0] = - 2.0 * f_x / screen_width;
    projectionMatrix.data[1] = 0.0;
    projectionMatrix.data[2] = 0.0;
    projectionMatrix.data[3] = 0.0;
    
    projectionMatrix.data[4] = 0.0;
    projectionMatrix.data[5] = 2.0 * f_y / screen_height;
    projectionMatrix.data[6] = 0.0;
    projectionMatrix.data[7] = 0.0;
    
    projectionMatrix.data[8] = 2.0 * c_x / screen_width - 1.0;
    projectionMatrix.data[9] = 2.0 * c_y / screen_height - 1.0;
    projectionMatrix.data[10] = -( far + near ) / ( far - near );
    projectionMatrix.data[11] = -1.0;
    
    projectionMatrix.data[12] = 0.0;
    projectionMatrix.data[13] = 0.0;
    projectionMatrix.data[14] = -2.0 * far * near / ( far - near );
    projectionMatrix.data[15] = 0.0;
}

void createCameraTexture(){
	unsigned int numTextures = 1;
	// Initialize texture for background image
	if (!gbIsCameraTextureInitialized)
	{
		  // we generate both textures
		//~ LOG("Camera texture Created");
		glGenTextures(numTextures, &gbCameraTexture);
		glBindTexture(GL_TEXTURE_2D, gbCameraTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gbIsCameraTextureInitialized = true;
	}
	glBindTexture(GL_TEXTURE_2D, 0);	
}

void destroyCameraTexture() {
	//~ LOG("Camera texture destroyed");
	glDeleteTextures(1, &gbCameraTexture);
	gbIsCameraTextureInitialized = false;
}

void drawBackground(cv::Mat *currentFrame)
{
	
    GLfloat w = (*currentFrame).cols;
    GLfloat h = (*currentFrame).rows;
    
    // if bgr image, change currentFrame colors
    createCameraTexture();
    
		cv::Mat rgbCameraFrame;
		cvtColor((*currentFrame), rgbCameraFrame, CV_BGR2RGB);
		//~ pthread_mutex_unlock(&FGmutex);
		w=rgbCameraFrame.cols;
		h=rgbCameraFrame.rows;
		//LOG_INFO("w,h, channels= %d,%d, %d, %d ",w,h,rgbFrame.channels(), textureId[0]);
		
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, gbCameraTexture);

		if (gbCameraTexture != 0){
			// Upload new texture data:
		if (rgbCameraFrame.channels() == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbCameraFrame.data);
		else if(rgbCameraFrame.channels() == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbCameraFrame.data);
		else if (rgbCameraFrame.channels()==1)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, rgbCameraFrame.data);
		}

    const GLfloat squareVertices[] =
    {
        0, 0,
        w, 0,
        0, h,
        w, h
    };
    
     static const GLfloat textureVertices[] =
     {
     1, 0,
     1, 1,
     0, 0,
     0, 1
     };
    
    static const GLfloat proj[] =
    {
        0, -2.f/w, 0, 0,
        -2.f/h, 0, 0, 0,
        0, 0, 1, 0,
        1, 1, 0, 1
    };
    
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(proj);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
		point3 cameraOrigin;
 		glPushMatrix();
		GLfloat mdl[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, mdl);
		getCameraOrigin(mdl,&cameraOrigin);
		glPopMatrix();
		
		std::cout << "x,y,z = " << cameraOrigin.x << ", " << cameraOrigin.y 
			<<", " << cameraOrigin.z << std::endl;   
    //~ glDepthMask(FALSE);
    //~ glDisable(GL_COLOR_MATERIAL);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gbCameraTexture);
    
    // Update attribute values.
    
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    
    glTexCoordPointer(2, GL_FLOAT, 0, textureVertices);
	glVertexPointer(2, GL_FLOAT, 0, squareVertices);
    
    glColor4f(1,1,1,1);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

void drawAR()
{
	cv::Size m_frameSize(640,480);
    Matrix44 projectionMatrix;
	float fx = 695.4521167717107;
	float cx = 337.2059936807979;
	float fy = 694.5519610122569;
	float cy = 231.1645822893514;

	//~ distCoeffs[4] ={0.1307141734428741, -0.1324767531982809, -0.01078250514591116, 0.01015178201016525, -0.6546831239744588}
	float distCoeffs[4] ={0.1307141734428741, -0.1324767531982809, -0.01078250514591116, 0.01015178201016525};
	CameraCalibration calibration(fx,fy,cx,cy,distCoeffs);
	
	//~ cv::Mat frame1 = cv::imread("images/frame1.jpg");
	//~ cv::Mat frame1 = cv::imread("images/twoMarkers1.jpg");
	cv::Mat frame1 = cv::imread("images/smallMarker.jpg");
	//~ cv::Mat frame1 = cv::imread("images/verySmall3.jpg");
	MarkerDetector myMarkerDetector(calibration);

	// prepare image
	myMarkerDetector.processFrame(frame1);	
	std::vector<Transformation> m_transformations;
	m_transformations = myMarkerDetector.getTransformations();
	
	//~ std::cout << " transformations " << m_transformations.size() << std::endl;
	
	//~ m_transformations[0].r();
	//~ std::cout << std::endl;
	//~ m_transformations[0].t();
	
    buildProjectionMatrix(calibration.getIntrinsic(),m_frameSize.width ,m_frameSize.height,projectionMatrix);
 
    glClear(GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projectionMatrix.data);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //~ glDepthMask(TRUE);
    //~ glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    //glDepthFunc(GL_GREATER);
    
    //~ glEnableClientState(GL_VERTEX_ARRAY);
    //~ glEnableClientState(GL_NORMAL_ARRAY);
    
    //~ glPushMatrix();
    glLineWidth(3.0f);
    
    const float xyzAxis[] = {0,0,0,1,0,0, //x
					0,0,0,0,1,0, //y
					0,0,0,0,0,1}; //z
	
	const float axisColors[] ={
		1.0, 0.0, 0.0, 1.0, // r
		1.0, 0.0, 0.0, 1.0, // r
		0.0, 1.0, 1.0, 1.0, // g
		0.0, 1.0, 1.0, 1.0, // g
		0.0, 0.0, 1.0, 1.0, // b
		0.0, 0.0, 1.0, 1.0 // b
	};
    
    const GLfloat squareVertices[] = {
        -0.5f, -0.5f,
        0.5f,  -0.5f,
        -0.5f,  0.5f,
        0.5f,   0.5f,
    };
    
    const GLfloat squareColors[] = {
       1, 1,   0, 1,
        0,   1, 1, 1,
        0,     1,  1,   1,
        1,   0, 1, 1,
    };
    
	for (size_t transformationIndex=0; transformationIndex < m_transformations.size(); transformationIndex++)
	{
		const Transformation& transformation = m_transformations[transformationIndex];

		Matrix44 glMatrix = transformation.getMat44();
				
		glLoadMatrixf(reinterpret_cast<const GLfloat*>(&glMatrix.data[0]));

		point3 cameraOrigin;
		
		glPushMatrix();
		GLfloat mdl[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, mdl);
		getCameraOrigin(mdl,&cameraOrigin);
		glPopMatrix();
		
		std::cout << "x,y,z = " << cameraOrigin.x << ", " << cameraOrigin.y 
			<<", " << cameraOrigin.z << std::endl;
		
		glEnableClientState(GL_COLOR_ARRAY); 
		glEnableClientState(GL_VERTEX_ARRAY);
		
		// draw data
		glVertexPointer(2, GL_FLOAT, 0, squareVertices);
		glColorPointer(4, GL_FLOAT, 0, squareColors);
				
		//~ std::cout << "We are here" << std::endl;
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		//~ float scale = 0.5;
		//~ glScalef(scale, scale, scale);

		//~ glTranslatef(0, 0, 0.1f);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glColorPointer(4,GL_FLOAT, 0, axisColors);
		glVertexPointer(3, GL_FLOAT, 0, xyzAxis);
		
		glDrawArrays(GL_LINES, 0, 6);
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
	}

	//~ glPopMatrix();
}

void drawFrame(cv::Mat *currentFrame)
{   
	
    // Draw a video on the background
    drawBackground(currentFrame);
    
    // Draw 3D objects on the position of the detected markers
    drawAR();
}
