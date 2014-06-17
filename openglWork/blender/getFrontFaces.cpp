#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <algorithm>    // std::sort
#include "rossini.h"
#include "rossiniColors.h"
using namespace std;
	
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

void showPoint3(const point3 *a){
	cout << (*a).x  << "i + "<< (*a).y << "j + " << (*a).z  << "k" << endl;
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

#define EPSILON 0.000001

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


struct face{
	triangle f;
	texture t;
	float d; // distance
};

bool nearCamera(face a, face b){
	return a.d < b.d;
};

float getDistance(point3 a, point3 b){
	return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

void getFacesNearToCamera(unsigned vertexesSize, point3 cameraOrigin,float inTexcoords[], float inColors[][4], float inVertexes[],
								float outTexCoords[], float outColors[][4], float outVertexes[], unsigned *finalVertexes){
	vector<face> faces;
	
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
	
	vector<face>::iterator itFaces;
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


void getFacesToCamera(unsigned vertexesSize, point3 cameraOrigin,float inTexcoords[], float inColors[][4], float inVertexes[],
								float outTexcoords[], float outColors[][4], float outVertexes[], unsigned *finalVertexes){

	unsigned saved = 0;
	int tCounter = 0;
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
			float t,u,v;
			point3 dir = directedVector(&cameraOrigin,&faceCenter);
			
			if (intersectTriangle(&cameraOrigin, &dir, &vertexCoord1, &vertexCoord2, &vertexCoord3,
					&t, &u, &v) == 1) {
			int k =saved;
			outColors[3*k][0] = inColors[3*i][0];
			outColors[3*k][1] = inColors[3*i][1];
			outColors[3*k][2] = inColors[3*i][2];
			outColors[3*k][3] = inColors[3*i][3];
			outColors[3*k + 1][0] = inColors[3*i + 1][0];
			outColors[3*k + 1][1] = inColors[3*i + 1][1];
			outColors[3*k + 1][2] = inColors[3*i + 1][2];
			outColors[3*k + 1][3] = inColors[3*i + 1][3];
			outColors[3*k + 2][0] = inColors[3*i + 2][0];
			outColors[3*k + 2][1] = inColors[3*i + 2][1];
			outColors[3*k + 2][2] = inColors[3*i + 2][2];
			outColors[3*k + 2][3] = inColors[3*i + 2][3];
			
			outVertexes[9*k] = inVertexes[9*i];
			outVertexes[9*k + 1] = inVertexes[9*i + 1];
			outVertexes[9*k + 2] = inVertexes[9*i + 2];
			outVertexes[9*k + 3] = inVertexes[9*i + 3];
			outVertexes[9*k + 4] = inVertexes[9*i + 4];
			outVertexes[9*k + 5] = inVertexes[9*i + 5];
			outVertexes[9*k + 6] = inVertexes[9*i + 6];
			outVertexes[9*k + 7] = inVertexes[9*i + 7];
			outVertexes[9*k + 8] = inVertexes[9*i + 8];
			
			outTexcoords[6*k] = inTexcoords[6*i];
			outTexcoords[6*k + 1] = inTexcoords[6*i + 1];
			outTexcoords[6*k + 2] = inTexcoords[6*i + 2];
			outTexcoords[6*k + 3] = inTexcoords[6*i + 3];
			outTexcoords[6*k + 4] = inTexcoords[6*i + 4];
			outTexcoords[6*k + 5] = inTexcoords[6*i + 5];

			++saved;
			}
			if (t > 0)
				++tCounter;
		}
	}
	cout << "tCounter " << tCounter << endl;
	*finalVertexes = 3*saved;
}

void testFaceToCamera(){
	point3 origin;
	origin.x = 0.0;
	origin.y = 0.0;
	origin.z = 0.0;
	unsigned saved;
	float outNormals[rossiniNumVerts*3];
	float outVertexes[rossiniNumVerts*3];
	float outColors[rossiniNumVerts][4];
	getFacesToCamera(rossiniNumVerts,origin,rossiniNormals,COLORS,rossiniVerts,
	outNormals,outColors,outVertexes,&saved);
	
	cout << saved;
}
void testFacesNearToCamera(){
	point3 origin;
	origin.x = 0.0;
	origin.y = 0.0;
	origin.z = 0.0;
	unsigned saved;
	float outNormals[rossiniNumVerts*3];
	float outVertexes[rossiniNumVerts*3];
	float outColors[rossiniNumVerts][4];
	getFacesNearToCamera(rossiniNumVerts,origin,rossiniNormals,COLORS,rossiniVerts,
	outNormals,outColors,outVertexes,&saved);
	
	cout << saved;
}

void testCrossing(){
	point3 cameraOrigin;
	cameraOrigin.x = 0.0;
	cameraOrigin.y = 0.0;
	cameraOrigin.z = 0.0;
	
		point3 vertexCoord1;
		vertexCoord1.x = 0.4542721196141;
		vertexCoord1.y = 0.126256250092948;
		vertexCoord1.z = 0.10220506292949;
		
		point3 vertexCoord2;
		vertexCoord2.x = 0.448569218042519;
		vertexCoord2.y = 1.177256800535815;
		vertexCoord2.z = 0.173193112135096;
		
		point3 vertexCoord3;
		vertexCoord3.x = 0.449603321479098;
		vertexCoord3.y = 0.140275080997274;
		vertexCoord3.z = 1.181205113868029;	
		
		point3 faceCenter;
		faceCenter.x = (vertexCoord1.x + vertexCoord2.x + vertexCoord3.x)/3.0;
		faceCenter.y = (vertexCoord1.y + vertexCoord2.y + vertexCoord3.y)/3.0;
		faceCenter.z = (vertexCoord1.z + vertexCoord2.z + vertexCoord3.z)/3.0;
	showPoint3(&cameraOrigin);
	showPoint3(&faceCenter);
	float t,u,v;
	point3 dir = directedVector(&cameraOrigin,&faceCenter);
	
	if (intersectTriangle(&cameraOrigin, &dir, &vertexCoord1, &vertexCoord2, &vertexCoord3,
			&t, &u, &v) == 1)
		cout << "crossed" << endl;
	else
		cout << "uncrossed" << endl;
		
		
	cout << "t=" << t << " u=" << u << " v=" << v << endl;
}


int main(){
	//~ testFaceToCamera();
	testFacesNearToCamera();
	//~ testCrossing();
	
	return 0;
}
