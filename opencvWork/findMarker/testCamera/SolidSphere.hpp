#ifndef SolidSphere_h
#define SolidSphere_h

#include <iostream>
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GL/gl.h>
#include <vector>

class SolidSphere
{
protected:
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texcoords;
    std::vector<GLushort> indices;

public:
    SolidSphere(float radius, unsigned int rings, unsigned int sectors);
	void draw(GLfloat x, GLfloat y, GLfloat z);
};

#endif
