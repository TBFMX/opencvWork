#include <GL/glut.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "geometryStructs.h"

int w1 = 0;
int h1 = 0;
void reshape(int w, int h)
{
    w1 = w;
    h1 = h;
    glViewport(0, 0, w, h);
}

void orthogonalStart() 
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-w1/2, w1/2, -h1/2, h1/2);
    glMatrixMode(GL_MODELVIEW);
}

void orthogonalEnd()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}


GLuint texture = 0;

void drawFurnish2()
{
	float w = 1024 ;
	float h = 768;
	float scale = 1.0;
//~ glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//~ glClearColor(0,0,0,0);
  //~ glPushAttrib(GL_COLOR_BUFFER_BIT);// | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_POLYGON_BIT);

  glColor4f(1.0f,1.0f,1.0f,1.0f);         // Full Brightness, 50% Alpha ( NEW )

 //~ // Enable texture mapping stuff
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, texture);
glEnableClientState(GL_TEXTURE_COORD_ARRAY);
glDisableClientState(GL_COLOR_ARRAY);
	
    glEnableClientState(GL_VERTEX_ARRAY);
    
    gluPerspective(45, (float)w / h, 0.1, 100);
    glTranslatef(0, 0.0, 1.00f);
     
    GLfloat mdl[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mdl);
	getCameraOrigin(mdl,&cameraOrigin);
	
	std::cout << "camera origin "   <<  cameraOrigin.x << ", " <<cameraOrigin.y << ", " <<cameraOrigin.z <<std::endl; 

	getFacesNearToCamera(couchNumVerts,cameraOrigin,couchTexCoords,COLORS,couchVerts,
							outTexCoords,outColors,outVertexes,&finalVertexSize);
	
	// scaling the vertexes withon any help
	scaling(scale, outVertexes, outVertexes,finalVertexSize);
	
	glVertexPointer(3, GL_FLOAT, 0, outVertexes);
    glTexCoordPointer(2, GL_FLOAT, 0, outTexCoords);

	//~ glDrawArrays(GL_TRIANGLES, 0, couchNumVerts);
	glDrawArrays(GL_TRIANGLES, 0, finalVertexSize);
	
    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

void background()
{
    orthogonalStart();

    const int iw = 800;
    const int ih = 600;
    
    const GLfloat bgTextureVertices[] = { 0, 0, iw, 0, 0, ih, iw, ih };
	const GLfloat bgTextureCoords[]   = { 1, 0, 1, 1, 0, 0, 0, 1 };
	const GLfloat proj[]              = { 0, -2.f/iw, 0, 0, -2.f/ih, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1 };

    glPushMatrix();
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
	
	glVertexPointer(2, GL_FLOAT, 0, bgTextureVertices);
	glTexCoordPointer(2, GL_FLOAT, 0, bgTextureCoords);

	glColor4f(1,1,1,1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
  
    glPopMatrix();

    orthogonalEnd();
    //~ flush();
}

void display()
{
    glClearColor (1.0,0.0,0.0,0.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    //~ glEnable( GL_TEXTURE_2D );
	
    background();
    
	drawFurnish2();
	//~ drawFurnish();
    glutSwapBuffers();
}

GLuint LoadTexture()
{
	GLuint texName=0;
	//texture image
	cv::Mat m_furnishImage = cv::imread("couch.jpg");

	std::cout << "couch channels "<<m_furnishImage.channels() << std::endl;
	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int w = m_furnishImage.cols;
	int h = m_furnishImage.rows;

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, texName);

	// Upload new texture data:
	if (m_furnishImage.channels() == 3)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, m_furnishImage.data);
	else if(m_furnishImage.channels() == 4)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_furnishImage.data);
	else if (m_furnishImage.channels()==1)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_furnishImage.data);
    return texName; //return whether it was successful
}


int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA);// | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowSize(800,600);
    glutCreateWindow("Aspect Ratio");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    texture = LoadTexture();
    glutMainLoop();
    return 0;
}
