#ifndef geometryStructs_h
#define geometryStructs_h

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

#include "geometryStructs.cpp"

#endif
