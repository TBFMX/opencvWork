

#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GL/glew.h>
#include <cstdlib>
#include <cmath>
#include <algorithm>    // std::sort
#include <vector>
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <fstream>
#include <sstream>
//~ #include "logger.h"
#include "geometryStructs.hpp"

// Utility for logging:
//~ #define LOG_TAG    "CAMERA_RENDERER"
//~ #define LOG(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

float outNormals[maxObjSize*3];
float outTexCoords[maxObjSize*2];
float outVertexes[maxObjSize*3];
float outColors[maxObjSize][4];
float _angle = 0.0;
point3 cameraOrigin;
unsigned finalVertexSize;

int gWidth = 1024;
int gHeight = 768;
cv::Mat rgbFrame;
float deltaZcamera = 0.5;

//~ bool isFaceDrawn[87360];
cv::Mat m_furnishImage;
bool m_isTextureInitialized = false;
bool m_isFurnishTextureInitialized = false;
static GLuint texName;

//~ 
//~ void applyOrtho(float left, float right,float bottom, float top,float near, float far) const{
	//~ float a = 2.0f / (right - left);
	//~ float b = 2.0f / (top - bottom);
	//~ float c = -2.0f / (far - near);
//~ 
	//~ float tx = - (right + left)/(right - left);
	//~ float ty = - (top + bottom)/(top - bottom);
	//~ float tz = - (far + near)/(far - near);
//~ 
	//~ float ortho[16] = {
		//~ a, 0, 0, 0,
		//~ 0, b, 0, 0,
		//~ 0, 0, c, 0,
		//~ tx, ty, tz, 1
	//~ };
//~ 
//~ 
	//~ GLint projectionUniform = glGetUniformLocation(_shaderProgram, "Projection");
	//~ glUniformMatrix4fv(projectionUniform, 1, 0, &ortho[0]);
//~ }

void orthogonalStart()
{
	int w1 = gWidth;
	int h1 = gHeight;
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

void perspectiveGL( float fovY, float aspect, float zNear, float zFar )
{
	//~ float aspect = screenWidth / screenHeight;
	float bt = (float) tan(45 / 2);
	float lr = bt * aspect;
	glFrustum(-lr * 0.1f, lr * 0.1f, -bt * 0.1f, bt * 0.1f, 0.1f, 100.0f);
	
    //~ const float pi = 3.1415926535897932384626433832795;
    //~ float fW, fH;

    //fH = tan( (fovY / 2) / 180 * pi ) * zNear;
    //~ fH = (float) tan( fovY / 360 * pi ) * zNear;
    //~ fH = (float) tan( fovY / 2 ) * zNear;
    //~ fW = fH * aspect *zNear;
//~ 
    //~ glFrustumf( -fW, fW, -fH, fH, zNear, zFar );
}

void startTexture(GLuint texName){
	// Enable texture mapping stuff
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName);
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

// for ordering from the farthest to nearest
bool nearCamera(face a, face b){
	return a.d > b.d;
};

float getDistance(point3 a, point3 b){
	return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}


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
	
	std::sort(faces.begin(),faces.end(),nearCamera);
	
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

void getAllSortedFaces(unsigned vertexesSize, point3 cameraOrigin,float inTexcoords[], float inVertexes[],
						float inNormals[], float outTexCoords[], float outVertexes[], float outNormals[], unsigned *finalVertexes){
	std::vector<face> faces;
	
	unsigned saved = 0;

	// we divide vertexes size by 3 because each face has 3 vertexes
	for(unsigned i  = 0; i < vertexesSize/3; ++i){
		
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
		
		point3 faceCenter;
		faceCenter.x = (vertexCoord1.x + vertexCoord2.x + vertexCoord3.x)/3.0;
		faceCenter.y = (vertexCoord1.y + vertexCoord2.y + vertexCoord3.y)/3.0;
		faceCenter.z = (vertexCoord1.z + vertexCoord2.z + vertexCoord3.z)/3.0;		

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
		
		currFace.n.p1.x = inNormals[9*i];
		currFace.n.p1.y = inNormals[9*i + 1];
		currFace.n.p1.z = inNormals[9*i + 2];
		currFace.n.p2.x = inNormals[9*i + 3];
		currFace.n.p2.y = inNormals[9*i + 4];
		currFace.n.p2.z = inNormals[9*i + 5];
		currFace.n.p3.x = inNormals[9*i + 6];
		currFace.n.p3.y = inNormals[9*i + 7];
		currFace.n.p3.z = inNormals[9*i + 8];
		
		currFace.t.p1.x = inTexcoords[6*i];
		currFace.t.p1.y  = inTexcoords[6*i + 1];
		currFace.t.p1.z  = inTexcoords[6*i + 2];
		currFace.t.p2.x  = inTexcoords[6*i + 3];
		currFace.t.p2.y  = inTexcoords[6*i + 4];
		currFace.t.p2.z  = inTexcoords[6*i + 5];

		faces.push_back(currFace);
		++saved;
	}
	
	std::sort(faces.begin(),faces.end(),nearCamera);
	
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
		
		outNormals[9*k]=(*itFaces).n.p1.x;
		outNormals[9*k + 1]=(*itFaces).n.p1.y;
		outNormals[9*k + 2]=(*itFaces).n.p1.z;
		outNormals[9*k + 3]=(*itFaces).n.p2.x;
		outNormals[9*k + 4]=(*itFaces).n.p2.y;
		outNormals[9*k + 5]=(*itFaces).n.p2.z;
		outNormals[9*k + 6]=(*itFaces).n.p3.x;
		outNormals[9*k + 7]=(*itFaces).n.p3.y;
		outNormals[9*k + 8]=(*itFaces).n.p3.z;
		
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

void getBackgroundTextures(int texName, int bufferIndex, cv::Mat captureBuffer[],unsigned capFrameWidth,unsigned capFrameHeight) {
	int w = capFrameWidth;
	int h = capFrameHeight;

	if(bufferIndex > 0){
		//~ pthread_mutex_lock(&FGmutex);
		cvtColor(captureBuffer[(bufferIndex - 1) % 30], rgbFrame, CV_BGR2RGB);
		//~ pthread_mutex_unlock(&FGmutex);
		w=rgbFrame.cols;
		h=rgbFrame.rows;
		//LOG_INFO("w,h, channels= %d,%d, %d, %d ",w,h,rgbFrame.channels(), textureId[0]);
		
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, texName);

		if (texName != 0){
			// Upload new texture data:
		if (rgbFrame.channels() == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbFrame.data);
		else if(rgbFrame.channels() == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbFrame.data);
		else if (rgbFrame.channels()==1)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, rgbFrame.data);
		}
		
		if (bufferIndex==30)
			bufferIndex = 0;
	}
}

void getFurnishTexture(unsigned int texName){
	if (!m_isFurnishTextureInitialized)
	{
		//texture image
		cv::Mat m_furnishImage = cv::imread("sdcard/Models/couch.jpg");
		cvtColor(m_furnishImage,m_furnishImage,CV_BGR2RGB);
		
		//~ //glGenTextures(1, &texName);
		glBindTexture(GL_TEXTURE_2D, texName);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);// GL_LINEAR
		int w = m_furnishImage.cols;
		int h = m_furnishImage.rows;

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, texName);

		// Upload new texture data:
		if (m_furnishImage.channels() == 3)
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, m_furnishImage.data);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, m_furnishImage.data);
		else if(m_furnishImage.channels() == 4)
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_furnishImage.data);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_furnishImage.data);
		else if (m_furnishImage.channels()==1)
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_furnishImage.data);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_furnishImage.data);		

		m_isFurnishTextureInitialized = true;
	}	
}

void getObjectTexture(unsigned int texName, const cv::Mat& image){
	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);// GL_LINEAR
	int w = image.cols;
	int h = image.rows;

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, texName);

	// Upload new texture data:
	if (image.channels() == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
	else if(m_furnishImage.channels() == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
	else if (image.channels()==1)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, image.data);		
}

