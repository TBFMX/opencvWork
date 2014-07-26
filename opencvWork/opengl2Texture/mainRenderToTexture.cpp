#include <iostream>
#include <GL/glew.h> 
#include <GL/glut.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cmath>

#include "geometryStructs.h"

// global variables
GLuint FramebufferName = 0;
GLuint renderedTexture = 0;
GLuint depthrenderbuffer;
GLuint quad_VertexArrayID; // fullscreen
GLuint quad_vertexbuffer;

int screenWidth = 1024;
int screenHeight = 768;
	
// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.


void genBuffers() {
	glGenFramebuffers(1, &FramebufferName);
	std::cout << "gen buffer well"<< std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
}

void genTextures(){
	// The texture we're going to render to
	glGenTextures(1, &renderedTexture);
}

void bindTextures(){
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	 
	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, screenWidth, screenHeight, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
	 
	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void genDepthRender(){
	// The depth buffer
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
}

void setRenderedTexture(){
	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
}

void setDrawBufferList(){
	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
}

bool isBufferOK(){
	// Always check that our framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;
	return true;
}


void renderToBuffer(){
	// Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glViewport(0,0,screenWidth,screenHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right
	//layout(location = 0) out vec3 color;
}

void bindFullscreen(){
	// The fullscreen quad's FBO

	glGenVertexArrays(1, &quad_VertexArrayID);
	glBindVertexArray(quad_VertexArrayID);
}
 
static const GLfloat g_quad_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
};
 
void genQuadBuffer(){
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
}
 
 
GLuint LoadTexture()
{std::cout << "buffers well"<< std::endl;

	genDepthRender();
	setRenderedTexture();
	setDrawBufferList();
	while(!isBufferOK()){
	}
	
	return renderedTexture; //return whether it was successful
} 
//~ void createCompileProgram(){
	//~ // Create and compile our GLSL program from the shaders
	//~ GLuint quad_programID = LoadShaders( "Passthrough.vertexshader", "SimpleTexture.fragmentshader" );
	//~ GLuint texID = glGetUniformLocation(quad_programID, "renderedTexture");
	//~ GLuint timeID = glGetUniformLocation(quad_programID, "time");
//~ }
//~ 
//~ // Render to the screen
//~ glBindFramebuffer(GL_FRAMEBUFFER, 0);
//~ glViewport(0,0,1024,768); // Render on the whole framebuffer, complete from the lower left corner to the upper right
 //~ 
//~ in vec2 UV;
 //~ 
//~ out vec3 color;
 //~ 
//~ uniform sampler2D renderedTexture;
//~ uniform float time;
 //~ 
 


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

	//~ glDrawArrays(GL_TRIANGLES, 0, couchNumVerts);
	glDrawArrays(GL_TRIANGLES, 0, finalVertexSize);
	
    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

void background()
{
	GLuint texName =  renderedTexture;
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
    glutSwapBuffers();
}

//~ GLuint LoadTexture()
//~ {
	//~ GLuint texName=0;
	//~ //texture image
	//~ cv::Mat m_furnishImage = cv::imread("couch.jpg");
//~ 
	//~ std::cout << "couch channels "<<m_furnishImage.channels() << std::endl;
	//~ glBindTexture(GL_TEXTURE_2D, texName);
//~ 
	//~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//~ int w = m_furnishImage.cols;
	//~ int h = m_furnishImage.rows;
//~ 
	//~ glPixelStorei(GL_PACK_ALIGNMENT, 1);
	//~ glBindTexture(GL_TEXTURE_2D, texName);
//~ 
	//~ // Upload new texture data:
	//~ if (m_furnishImage.channels() == 3)
	//~ glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, m_furnishImage.data);
	//~ else if(m_furnishImage.channels() == 4)
	//~ glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_furnishImage.data);
	//~ else if (m_furnishImage.channels()==1)
	//~ glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_furnishImage.data);
    //~ return texName; //return whether it was successful
//~ }




int main(int argc, char **argv)
{
	genBuffers();
	genTextures();
	bindTextures();
	
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA);// | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowSize(screenWidth,screenHeight);
    glutCreateWindow("Aspect Ratio");

    glutDisplayFunc(display);
    std::cout << "display well"<< std::endl;
    glutReshapeFunc(reshape);
     std::cout << "reshape well"<< std::endl;
    texture = LoadTexture();
    std::cout << "texture well"<< std::endl;
    glutMainLoop();
    return 0;
}
 

