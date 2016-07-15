
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <gl/GLU.h>


void Display_InitGL()
{
	/* Enable smooth shading */
	glShadeModel(GL_SMOOTH);

	/* Set the background black */
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	/* Depth buffer setup */
	glClearDepth(1.0f);

	/* Enables Depth Testing */
	glEnable(GL_DEPTH_TEST);

	/* The Type Of Depth Test To Do */
	glDepthFunc(GL_LEQUAL);

	/* Really Nice Perspective Calculations */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}
/* function to reset our viewport after a window resize */
int Display_SetViewport(int width, int height)
{
	/* Height / width ration */
	GLfloat ratio;

	/* Protect against a divide by zero */
	if (height==0) {
		height=1;
	}

	ratio=(GLfloat)width/(GLfloat)height;

	/* Setup our viewport. */
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	/* change to the projection matrix and set our viewing volume. */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	/* Set our perspective */
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);

	/* Make sure we're chaning the model view and not the projection */
	glMatrixMode(GL_MODELVIEW);

	/* Reset The View */
	glLoadIdentity();

	return 1;
}

void Display_Render()
{
	/* Set the background black */
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	/* Clear The Screen And The Depth Buffer */
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	/* Move Left 1.5 Units And Into The Screen 6.0 */
	glLoadIdentity();
	glTranslatef(-1.5f, 0.0f, -6.0f);

	glBegin(GL_TRIANGLES);            /* Drawing Using Triangles */
	glVertex3f(0.0f, 1.0f, 0.0f); /* Top */
	glVertex3f(-1.0f, -1.0f, 0.0f); /* Bottom Left */
	glVertex3f(1.0f, -1.0f, 0.0f); /* Bottom Right */
	glEnd();                           /* Finished Drawing The Triangle */

									   /* Move Right 3 Units */
	glTranslatef(3.0f, 0.0f, 0.0f);

	glBegin(GL_QUADS);                /* Draw A Quad */
	glVertex3f(-1.0f, 1.0f, 0.0f); /* Top Left */
	glVertex3f(1.0f, 1.0f, 0.0f); /* Top Right */
	glVertex3f(1.0f, -1.0f, 0.0f); /* Bottom Right */
	glVertex3f(-1.0f, -1.0f, 0.0f); /* Bottom Left */
	glEnd();                           /* Done Drawing The Quad */
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
	)
{
	// Initialize SDL's Video subsystem
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
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
	while( !quit ) {
		SDL_Event e;
		while( SDL_PollEvent( &e ) != 0 )
		{
			if( e.type == SDL_QUIT ) { quit = true; }
		}

		Display_Render();
		SDL_GL_SwapWindow(window);
	}
	SDL_Quit();

	return 0;
}
