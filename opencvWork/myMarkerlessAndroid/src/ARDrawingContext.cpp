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

////////////////////////////////////////////////////////////////////
// File includes:
#include "ARDrawingContext.hpp"

////////////////////////////////////////////////////////////////////
// Standard includes:
#include <GL/gl.h>
#include <GL/glu.h>

#include "couch.h"
#include "rossiniColors.h"

unsigned finalVertexSize;

#define maxObjSize 1596
float outNormals[maxObjSize*3];
float outTexCoords[maxObjSize*2];
float outVertexes[maxObjSize*3];
float outColors[maxObjSize][4];

/*  Create checkerboard texture  */
#define checkImageWidth 64
#define checkImageHeight 64
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

struct point3{
	float x;
	float y;
	float z;
};

struct triangle{
	point3 p1;
	point3 p2;
	point3 p3;
};

struct texture{
	point3 p1;
	point3 p2;
};

struct face{
	triangle f;
	texture t;
	float d; // distance
};

// for ordering from the farthest to nearest
bool farToCamera(face a, face b){
	return a.d > b.d;
};

float getDistance(point3 a, point3 b){
	return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

point3 cameraOrigin;

double internalProduct(const point3 *a, const point3 *b){
	return (*a).x * (*b).x + (*a).y * (*b).y + (*a).z * (*b).z;
}

point3 vectorProduct(const point3 *a, const point3 *b){
	point3 c;
	c.x = (*a).y * (*b).z - (*a).z * (*b).y;
	c.y = -(*a).x * (*b).z + (*a).z * (*b).x;
	c.z = (*a).x * (*b).y - (*a).y * (*b).x;
	return c;
}

// vector comes from a and goes to b
point3 directedVector(point3 *a, point3 * b){
	
	point3 a2b;
	a2b.x = (*b).x - (*a).x;
	a2b.y = (*b).y - (*a).y;
	a2b.z = (*b).z - (*a).z;
	
	return a2b;
}
#define EPSILON 1e-7

int intersectTriangle(point3 *orig, point3 *dir, point3 *v1, point3 *v2, point3 *v3,
		float *t, float *u, float *v){
	point3 e1, e2, tV, pV, qV;
	float det, invDet;
	
	// find vectors for two edges sharing v2
	e1 = directedVector(v1,v2);
	e2 = directedVector(v1,v3);
	
	// begin calculating determinant - also used to calculate U parameter
	pV = vectorProduct(dir,&e2);
	
	//if determinant is near zero, ray lies in plane of triangle
	det = internalProduct(&e1,&pV);
	
	if(det > -EPSILON && det < EPSILON)
		return 0;
		
	invDet = 1.0 / det;
	//calculate distance from v1 to ray origin
	tV = directedVector(v1, orig);
	
	// calculate U parameter and test bounds
	*u = internalProduct(&tV, &pV) * invDet;
	
	if(*u < 0.0 || *u > 1.0)
		return 0;
		
	// prepare to test V parameter
	qV = vectorProduct(&tV,&e1);
		
	// calculate V parameter and test bounds
	*v = internalProduct(dir,&qV) * invDet;
	if(*v < 0.0 || *u + *v > 1.0)
		return 0;
	
	// calculate t, ray intersects triangle 
	*t = internalProduct(&e2, &qV) * invDet;
	
	return 1;
}

int intersectFace(point3 *orig, point3 *dir, point3 *v1, point3 *v2, point3 *v3){
	//~ float t;
	float u, v;
	point3 e1, e2, tV, pV, qV;
	float det, invDet;
	
	// find vectors for two edges sharing v2
	e1 = directedVector(v1,v2);
	e2 = directedVector(v1,v3);
	
	// begin calculating determinant - also used to calculate U parameter
	pV = vectorProduct(dir,&e2);
	
	//if determinant is near zero, ray lies in plane of triangle
	det = internalProduct(&e1,&pV);
	
	if(det > -EPSILON && det < EPSILON)
		return 0;
		
	invDet = 1.0 / det;
	//calculate distance from v1 to ray origin
	tV = directedVector(v1, orig);
	
	// calculate U parameter and test bounds
	u = internalProduct(&tV, &pV) * invDet;
	
	if(u < 0.0 || u > 1.0)
		return 0;
		
	// prepare to test V parameter
	qV = vectorProduct(&tV,&e1);
		
	// calculate V parameter and test bounds
	v = internalProduct(dir,&qV) * invDet;
	if(v < 0.0 || u + v > 1.0)
		return 0;
	
	// calculate t, ray intersects triangle 
	//~ t = internalProduct(&e2, &qV) * invDet;
	
	return 1;
}
 
void getCameraOrigin(GLfloat mdl[16], point3 *camera_org){
    
    (*camera_org).x = -(mdl[0] * mdl[12] + mdl[1] * mdl[13] + mdl[2] * mdl[14]);
    (*camera_org).y = -(mdl[4] * mdl[12] + mdl[5] * mdl[13] + mdl[6] * mdl[14]);
    (*camera_org).z = -(mdl[8] * mdl[12] + mdl[9] * mdl[13] + mdl[10] * mdl[14]);

}

void getFacesNearToCamera(unsigned vertexesSize, point3 cameraOrigin,float inTexcoords[], float inColors[][4], float inVertexes[],
								float outTexCoords[], float outColors[][4], float outVertexes[], unsigned *finalVertexes){
	std::vector<face> faces;
	
	unsigned saved = 0;

	// we divide vertexes size by 3 because each face has 3 vertexes
	for(unsigned i  = 0; i < vertexesSize/3; ++i){

		// we realize the vector product according to hand right rule
		
		point3 vertexCoord1;
		vertexCoord1.x = inVertexes[9*i];
		vertexCoord1.y = inVertexes[9*i + 1];
		vertexCoord1.z = inVertexes[9*i + 2];
		
		point3 vertexCoord2;
		vertexCoord2.x = inVertexes[9*i + 3];
		vertexCoord2.y = inVertexes[9*i + 4];
		vertexCoord2.z = inVertexes[9*i + 5];
		
		point3 vertexCoord3;
		vertexCoord3.x = inVertexes[9*i + 6];
		vertexCoord3.y = inVertexes[9*i + 7];
		vertexCoord3.z = inVertexes[9*i + 8];

		// this vector comes from p2 and goes to p1
		point3 vT1 = directedVector(&vertexCoord2,&vertexCoord1);
		// this vector comes from p2 and goes to p3
		point3 vT2 = directedVector(&vertexCoord2,&vertexCoord3);
		// we realize the vector product according to hand right rule
		point3 normalOutFace = vectorProduct(&vT2,&vT1);
		
		point3 faceCenter;
		faceCenter.x = (vertexCoord1.x + vertexCoord2.x + vertexCoord3.x)/3.0;
		faceCenter.y = (vertexCoord1.y + vertexCoord2.y + vertexCoord3.y)/3.0;
		faceCenter.z = (vertexCoord1.z + vertexCoord2.z + vertexCoord3.z)/3.0;
		// vector from triangle face to camera origin;
		point3 tF2cO = directedVector(&faceCenter,&cameraOrigin);

		// calcule internal product between gC2TC and normalOutFace
		// if internal product greater or equal than zero 
		//   save the coords that are facing to camera
		//~ if (internalProduct(&tF2cO,&normalOutFace) > 0){
		
		if (internalProduct(&tF2cO,&normalOutFace) > EPSILON){

			point3 dir = directedVector(&cameraOrigin,&faceCenter);
			
			if (intersectFace(&cameraOrigin, &dir, &vertexCoord1, &vertexCoord2, &vertexCoord3) == 1) {
			//~ int k = saved;
			face currFace;
			
			currFace.d = getDistance(faceCenter,cameraOrigin);
			//~ outColors[3*k][0] = inColors[3*i][0];
			//~ outColors[3*k][1] = inColors[3*i][1];
			//~ outColors[3*k][2] = inColors[3*i][2];
			//~ outColors[3*k][3] = inColors[3*i][3];
			//~ outColors[3*k + 1][0] = inColors[3*i + 1][0];
			//~ outColors[3*k + 1][1] = inColors[3*i + 1][1];
			//~ outColors[3*k + 1][2] = inColors[3*i + 1][2];
			//~ outColors[3*k + 1][3] = inColors[3*i + 1][3];
			//~ outColors[3*k + 2][0] = inColors[3*i + 2][0];
			//~ outColors[3*k + 2][1] = inColors[3*i + 2][1];
			//~ outColors[3*k + 2][2] = inColors[3*i + 2][2];
			//~ outColors[3*k + 2][3] = inColors[3*i + 2][3];
			
			currFace.f.p1.x = inVertexes[9*i];
			currFace.f.p1.y = inVertexes[9*i + 1];
			currFace.f.p1.z = inVertexes[9*i + 2];
			currFace.f.p2.x = inVertexes[9*i + 3];
			currFace.f.p2.y = inVertexes[9*i + 4];
			currFace.f.p2.z = inVertexes[9*i + 5];
			currFace.f.p3.x = inVertexes[9*i + 6];
			currFace.f.p3.y = inVertexes[9*i + 7];
			currFace.f.p3.z = inVertexes[9*i + 8];
			
			currFace.t.p1.x = inTexcoords[6*i];
			currFace.t.p1.y  = inTexcoords[6*i + 1];
			currFace.t.p1.z  = inTexcoords[6*i + 2];
			currFace.t.p2.x  = inTexcoords[6*i + 3];
			currFace.t.p2.y  = inTexcoords[6*i + 4];
			currFace.t.p2.z  = inTexcoords[6*i + 5];

			faces.push_back(currFace);
			++saved;
			}
		}
	}
	
	std::sort(faces.begin(),faces.end(),farToCamera);
	
	std::vector<face>::iterator itFaces;
	int k = 0;
	for(itFaces = faces.begin(); itFaces < faces.end(); ++itFaces){
		outVertexes[9*k]=(*itFaces).f.p1.x;
		outVertexes[9*k + 1]=(*itFaces).f.p1.y;
		outVertexes[9*k + 2]=(*itFaces).f.p1.z;
		outVertexes[9*k + 3]=(*itFaces).f.p2.x;
		outVertexes[9*k + 4]=(*itFaces).f.p2.y;
		outVertexes[9*k + 5]=(*itFaces).f.p2.z;
		outVertexes[9*k + 6]=(*itFaces).f.p3.x;
		outVertexes[9*k + 7]=(*itFaces).f.p3.y;
		outVertexes[9*k + 8]=(*itFaces).f.p3.z;
		
		outTexCoords[6*k]=(*itFaces).t.p1.x;
		outTexCoords[6*k + 1]=(*itFaces).t.p1.y;
		outTexCoords[6*k + 2]=(*itFaces).t.p1.z;
		outTexCoords[6*k + 3]=(*itFaces).t.p2.x;
		outTexCoords[6*k + 4]=(*itFaces).t.p2.y;
		outTexCoords[6*k + 5]=(*itFaces).t.p2.z;
		
		++k;
	}

	*finalVertexes = 3*saved;
}

void scaling(float scale, float inVertexes[], float outVertexes[], unsigned vertexesSize){
	for(unsigned i  = 0; i < vertexesSize*3; ++i){
		outVertexes[i] = inVertexes[i]*scale;
	}
}

static void makeCheckImage(void)
{
   int i, j, a,b,c;
    
   for (i = 0; i < checkImageHeight; i++) {
      for (j = 0; j < checkImageWidth; j++) {
         a = rand()*255;
         b = rand()*255;
         c = rand()*255;
         checkImage[i][j][0] = (GLubyte) a;
         checkImage[i][j][1] = (GLubyte) b;
         checkImage[i][j][2] = (GLubyte) c;
         checkImage[i][j][3] = (GLubyte) 255;
      }
   }
}

void ARDrawingContextDrawCallback(void* param)
{
    ARDrawingContext * ctx = static_cast<ARDrawingContext*>(param);
    if (ctx)
    {
        ctx->draw();
    }
}

ARDrawingContext::ARDrawingContext(std::string windowName, cv::Size frameSize, const CameraCalibration& c)
  : m_isTextureInitialized(false)
  , m_isFurnishTextureInitialized(false)
  , m_calibration(c)
  , m_windowName(windowName)
{
    // Create window with OpenGL support
    cv::namedWindow(windowName, cv::WINDOW_OPENGL);

    // Resize it exactly to video size
    cv::resizeWindow(windowName, frameSize.width, frameSize.height);

    // Initialize OpenGL draw callback:
    cv::setOpenGlContext(windowName);
    cv::setOpenGlDrawCallback(windowName, ARDrawingContextDrawCallback, this);
}

ARDrawingContext::~ARDrawingContext()
{
    cv::setOpenGlDrawCallback(m_windowName, 0, 0);
    glBindTexture(GL_TEXTURE_2D, 0); // Set the GL texture to NULL, standard cleanup
}

void ARDrawingContext::updateBackground(const cv::Mat& frame)
{
  frame.copyTo(m_backgroundImage);
}

void ARDrawingContext::updateWindow()
{
    cv::updateWindow(m_windowName);
}

void ARDrawingContext::draw()
{
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // Clear entire screen:
  drawCameraFrame();                                  // Render background
  drawAugmentedScene();                               // Draw AR
  glFlush();
}


void ARDrawingContext::drawCameraFrame()
{
  // Initialize texture for background image
  if (!m_isTextureInitialized)
  {
	  // we generate both textures
    glGenTextures(2, m_backgroundTextureId);
    glBindTexture(GL_TEXTURE_2D, m_backgroundTextureId[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_isTextureInitialized = true;
  }

  int w = m_backgroundImage.cols;
  int h = m_backgroundImage.rows;

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glBindTexture(GL_TEXTURE_2D, m_backgroundTextureId[0]);

  // Upload new texture data:
  if (m_backgroundImage.channels() == 3)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, m_backgroundImage.data);
  else if(m_backgroundImage.channels() == 4)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_backgroundImage.data);
  else if (m_backgroundImage.channels()==1)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_backgroundImage.data);

  const GLfloat bgTextureVertices[] = { 0, 0, w, 0, 0, h, w, h };
  const GLfloat bgTextureCoords[]   = { 1, 0, 1, 1, 0, 0, 0, 1 };
  const GLfloat proj[]              = { 0, -2.f/w, 0, 0, -2.f/h, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1 };

  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(proj);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, m_backgroundTextureId[0]);

  // Update attribute values.
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glVertexPointer(2, GL_FLOAT, 0, bgTextureVertices);
  glTexCoordPointer(2, GL_FLOAT, 0, bgTextureCoords);

  glColor4f(1,1,1,1);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisable(GL_TEXTURE_2D);
}

void ARDrawingContext::drawAugmentedScene()
{
  // Init augmentation projection
  Matrix44 projectionMatrix;
  int w = m_backgroundImage.cols;
  int h = m_backgroundImage.rows;
  buildProjectionMatrix(m_calibration, w, h, projectionMatrix);

  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(projectionMatrix.data);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if (isPatternPresent)
  {
    // Set the pattern transformation
    Matrix44 glMatrix = patternPose.getMat44();
    glLoadMatrixf(reinterpret_cast<const GLfloat*>(&glMatrix.data[0]));

    // Render model
    drawCoordinateAxis();
    //~ drawCubeModel();
    //~ try{
		drawFurnish();
	//~ }
	//~ catch(int e){
		//~ std::cout<< "problem " << e << std::endl;
	//~ }
  }
}

void ARDrawingContext::buildProjectionMatrix(const CameraCalibration& calibration, int screen_width, int screen_height, Matrix44& projectionMatrix)
{
  float nearPlane = 0.01f;  // Near clipping distance
  float farPlane  = 100.0f;  // Far clipping distance

  // Camera parameters
  float f_x = calibration.fx(); // Focal length in x axis
  float f_y = calibration.fy(); // Focal length in y axis (usually the same?)
  float c_x = calibration.cx(); // Camera primary point x
  float c_y = calibration.cy(); // Camera primary point y

  projectionMatrix.data[0] = -2.0f * f_x / screen_width;
  projectionMatrix.data[1] = 0.0f;
  projectionMatrix.data[2] = 0.0f;
  projectionMatrix.data[3] = 0.0f;

  projectionMatrix.data[4] = 0.0f;
  projectionMatrix.data[5] = 2.0f * f_y / screen_height;
  projectionMatrix.data[6] = 0.0f;
  projectionMatrix.data[7] = 0.0f;

  projectionMatrix.data[8] = 2.0f * c_x / screen_width - 1.0f;
  projectionMatrix.data[9] = 2.0f * c_y / screen_height - 1.0f;
  projectionMatrix.data[10] = -( farPlane + nearPlane) / ( farPlane - nearPlane );
  projectionMatrix.data[11] = -1.0f;

  projectionMatrix.data[12] = 0.0f;
  projectionMatrix.data[13] = 0.0f;
  projectionMatrix.data[14] = -2.0f * farPlane * nearPlane / ( farPlane - nearPlane );        
  projectionMatrix.data[15] = 0.0f;
}


void ARDrawingContext::drawCoordinateAxis()
{
  static float lineX[] = {0,0,0,1,0,0};
  static float lineY[] = {0,0,0,0,1,0};
  static float lineZ[] = {0,0,0,0,0,1};

  glLineWidth(2);

  glBegin(GL_LINES);

  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3fv(lineX);
  glVertex3fv(lineX + 3);

  glColor3f(0.0f, 1.0f, 0.0f);
  glVertex3fv(lineY);
  glVertex3fv(lineY + 3);

  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex3fv(lineZ);
  glVertex3fv(lineZ + 3);

  glEnd();
}

void ARDrawingContext::drawCubeModel()
{
  static const GLfloat LightAmbient[]=  { 0.25f, 0.25f, 0.25f, 1.0f };    // Ambient Light Values
  static const GLfloat LightDiffuse[]=  { 0.1f, 0.1f, 0.1f, 1.0f };    // Diffuse Light Values
  static const GLfloat LightPosition[]= { 0.0f, 0.0f, 2.0f, 1.0f };    // Light Position
  
  glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_POLYGON_BIT);

  glColor4f(0.2f,0.35f,0.3f,0.75f);         // Full Brightness, 50% Alpha ( NEW )
  glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);       // Blending Function For Translucency Based On Source Alpha 
  glEnable(GL_BLEND);

  glShadeModel(GL_SMOOTH);

  glEnable(GL_LIGHTING);
  glDisable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
  glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
  glEnable(GL_COLOR_MATERIAL);

  glScalef(0.25,0.25, 0.25);
  glTranslatef(0,0, 1);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBegin(GL_QUADS); 
  // Front Face
  glNormal3f( 0.0f, 0.0f, 1.0f);    // Normal Pointing Towards Viewer
  glVertex3f(-1.0f, -1.0f,  1.0f);  // Point 1 (Front)
  glVertex3f( 1.0f, -1.0f,  1.0f);  // Point 2 (Front)
  glVertex3f( 1.0f,  1.0f,  1.0f);  // Point 3 (Front)
  glVertex3f(-1.0f,  1.0f,  1.0f);  // Point 4 (Front)
  // Back Face
  glNormal3f( 0.0f, 0.0f,-1.0f);    // Normal Pointing Away From Viewer
  glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Back)
  glVertex3f(-1.0f,  1.0f, -1.0f);  // Point 2 (Back)
  glVertex3f( 1.0f,  1.0f, -1.0f);  // Point 3 (Back)
  glVertex3f( 1.0f, -1.0f, -1.0f);  // Point 4 (Back)
  // Top Face
  glNormal3f( 0.0f, 1.0f, 0.0f);    // Normal Pointing Up
  glVertex3f(-1.0f,  1.0f, -1.0f);  // Point 1 (Top)
  glVertex3f(-1.0f,  1.0f,  1.0f);  // Point 2 (Top)
  glVertex3f( 1.0f,  1.0f,  1.0f);  // Point 3 (Top)
  glVertex3f( 1.0f,  1.0f, -1.0f);  // Point 4 (Top)
  // Bottom Face
  glNormal3f( 0.0f,-1.0f, 0.0f);    // Normal Pointing Down
  glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Bottom)
  glVertex3f( 1.0f, -1.0f, -1.0f);  // Point 2 (Bottom)
  glVertex3f( 1.0f, -1.0f,  1.0f);  // Point 3 (Bottom)
  glVertex3f(-1.0f, -1.0f,  1.0f);  // Point 4 (Bottom)
  // Right face
  glNormal3f( 1.0f, 0.0f, 0.0f);    // Normal Pointing Right
  glVertex3f( 1.0f, -1.0f, -1.0f);  // Point 1 (Right)
  glVertex3f( 1.0f,  1.0f, -1.0f);  // Point 2 (Right)
  glVertex3f( 1.0f,  1.0f,  1.0f);  // Point 3 (Right)
  glVertex3f( 1.0f, -1.0f,  1.0f);  // Point 4 (Right)
  // Left Face
  glNormal3f(-1.0f, 0.0f, 0.0f);    // Normal Pointing Left
  glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Left)
  glVertex3f(-1.0f, -1.0f,  1.0f);  // Point 2 (Left)
  glVertex3f(-1.0f,  1.0f,  1.0f);  // Point 3 (Left)
  glVertex3f(-1.0f,  1.0f, -1.0f);  // Point 4 (Left)
  glEnd();
  
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor4f(0.2f,0.65f,0.3f,0.35f); // Full Brightness, 50% Alpha ( NEW )
  glBegin(GL_QUADS);
  // Front Face
  glNormal3f( 0.0f, 0.0f, 1.0f);    // Normal Pointing Towards Viewer
  glVertex3f(-1.0f, -1.0f,  1.0f);  // Point 1 (Front)
  glVertex3f( 1.0f, -1.0f,  1.0f);  // Point 2 (Front)
  glVertex3f( 1.0f,  1.0f,  1.0f);  // Point 3 (Front)
  glVertex3f(-1.0f,  1.0f,  1.0f);  // Point 4 (Front)
  // Back Face
  glNormal3f( 0.0f, 0.0f,-1.0f);    // Normal Pointing Away From Viewer
  glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Back)
  glVertex3f(-1.0f,  1.0f, -1.0f);  // Point 2 (Back)
  glVertex3f( 1.0f,  1.0f, -1.0f);  // Point 3 (Back)
  glVertex3f( 1.0f, -1.0f, -1.0f);  // Point 4 (Back)
  // Top Face
  glNormal3f( 0.0f, 1.0f, 0.0f);    // Normal Pointing Up
  glVertex3f(-1.0f,  1.0f, -1.0f);  // Point 1 (Top)
  glVertex3f(-1.0f,  1.0f,  1.0f);  // Point 2 (Top)
  glVertex3f( 1.0f,  1.0f,  1.0f);  // Point 3 (Top)
  glVertex3f( 1.0f,  1.0f, -1.0f);  // Point 4 (Top)
  // Bottom Face
  glNormal3f( 0.0f,-1.0f, 0.0f);    // Normal Pointing Down
  glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Bottom)
  glVertex3f( 1.0f, -1.0f, -1.0f);  // Point 2 (Bottom)
  glVertex3f( 1.0f, -1.0f,  1.0f);  // Point 3 (Bottom)
  glVertex3f(-1.0f, -1.0f,  1.0f);  // Point 4 (Bottom)
  // Right face
  glNormal3f( 1.0f, 0.0f, 0.0f);    // Normal Pointing Right
  glVertex3f( 1.0f, -1.0f, -1.0f);  // Point 1 (Right)
  glVertex3f( 1.0f,  1.0f, -1.0f);  // Point 2 (Right)
  glVertex3f( 1.0f,  1.0f,  1.0f);  // Point 3 (Right)
  glVertex3f( 1.0f, -1.0f,  1.0f);  // Point 4 (Right)
  // Left Face
  glNormal3f(-1.0f, 0.0f, 0.0f);    // Normal Pointing Left
  glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Left)
  glVertex3f(-1.0f, -1.0f,  1.0f);  // Point 2 (Left)
  glVertex3f(-1.0f,  1.0f,  1.0f);  // Point 3 (Left)
  glVertex3f(-1.0f,  1.0f, -1.0f);  // Point 4 (Left)
  glEnd();
 
  glPopAttrib();
}

void ARDrawingContext::drawFurnish()
{
	float scale = 5.0;
  //~ static const GLfloat LightAmbient[]=  { 0.25f, 0.25f, 0.25f, 1.0f };    // Ambient Light Values
  //~ static const GLfloat LightDiffuse[]=  { 0.1f, 0.1f, 0.1f, 1.0f };    // Diffuse Light Values
  //~ static const GLfloat LightPosition[]= { 0.0f, 0.0f, 2.0f, 1.0f };    // Light Position
  //~ 
  glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_POLYGON_BIT);

  glColor4f(1.0f,1.0f,1.0f,1.0f);         // Full Brightness, 50% Alpha ( NEW )
  //~ glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);       // Blending Function For Translucency Based On Source Alpha 
  //~ glEnable(GL_BLEND);
//~ 
  //~ glShadeModel(GL_SMOOTH);
//~ 
  //~ glEnable(GL_LIGHTING);
  //~ glDisable(GL_LIGHT0);
  //~ glEnable(GL_LIGHT1);
  //~ glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
  //~ glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
  //~ glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
  //~ glEnable(GL_COLOR_MATERIAL);
  
    // Initialize texture for background image
  //~ if (!m_isFurnishTextureInitialized)
  //~ {
		//~ makeCheckImage();
	   //~ glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	   //~ glBindTexture(GL_TEXTURE_2D, m_backgroundTextureId[1]);
//~ 
	   //~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	   //~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	   //~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	   //~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	   //~ glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth,
			//~ checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
		//~ m_isFurnishTextureInitialized = true;
	//~ }
	
	if (!m_isFurnishTextureInitialized)
	{
		//texture image
		m_furnishImage = cv::imread("couch.jpg");
		
		std::cout << "couch channels "<<m_furnishImage.channels() << std::endl;
		glBindTexture(GL_TEXTURE_2D, m_backgroundTextureId[1]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		int w = m_furnishImage.cols;
		int h = m_furnishImage.rows;

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, m_backgroundTextureId[1]);

		// Upload new texture data:
		if (m_furnishImage.channels() == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, m_furnishImage.data);
		else if(m_furnishImage.channels() == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_furnishImage.data);
		else if (m_furnishImage.channels()==1)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_furnishImage.data);

		//~ const GLfloat bgTextureVertices[] = { 0, 0, w, 0, 0, h, w, h };
		//~ const GLfloat bgTextureCoords[]   = { 1, 0, 1, 1, 0, 0, 0, 1 };			
			
		m_isFurnishTextureInitialized = true;
	}

 //~ // Enable texture mapping stuff
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, m_backgroundTextureId[1]);
glEnableClientState(GL_TEXTURE_COORD_ARRAY);
glDisableClientState(GL_COLOR_ARRAY);

//~ glEnableClientState(GL_NORMAL_ARRAY);
	//~ glEnableClientState(GL_COLOR_ARRAY);
	//~ glColorPointer(4, GL_FLOAT, 0, COLORS);
    glEnableClientState(GL_VERTEX_ARRAY);
     //~ glScalef(scale,scale,scale);
     //~ glTranslatef(0, 0, -5.0f);
     
    GLfloat mdl[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mdl);
	getCameraOrigin(mdl,&cameraOrigin);

	getFacesNearToCamera(couchNumVerts,cameraOrigin,couchTexCoords,COLORS,couchVerts,
	outTexCoords,outColors,outVertexes,&finalVertexSize);
	
	// scaling the vertexes withon any help
	scaling(scale, outVertexes, outVertexes,finalVertexSize);
	
	//~ glVertexPointer(3, GL_FLOAT, 0, couchVerts);
    //~ glTexCoordPointer(2, GL_FLOAT, 0, couchTexCoords);
	glVertexPointer(3, GL_FLOAT, 0, outVertexes);
    glTexCoordPointer(2, GL_FLOAT, 0, outTexCoords);
   
	//~ glNormalPointer(GL_FLOAT, 0, couchNormals);
	//~ glTexCoordPointer(2, GL_FLOAT, 0, couchTexCoords);

	//~ glDrawArrays(GL_TRIANGLES, 0, couchNumVerts);
	glDrawArrays(GL_TRIANGLES, 0, finalVertexSize);
	
    //~ glDisableClientState(GL_COLOR_ARRAY);
	//~ glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
  //~ glFlush();
  glPopAttrib();
}
