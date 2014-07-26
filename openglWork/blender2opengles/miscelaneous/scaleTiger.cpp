#include <iostream>
#include <fstream>
#include <cmath>
#include "TigerOrig.hpp"

#define PI 3.14159

using namespace std;

int main(){
	
	float outPos[3*TigerVertices];
	float outTex[2*TigerVertices];
	float maxX, minX, maxY, minY, maxZ, minZ;
	maxX = TigerPositions[0];
	minX = TigerPositions[0];
	maxY = TigerPositions[1];
	minY = TigerPositions[1];
	maxZ = TigerPositions[2];
	minZ = TigerPositions[2];
	
	unsigned currentIndex;
	// finding the max and min for x, y and z
	for(int i = 0; i < TigerVertices; ++i){
		currentIndex = 3*i;
		if(maxX < TigerPositions[currentIndex])
			maxX =  TigerPositions[currentIndex];
		if(minX > TigerPositions[currentIndex])
			minX =  TigerPositions[currentIndex];
		if(maxY < TigerPositions[currentIndex + 1])
			maxY =  TigerPositions[currentIndex + 1];
		if(minY > TigerPositions[currentIndex + 1])
			minY =  TigerPositions[currentIndex + 1];
		if(maxZ < TigerPositions[currentIndex + 2])
			maxZ =  TigerPositions[currentIndex + 2];
		if(minZ > TigerPositions[currentIndex + 2])
			minZ =  TigerPositions[currentIndex + 2];
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
	for(int i = 0; i < 3*TigerVertices; ++i){
		outPos[i] = TigerPositions[i] / maxDiv;
	}
	
		
	// Translation over Y to fix the minimun Y coordinate to zero
	minY = outPos[1];
	
	// we calculate the minimum Y for all vertexes
	for(int i = 0; i < TigerVertices; ++i){
		currentIndex = 3*i;
		if(minY > outPos[currentIndex + 1])
			minY =  outPos[currentIndex + 1];		
	}
	
	// tranlating the model to Y coordinate zero
	for(int i = 0; i < TigerVertices; ++i){
		currentIndex = 3*i;
		outPos[currentIndex + 1] = outPos[currentIndex + 1] - minY;
	}
	
	// rotation over Y axis
	float angle = 45*PI/180;
	for(int i = 0; i < TigerVertices; ++i){
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
	for(int i = 0; i < TigerVertices; ++i){
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
	ofstream tigerOut ("Tiger.hpp");
	tigerOut << "float TigerPositions[336762] = " << endl << "{" << endl;
	for(int i = 0; i < TigerVertices; ++i){
		currentIndex = 3*i;
		tigerOut <<  outPos[currentIndex] << "," << outPos[currentIndex + 1] << ", " << outPos[currentIndex + 2] << ","<< endl; 
	}
	
	tigerOut << "};" << endl << endl;
	
	
	tigerOut.close();
	//~ scaling
	//~ TigerPositions
	//~ TigerTexels
	//~ TigerVertices;
}
