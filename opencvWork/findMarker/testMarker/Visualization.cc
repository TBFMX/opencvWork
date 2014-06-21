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

void drawBackground()
{
    GLfloat w = 640;// = m_glview.bounds.size.width;
    GLfloat h = 480;// = m_glview.bounds.size.height;
    
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
    
    glDepthMask(FALSE);
    glDisable(GL_COLOR_MATERIAL);
    
    glEnable(GL_TEXTURE_2D);
    static GLuint m_backgroundTextureId;
    glBindTexture(GL_TEXTURE_2D, m_backgroundTextureId);
    
    // Update attribute values.
    glVertexPointer(2, GL_FLOAT, 0, squareVertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, textureVertices);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
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
	
	cv::Mat frame1 = cv::imread("images/frame1.jpg");
	MarkerDetector myMarkerDetector(calibration);

	// prepare image
	myMarkerDetector.processFrame(frame1);	
	std::vector<Transformation> m_transformations;
	m_transformations = myMarkerDetector.getTransformations();
	std::cout << m_transformations.size() << std::endl;
	m_transformations[0].r();
	std::cout << std::endl;
	m_transformations[0].t();
	
    buildProjectionMatrix(calibration.getIntrinsic(),m_frameSize.width ,m_frameSize.height,projectionMatrix);
    
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projectionMatrix.data);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glDepthMask(TRUE);
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    //glDepthFunc(GL_GREATER);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    
    glPushMatrix();
    glLineWidth(3.0f);
    
    float lineX[] = {0,0,0,1,0,0};
    float lineY[] = {0,0,0,0,1,0};
    float lineZ[] = {0,0,0,0,0,1};
    
    const GLfloat squareVertices[] = {
        -0.5f, -0.5f,
        0.5f,  -0.5f,
        -0.5f,  0.5f,
        0.5f,   0.5f,
    };
    const GLubyte squareColors[] = {
        255, 255,   0, 255,
        0,   255, 255, 255,
        0,     0,   0,   0,
        255,   0, 255, 255,
    };
    
    
    for (size_t transformationIndex=0; transformationIndex < m_transformations.size(); transformationIndex++)
    {
        const Transformation& transformation = m_transformations[transformationIndex];

        Matrix44 glMatrix = transformation.getMat44();
                
        glLoadMatrixf(reinterpret_cast<const GLfloat*>(&glMatrix.data[0]));
        
        // draw data
        glVertexPointer(2, GL_FLOAT, 0, squareVertices);
        glEnableClientState(GL_VERTEX_ARRAY);
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
        glEnableClientState(GL_COLOR_ARRAY);
        std::cout << "We are here" << std::endl;
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      
        glDisableClientState(GL_COLOR_ARRAY);
         
        float scale = 0.5;
        glScalef(scale, scale, scale);
        
        glTranslatef(0, 0, 0.1f);
        
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glVertexPointer(3, GL_FLOAT, 0, lineX);
        glDrawArrays(GL_LINES, 0, 2);
        
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        glVertexPointer(3, GL_FLOAT, 0, lineY);
        glDrawArrays(GL_LINES, 0, 2);
        
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        glVertexPointer(3, GL_FLOAT, 0, lineZ);
        glDrawArrays(GL_LINES, 0, 2);
        
    }
    
    glPopMatrix();
    glDisableClientState(GL_VERTEX_ARRAY);
}

void drawFrame()
{   
    // Draw a video on the background
    drawBackground();
    
    // Draw 3D objects on the position of the detected markers
    drawAR();
}
