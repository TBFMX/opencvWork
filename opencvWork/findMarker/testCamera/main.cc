#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

// opencv headers
#include<opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/video/tracking.hpp>
#include<opencv2/features2d/features2d.hpp>
#include<opencv2/legacy/legacy.hpp>
#include<opencv2/flann/miniflann.hpp>
//~ #include<opencv2/stitching/stitcher.hpp>
#include<opencv2/stitching/warpers.hpp>
#include<opencv2/calib3d/calib3d.hpp>

#include "GeometryTypes.hpp"
#include "Marker.hpp"
#include "MarkerDetector.hpp"
#include "CameraCalibration.hpp"
#include "ARDrawingContext.hpp"

#define MAX_PERSISTANCE 60

using namespace std;

MarkerDetector gbMarkerDetector;
ARDrawingContext gbDrawingCtx;
CameraCalibration calibration;

bool gbIsWindowUpdated = false;
bool gbIsProcessing = false;

bool gbIsCaptureOpened =true;
bool gbIsCameraTextureInitialized = false;
unsigned int gbCameraTexture=0;
unsigned int gbPersistance = 0;

void showM33(Matrix33 intrinsic){
	for(int i = 0; i < 3; ++i){
		for(int j = 0; j < 3; ++j)
			cout << intrinsic.mat[i][j] << ",";
		cout << endl;
	}
}

void showV4(Vector4 distorsion){
	for(int i = 0; i < 4; ++i)
			cout << distorsion.data[i] << ",";
	cout << endl;
}

void showV3(Vector3 distorsion){
	for(int i = 0; i < 3; ++i)
			cout << distorsion.data[i] << ",";
	cout << endl;
}

void createCameraTexture() {
	unsigned int numTextures = 1;
	// Initialize texture for background image
	if (!gbIsCameraTextureInitialized)
	{
		  // we generate both textures
		//~ LOG("Camera texture Created");
		std::cout << "Camera texture Created" << std::endl;
		glGenTextures(numTextures, &gbCameraTexture);
		glBindTexture(GL_TEXTURE_2D, gbCameraTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gbIsCameraTextureInitialized = true;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void destroyCameraTexture() {
	std::cout << "Camera texture destroyed" << std::endl;
	glDeleteTextures(1, &gbCameraTexture);
	gbIsCameraTextureInitialized = false;
}

void drawCurrentCameraFrame(int texName, int bufferIndex, cv::Mat *currentFrame) {
	
	if(bufferIndex > 0){
		std::cout << "treating to draw a frame" << std::endl;
		//~ pthread_mutex_lock(&FGmutex);
		cv::Mat rgbCameraFrame;
		cvtColor(*currentFrame, rgbCameraFrame, CV_BGR2RGB);
		//~ pthread_mutex_unlock(&FGmutex);
		int w=rgbCameraFrame.cols;
		int h=rgbCameraFrame.rows;
		//LOG_INFO("w,h, channels= %d,%d, %d, %d ",w,h,rgbFrame.channels(), textureId[0]);
		
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, texName);

		if (texName != 0){
			// Upload new texture data:
		if (rgbCameraFrame.channels() == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbCameraFrame.data);
		else if(rgbCameraFrame.channels() == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbCameraFrame.data);
		else if (rgbCameraFrame.channels()==1)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, rgbCameraFrame.data);
		}
		
		//~ glColor4f(1.0f,1.0f,1.0f,1.0f);
		
		// drawing the current frame
		const GLfloat bgTextureVertices[] = { 0, 0, w, 0, 0, h, w, h };
		const GLfloat bgTextureCoords[]   = { 1, 0, 1, 1, 0, 0, 0, 1 };
		const GLfloat proj[]              = { 0, -2.f/w, 0, 0, -2.f/h, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1 };

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(proj);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texName);

		// Update attribute values.
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		
		//~ GLfloat mdl[16];
		//~ glGetFloatv(GL_MODELVIEW_MATRIX, mdl);
		//~ getCameraOrigin(mdl,&cameraOrigin);

		glVertexPointer(2, GL_FLOAT, 0, bgTextureVertices);
		glTexCoordPointer(2, GL_FLOAT, 0, bgTextureCoords);

		glColor4f(1,1,1,1);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);	
	
	}
}

bool processFrame(const cv::Mat& cameraFrame, MarkerDetector& markerDetector, ARDrawingContext& drawingCtx)
{
    //~ LOG_INFO("current buffer index %d",bufferIndex);
    drawingCtx.updateBackground(cameraFrame);

	//~ pipeline.m_patternDetector.homographyReprojectionThreshold += 0.2;
	//~ pipeline.m_patternDetector.homographyReprojectionThreshold = std::min(10.0f, pipeline.m_patternDetector.homographyReprojectionThreshold);
	
    // Find a pattern and update it's detection status:
    drawingCtx.isPatternPresent = markerDetector.processFrame(cameraFrame);
    //~ LOG_INFO("pattern testing");

	std::vector<Transformation> m_transformations;
	m_transformations = markerDetector.getTransformations();
    // Update a pattern pose:
    //~ for( int i = 0; i < m_transformations.size(); ++i)
		//~ drawingCtx.patternPose = m_transformations[i];
    if(m_transformations.size() > 0)
		drawingCtx.patternPose = m_transformations[0];

     //~ LOG_INFO("pose testing %d transformations", m_transformations.size());
     std::cout << "pose testing "  << m_transformations.size()<< " transformations " << std::endl;

    // Request redraw of the window:
    drawingCtx.updateWindow(); // callback to window draw
	std::cout << "can draw" << std::endl;
	
    //~ return drawingCtx.isWindowUpdated(); // always true
    return true;
}

// -- Written in C -- //

Display                 *dpy;
Window                  root;
GLint                   att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo             *vi;
Colormap                cmap;
XSetWindowAttributes    swa;
Window                  win;
GLXContext              glc;
XWindowAttributes       gwa;
XEvent                  xev;
 
int main(int argc, char *argv[]) {
	float fx = 695.4521167717107;
	float fy = 694.5519610122569;
	float cx = 337.2059936807979;
	float cy = 231.1645822893514;
	
    // Change this calibration to yours:
    calibration.set4Params(fx,fy,cx,cy);	
	
	int frameWidth = 640;
	int frameHeight = 480;
	cv::Size frameSize(frameWidth, frameHeight);
    MarkerDetector  markerDetector(calibration);
    gbMarkerDetector = markerDetector;
    ARDrawingContext drawingCtx(frameSize, calibration);
    gbDrawingCtx = drawingCtx;
    gbDrawingCtx.updateFurnishImage();
    gbDrawingCtx.updateTigerImage();
	
	dpy = XOpenDisplay(NULL);

	if(dpy == NULL) {
		printf("\n\tcannot connect to X server\n\n");
		exit(0);
	}
		
	root = DefaultRootWindow(dpy);

	vi = glXChooseVisual(dpy, 0, att);

	if(vi == NULL) {
		printf("\n\tno appropriate visual found\n\n");
		exit(0);
	} 
	else {
		printf("\n\tvisual %p selected\n", (void *)vi->visualid); /* %p creates hexadecimal output like in glxinfo */
	}

	cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask;

	win = XCreateWindow(dpy, root, 0, 0, 640, 480, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

	XMapWindow(dpy, win);
	XStoreName(dpy, win, "VERY SIMPLE APPLICATION");

	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);

	glEnable(GL_DEPTH_TEST);
 
 	// VideoCapture object
	cv::VideoCapture cap(0);
	
	while( char(cv::waitKey(50)) != 'q' && cap.isOpened()) {
		// capturing a frame
		cv::Mat currentFrame;
		cap >> currentFrame;
		// ----------------------------------------------

		//--- rendering in opengl ---
		if(!currentFrame.empty()){
			int bufferIndex = 1;
			//~ glClear(GL_DEPTH_BUFFER_BIT);
			//~ drawFrame(&currentFrame);
			cv::Mat rgbFrame;

			cvtColor(currentFrame, rgbFrame, CV_BGR2RGB);
	
			//~ if(!rgbFrame.empty()){
			gbIsProcessing = true;
			gbIsWindowUpdated = processFrame(rgbFrame, gbMarkerDetector, gbDrawingCtx);
			gbIsProcessing = false;
			
			if(bufferIndex > 0 && gbIsWindowUpdated == true && gbIsProcessing == false){
				//~ pthread_mutex_lock(&FGmutex);
				// we validate if the pattern found was the best
				gbDrawingCtx.validatePatternPresent(); 
				//~ pthread_mutex_unlock(&FGmutex);
				gbIsWindowUpdated = false;
			}			
			
			XGetWindowAttributes(dpy, win, &gwa);
			glViewport(0, 0, gwa.width, gwa.height);
			
			// drawing here----------
			// pthread_mutex_lock(&FGmutex);
			if(gbDrawingCtx.isThereAPattern()){
				gbDrawingCtx.draw();
				gbPersistance = 1;
			}else{
				if(gbPersistance > 0){
					gbDrawingCtx.draw();
					++gbPersistance;
					// LOG_INFO("persistance %d", gbPersistance);
					cout << "persistance " << gbPersistance << std::endl;
					if(gbPersistance > MAX_PERSISTANCE)
						gbPersistance = 0;
					
				}else{
					glClear(GL_DEPTH_BUFFER_BIT);
					createCameraTexture();
					drawCurrentCameraFrame(gbCameraTexture, bufferIndex, &currentFrame);
					destroyCameraTexture();
				}
			}
			// pthread_mutex_unlock(&FGmutex);
			// glFlush();
			//-----------------------

			glXSwapBuffers(dpy, win);
			//~ XNextEvent(dpy, &xev);
			cout << "new frame" << std::endl;
		}
		//---------------------------
	}
	
	glXMakeCurrent(dpy, None, NULL);
	glXDestroyContext(dpy, glc);
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
	exit(0);
} 

