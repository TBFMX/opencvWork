#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>

// include generated arrays
#import "./banana.h"

using namespace std;

int main(){
	// set input data to arrays
	glVertexPointer(3, GL_FLOAT, 0, bananaVerts);
	glNormalPointer(GL_FLOAT, 0, bananaNormals);
	glTexCoordPointer(2, GL_FLOAT, 0, bananaTexCoords);

	// draw data
	glDrawArrays(GL_TRIANGLES, 0, bananaNumVerts);
	return 0;
}
