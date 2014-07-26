#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

#include "GeometryTypes.h"
#include "Marker.h"
#include "MarkerDetector.h"
#include "CameraCalibration.h"
#include "Visualization.h"
using namespace std;

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
	//~ showM33(intrinsic);
	//~ showV4(distorsion);	
	
	cv::Mat frame1 = cv::imread("images/frame1.jpg");
	//~ cv::Mat frame1Gray;
	//~ cv::namedWindow("Frame1");
	//~ cv::imshow("Frame1", frame1);
	//~ cv::waitKey();
	MarkerDetector myMarkerDetector(calibration);
	std::vector<Transformation> transformations;
	// prepare image
	myMarkerDetector.processFrame(frame1);
	transformations = myMarkerDetector.getTransformations();
	
	cout << "transformations size "<<  transformations.size() << endl;
	showM33(transformations[0].r());
	cout << endl;
	showV3(transformations[0].t());
	//~ Marker nMarker;
	//~ cout << nMarker.id;
	cv::namedWindow("Frame1");
	cv::imshow("Frame1", frame1);
	cv::waitKey();
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
 
	while(1) {
		// --------- simulates a camera frame -----------
		cv::Mat currentFrame = cv::imread("images/currentFrame.jpg");

		// ----------------------------------------------
		XNextEvent(dpy, &xev);

		if(xev.type == Expose) {
			XGetWindowAttributes(dpy, win, &gwa);
			glViewport(0, 0, gwa.width, gwa.height);
			//--- rendering in opengl ---
			if(!currentFrame.empty())
				drawFrame(&currentFrame);
			//---------------------------
			
			glXSwapBuffers(dpy, win);
		}

		else if(xev.type == KeyPress) {
			glXMakeCurrent(dpy, None, NULL);
			glXDestroyContext(dpy, glc);
			XDestroyWindow(dpy, win);
			XCloseDisplay(dpy);
			exit(0);
		}
	}
} 

