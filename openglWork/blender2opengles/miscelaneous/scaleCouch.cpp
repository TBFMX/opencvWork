#include <iostream>
#include <fstream>
#include <cmath>
#include "couchOrig.hpp"

#define PI 3.14159

using namespace std;

int main(){
	
	float outPos[3*couchVertices];
	float outTex[2*couchVertices];
	float maxX, minX, maxY, minY, maxZ, minZ;
	maxX = couchPositions[0];
	minX = couchPositions[0];
	maxY = couchPositions[1];
	minY = couchPositions[1];
	maxZ = couchPositions[2];
	minZ = couchPositions[2];
	
	unsigned currentIndex;
	// finding the max and min for x, y and z
	for(int i = 0; i < couchVertices; ++i){
		currentIndex = 3*i;
		if(maxX < couchPositions[currentIndex])
			maxX =  couchPositions[currentIndex];
		if(minX > couchPositions[currentIndex])
			minX =  couchPositions[currentIndex];
		if(maxY < couchPositions[currentIndex + 1])
			maxY =  couchPositions[currentIndex + 1];
		if(minY > couchPositions[currentIndex + 1])
			minY =  couchPositions[currentIndex + 1];
		if(maxZ < couchPositions[currentIndex + 2])
			maxZ =  couchPositions[currentIndex + 2];
		if(minZ > couchPositions[currentIndex + 2])
			minZ =  couchPositions[currentIndex + 2];
	}
	
	cout << "minX " << minX << ", maxX " << maxX<< endl;
	cout << "minY " << minY << ", maxY " << maxY<< endl;
	cout << "minZ " << minZ << ", maxZ " << maxZ<< endl;
		
	float divX = maxX - minX;
	float divY = maxY - minY;
	float divZ = maxZ - minZ;
	float maxDiv = divX;
	if(maxDiv < divY)
		maxDiv = maxY;
	if(maxDiv < divZ)
		maxDiv = maxZ;
	cout << "maxDiv " << maxDiv <<endl;
	// all coordinates are divided by the same value in order to mantain the proportions
	for(int i = 0; i < 3*couchVertices; ++i){
		outPos[i] = couchPositions[i] / maxDiv;
	}
	
		
	// Translation over Y to fix the minimun Y coordinate to zero
	minY = outPos[1];
	
	// we calculate the minimum Y for all vertexes
	for(int i = 0; i < couchVertices; ++i){
		currentIndex = 3*i;
		if(minY > outPos[currentIndex + 1])
			minY =  outPos[currentIndex + 1];		
	}
	
	// tranlating the model to Y coordinate zero
	for(int i = 0; i < couchVertices; ++i){
		currentIndex = 3*i;
		outPos[currentIndex + 1] = outPos[currentIndex + 1] - minY;
	}
	
	// rotation over Y axis
	float angle = 45*PI/180;
	for(int i = 0; i < couchVertices; ++i){
		currentIndex = 3*i;
		// saving z
		float currentX = outPos[currentIndex];
		float currentZ = outPos[currentIndex + 2];
		// calculating x
		outPos[currentIndex] = currentX * cos(angle) + currentZ * sin(angle);
		// calculating z
		outPos[currentIndex + 2] = -currentX * sin(angle) + currentZ * cos(angle);
	}
	
	// rotation over X axis
	angle = 90*PI/180;
	for(int i = 0; i < couchVertices; ++i){
		currentIndex = 3*i;
		// saving z
		float currentY = outPos[currentIndex + 1];
		float currentZ = outPos[currentIndex + 2];
		// calculating y
		outPos[currentIndex + 1] = currentY * cos(angle) - currentZ * sin(angle);
		// calculating z
		outPos[currentIndex + 2] = currentY * sin(angle) + currentZ * cos(angle);
	}
	
	// saving the current file
	ofstream couchOut ("couch.hpp");
	couchOut << "const float couchPositions[4788] = " << endl << "{" << endl;
	for(int i = 0; i < couchVertices; ++i){
		currentIndex = 3*i;
		couchOut <<  outPos[currentIndex] << "," << outPos[currentIndex + 1] << ", " << outPos[currentIndex + 2] << ","<< endl; 
	}
	
	couchOut << "};" << endl << endl;
	
	
	couchOut.close();
	//~ scaling
	//~ couchPositions
	//~ couchTexels
	//~ couchVertices;
}
