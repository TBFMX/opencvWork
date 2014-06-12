#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "rossini.h"
using namespace std;


struct point3{
	double x;
	double y;
	double z;
};

struct triangle{
	point3 p1;
	point3 p2;
	point3 p3;
};


// calculates internal product


point3 vectorProduct(const point3 *a, const point3 *b){
	point3 c;
	c.x = (*a).y * (*b).z - (*a).z * (*b).y;
	c.y = -(*a).x * (*b).z + (*a).z * (*b).x;
	c.z = (*a).x * (*b).y - (*a).y * (*b).x;
	return c;
}

void showPoint3(const point3 *a){
	cout << (*a).x  << "i + "<< (*a).y << "j + " << (*a).z  << "k" << endl;
}

double internalProduct(const point3 *a, const point3 *b){
	return (*a).x * (*b).x + (*a).y * (*b).y + (*a).z * (*b).z;
}

void swapPoints(point3 *a, point3 *b){
	point3 c;
	c.x = (*a).x;
	c.y = (*a).y;
	c.z = (*a).z;
	(*a).x = (*b).x;
	(*a).y = (*b).y;
	(*a).z = (*b).z;
	(*b).x = c.x;
	(*b).y = c.y;
	(*b).z = c.z;
}

void testVectorProduct(){
	point3 a, b, c;
	a.x = 0;
	a.y = 0;
	a.z = 1;
	b.x = 0;
	b.y = 0;
	b.z = 1;
	
	c = vectorProduct(&a,&b);
	showPoint3(&c);
	cout << "internal " << internalProduct(&a,&b) << endl;	
}

// vector comes from a and goes to b
point3 directedVector(point3 *a, point3 * b){
	
	point3 a2b;
	a2b.x = (*b).x - (*a).x;
	a2b.y = (*b).y - (*a).y;
	a2b.z = (*b).z - (*a).z;
	
	return a2b;
}

void saveVertexes(vector<triangle> *triangles, string name){
	char *outputName= &name[0];
	vector<triangle>::iterator itTriangles;
	ofstream outputFile(outputName);
	for(itTriangles = (*triangles).begin(); itTriangles < (*triangles).end() ; ++itTriangles){
		outputFile << endl;
		outputFile << (*itTriangles).p1.x << ',';
		outputFile << (*itTriangles).p1.y << ',';
		outputFile << (*itTriangles).p1.z << ',' << endl;
		outputFile << (*itTriangles).p2.x << ',';
		outputFile << (*itTriangles).p2.y << ',';
		outputFile << (*itTriangles).p2.z << ',' << endl;
		outputFile << (*itTriangles).p3.x << ',';
		outputFile << (*itTriangles).p3.y << ',';
		outputFile << (*itTriangles).p3.z << ',' << endl;
	}
}

void calculatPerl(){
	vector<point3>rossiniVertexes(rossiniNumVerts);
	unsigned int numTriangles = rossiniNumVerts/3;
	vector<triangle>rossiniTriangles(numTriangles);
	vector<point3>rossiniTriangleCenters(numTriangles);
	// getting the points for each vertex
	for(unsigned int i = 0; i < 3*rossiniNumVerts; ++i){
		if (i % 3 == 0)
			rossiniVertexes[i/3].x = rossiniVerts[i];
		else if (i % 3 == 1)
			rossiniVertexes[i/3].y = rossiniVerts[i];
		else if (i % 3 == 2)
			rossiniVertexes[i/3].z = rossiniVerts[i];
	}
	
	// getting ordered vertexes for each triangle
	for(unsigned int i = 0; i < rossiniNumVerts; ++i){
		if ( i % 3 == 0)
			rossiniTriangles[i/3].p1 = rossiniVertexes[i];
		else if (i%3 == 1)
			rossiniTriangles[i/3].p2 = rossiniVertexes[i];
		else if (i % 3 == 2)
			rossiniTriangles[i/3].p3 = rossiniVertexes[i];
	}
	
	// calculate triangle centers
	for (unsigned i = 0; i < numTriangles; ++i){
		point3 triangleC;
		triangleC.x = (rossiniTriangles[i].p1.x + rossiniTriangles[i].p2.x + rossiniTriangles[i].p3.x)/3.0;
		triangleC.y = (rossiniTriangles[i].p1.y + rossiniTriangles[i].p2.y + rossiniTriangles[i].p3.y)/3.0;
		triangleC.z = (rossiniTriangles[i].p1.z + rossiniTriangles[i].p2.z + rossiniTriangles[i].p3.z)/3.0;
		rossiniTriangleCenters[i] = triangleC;
	}
	
	// calcule the gravity center
	point3 gravity;
	double gravityX = 0.0;
	double gravityY = 0.0;
	double gravityZ = 0.0;	
	for (unsigned i = 0; i < numTriangles; ++i){
		gravityX += rossiniTriangleCenters[i].x;
		gravityY += rossiniTriangleCenters[i].y;
		gravityZ += rossiniTriangleCenters[i].z;
	}
	
	gravity.x = gravityX / numTriangles; 
	gravity.y = gravityY / numTriangles; 
	gravity.z = gravityZ / numTriangles;
	
	showPoint3(&gravity);
	// calculate normal vector for each triangle
	for(unsigned i  = 0; i < numTriangles; ++i){
		// this vector comes from p2 and goes to p1
		point3 vT1 = directedVector(&rossiniTriangles[i].p2,&rossiniTriangles[i].p1);
		// this vector comes from p2 and goes to p3
		point3 vT2 = directedVector(&rossiniTriangles[i].p2,&rossiniTriangles[i].p3);
		// we realize the vector product according to hand right rule
		point3 normalOutFace = vectorProduct(&vT2,&vT1);
		
		point3 triangleC = rossiniTriangleCenters[i];

		// vector from gravity center to triangle center;
		point3 gC2TC = directedVector(&gravity,&triangleC);
		
		// calcule internal product between gC2TC and normalOutFace
		// if internal product lower than zero swap point 1 and point 2 in triangles
		if (internalProduct(&gC2TC,&normalOutFace) < 0){
			swapPoints(&rossiniTriangles[i].p1, &rossiniTriangles[i].p2);
		}
	}
	
	saveVertexes(&rossiniTriangles,"rossini3.h");
}

int main(){
	calculatPerl();
	return 0;
}
