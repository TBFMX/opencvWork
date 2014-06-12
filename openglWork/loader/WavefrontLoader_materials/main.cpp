#include "main.h"

int createWindow(int width, int height, int depth, bool fullScreen, const string& title)
{
	bool madeWindow = false;
	
	screenWidth = width;
	screenHeight = height;
	screenDepth = depth;
	screenFullScreen = fullScreen;
	
	if(!madeWindow)
	{
		if(fullScreen)
		{
			screen = SDL_SetVideoMode(width, height, depth, SDL_OPENGL | SDL_RESIZABLE | SDL_FULLSCREEN);
		}
		else if(!fullScreen)
		{
			screen = SDL_SetVideoMode(width, height, depth, SDL_OPENGL | SDL_RESIZABLE);
		}
	
		if(!screen) 
		{
			cout << "ERROR: Could not set " << width << "x" << height << " video mode: " << SDL_GetError() << endl;
			SDL_Quit();
			exit(2);
		}
		
		SDL_WM_SetCaption(title.c_str(), title.c_str());
	
		madeWindow = true;
		
		return 0;
	}
	else
	{
		cout << "ERROR: There is already a window open. Check your code for more than one window creation\n";
		return 1;
	}
}

void reshape(int w, int h)
{
	if(h == 0) 
	{
		h = 1;
	}

	// Keep global variables up to date
	screenWidth = w;
	screenHeight = h;

	// delete the last display lists
	glDeleteLists(g_persp, 1);
	glDeleteLists(g_ortho, 1);

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// generate new display list ID's
	g_persp = glGenLists(1);
	g_ortho = glGenLists(1);

	// generate perspective display list
	glNewList(g_persp, GL_COMPILE);

		// set up the projection matrix 
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// just use a perspective projection
		gluPerspective(45, (float)w / h, 0.1, 100);

		// go back to modelview matrix so we can move the objects about
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	glEndList();


	// generate orthographic display list
	glNewList(g_ortho, GL_COMPILE);

		// set up the projection matrix 
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// just use a orthographic projection
		glOrtho(0, w, h, 0, -100, 100);

		// go back to modelview matrix so we can move the objects about
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	glEndList();
}

void idle()
{
	
}

void simulationUpdateFunctions()
{
	//~ angle += 1;
	angle += 1;
}

void updateSimulation()
{
	// FPS counter
	int fpsCurrentTime = SDL_GetTicks();

	if(fpsCurrentTime - past >= 16)
	{
		past = SDL_GetTicks(); // this should be done before redrawing screen
		
		// SDL_Flip( screen );
		simulationUpdateFunctions();
		draw();
		SDL_GL_SwapBuffers();
		FPS++;
	}

	if(fpsCurrentTime - pastFPS >= 1000)
	{
		static char buffer[20] = {0};
		sprintf(fpsString, "%d FPS", FPS);
		
		FPS = 0;
		pastFPS = fpsCurrentTime;
	}
}

void draw()
{
	glPushMatrix();
	
	// clear the screen & depth buffer
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// set the perspective projection
	glCallList(g_persp);

	// set the camera position
	gluLookAt(	0, 5, 10,	// 	eye pos
				0, 0, 0,	// 	aim point
				0, 1, 0);	// 	up direction

	// 3D stuff	*****************************************
	glRotatef(angle, 0.0, 1.0, 0.0);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	
	model.draw();

	// set the orthographic projection
	glCallList(g_ortho);

	// 2D/text	*****************************************
	
	
	glPopMatrix();

	// Commented out because we call it in our idle() function - maintains framerate independance
	// SDL_GL_SwapBuffers();
}

int init()
{
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		cout << "SDL error: " << SDL_GetError() << endl;
		return false;
	}
	
	// SDL Window crap
	createWindow(screenWidth, screenHeight, 32, false, "WaveFront Object Loader");
	reshape(screenWidth, screenHeight);
	
	// OpenGL init
	// Stuff
	glShadeModel(GL_SMOOTH);
	//~ glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	
	// Face culling (for textures)
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); 
	glEnable(GL_DEPTH_TEST);
	
	// Lighting
	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
	
	//~ char pathToObj[] = "./models/MultiTextures.obj";
	char pathToObj[] = "./models/rossini.obj";
	// Object loader
	if(!model.load(pathToObj))
	{
		cout << "Could not load model" << endl;
		return false;
	}
	
	return true;
}

void testKeys(Uint8 *keys)
{
	if(keys[SDLK_ESCAPE])
	{
		done = 1;
	}
}

int main(int argc, char *argv[])
{
	if(!init())
	{
		cout << "Bad init" << endl;
		done = 1;
		SDL_Quit();
	}
	
	while(!done)
	{
		SDL_Event event;
	
		while(SDL_PollEvent(&event) && !done) 
		{
			switch(event.type) 
			{
				case SDL_QUIT:
					done = 1;
				break;
				
				case SDL_VIDEORESIZE:
					screen = SDL_SetVideoMode(event.resize.w, event.resize.h, screenDepth, SDL_OPENGL | SDL_RESIZABLE);
					
					if(screen) 
					{
						reshape(screen->w, screen->h);
					}
					else 
					{
						cout << " ERROR: Could not set video mode: " << SDL_GetError() << endl;
					}
			}
				break;
		}
		
		testKeys(SDL_GetKeyState(NULL));

		updateSimulation();
		
		idle();
	}
	
	SDL_Quit();
	return 0;
}
