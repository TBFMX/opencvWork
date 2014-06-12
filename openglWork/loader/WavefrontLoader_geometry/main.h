#ifndef _MAIN_H_
#define _MAIN_H_

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
using namespace std;

#include "wavefrontLoader.h"

float angle = 0;

// SDL timing
long oldTime = 0;
long newTime = 0;

// OpenGL variables
unsigned int g_persp = 0, g_ortho = 0;

// SDL stuff
SDL_Surface *screen;
int screenWidth = 800, screenHeight = 600, screenDepth;
bool screenFullScreen;
int done = 0;

// FPS independance
double fpsCurrentTime;

// Frames/second variables
int FPS = 0;
int pastFPS = 0;
int past = 0; 
char fpsString[20];

// Load models
WFObject model;

// Functions
void draw();

#endif
