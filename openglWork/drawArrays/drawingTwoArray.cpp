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
    //~ gluOrtho2D(-w1/2, w1/2, -h1/2, h1/2);
    glOrtho(-w1/2, w1/2, -h1/2, h1/2,-1,1);
    glMatrixMode(GL_MODELVIEW);
}

void orthogonalEnd()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}


GLuint texture = 0;

void startTexture(GLuint texture){
	// Enable texture mapping stuff
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
}

void endTexture(){
	glDisable(GL_TEXTURE_2D);
}

void startArrays(){
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void endArrays(){
	glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);
}

void drawFurnish2()
{
	float w = 1024 ;
	float h = 768;
	float scale = 1.0;

	//~ startTexture(texName);
	//~ startArrays();
    gluPerspective(45, (float)w / h, 0.1, 100);
    glTranslatef(0, 0.0, -1.00f);
     
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

	glColor4f(1.0f,1.0f,1.0f,1.0f); 
	glDrawArrays(GL_TRIANGLES, 0, finalVertexSize);
	//~ endArrays();
	//~ endTexture();
}

void background()
{
    //~ orthogonalStart();
	//~ startTexture(texName);
	//~ startArrays();
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


	//~ glEnable(GL_TEXTURE_2D);
	//~ glBindTexture(GL_TEXTURE_2D, texName);

	//~ // Update attribute values.
	//~ glEnableClientState(GL_VERTEX_ARRAY);
	//~ glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//~ glDisableClientState(GL_COLOR_ARRAY);
	
	glVertexPointer(2, GL_FLOAT, 0, bgTextureVertices);
	glTexCoordPointer(2, GL_FLOAT, 0, bgTextureCoords);

	glColor4f(1,1,1,1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//~ glDisableClientState(GL_VERTEX_ARRAY);
	//~ glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//~ glDisable(GL_TEXTURE_2D);
	//~ endArrays();
	//~ endTexture();
    //~ glPopMatrix();

    //~ orthogonalEnd();
}

void display()
{
    glClearColor (1.0,0.0,0.0,0.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    //~ glEnable( GL_TEXTURE_2D );
    
	orthogonalStart();
	startTexture(texName);
	startArrays();
    background();
    //~ endArrays();
    //~ endTexture();
    orthogonalEnd();
    
    //~ startTexture(texName);
    //~ startArrays();
	drawFurnish2();
	endArrays();
	endTexture();
    glutSwapBuffers();
   
}

//~ GLuint LoadTexture()
//~ {
    //~ unsigned char data[] = { 255,0,0, 0,255,0, 0,0,255, 255,255,255 };
//~ 
    //~ glGenTextures( 1, &texture ); 
    //~ glBindTexture( GL_TEXTURE_2D, texture ); 
    //~ glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    //~ glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); 
//~ 
    //~ //even better quality, but this will do for now.
    //~ glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //~ glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
//~ 
    //~ //to the edge of our shape. 
    //~ glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    //~ glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
//~ 
    //~ //Generate the texture
    //~ glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0,GL_RGB, GL_UNSIGNED_BYTE, data);
    //~ return texture; //return whether it was successful
//~ }

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
