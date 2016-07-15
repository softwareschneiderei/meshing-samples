
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <gl/GLU.h>

#define X .525731112119133606f
#define Z .850650808352039932f

static GLfloat vdata[12][3]={

	{-X, 0.0f, Z},{X, 0.0f, Z},{-X, 0.0f, -Z},{X, 0.0f, -Z},

	{0.0f, Z, X},{0.0f, Z, -X},{0.0f, -Z, X},{0.0f, -Z, -X},

	{Z, X, 0.0f},{-Z, X, 0.0f},{Z, -X, 0.0f},{-Z, -X, 0.0f}

};

static GLuint tindices[20][3]={

	{0,4,1},{0,9,4},{9,5,4},{4,5,8},{4,8,1},

	{8,10,1},{8,3,10},{5,3,8},{5,2,3},{2,7,3},

	{7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},

	{6,1,10},{9,0,11},{9,11,2},{9,2,5},{7,2,11}};


void Display_InitGL()
{
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glCullFace(GL_BACK);
	//glEnable(GL_CULL_FACE);
}
/* function to reset our viewport after a window resize */
int Display_SetViewport(int width, int height)
{
	if (height==0) {
		height=1;
	}

	auto ratio=(GLfloat)width/(GLfloat)height;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return 1;
}

void Render(float angle)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0.f, 0.0f, -6.0f);
	glRotatef(angle, 0.f, 1.f, 0.f);

	glBegin(GL_TRIANGLES);

	for (int i=0; i<20; i++)
	{
		glVertex3fv(&vdata[tindices[i][0]][0]);
		glVertex3fv(&vdata[tindices[i][1]][0]);
		glVertex3fv(&vdata[tindices[i][2]][0]);
	}

	glEnd();
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
	)
{
	// Initialize SDL's Video subsystem
	if (SDL_Init(SDL_INIT_VIDEO)<0)
	{
		return -1;
	}

	// Create our window centered at 512x512 resolution

	auto window=SDL_CreateWindow(
		"My Window",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		800,
		600,
		SDL_WINDOW_OPENGL
		);

	auto context=SDL_GL_CreateContext(window);

	Display_InitGL();

	Display_SetViewport(800, 600);

	bool quit=false;
	float angle=0.f;
	while (!quit) {
		SDL_Event e;
		while (SDL_PollEvent(&e)!=0)
		{
			if (e.type==SDL_QUIT) { quit=true; }
		}

		Render(angle);
		SDL_GL_SwapWindow(window);
		angle+=0.1f;

	}
	SDL_Quit();

	return 0;
}
