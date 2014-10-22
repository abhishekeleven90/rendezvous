#include "Headers.h"
#include "MapNGrid.h"
#include "GeneralHelper.h"
#include "OpenGlHelper.h"
#include "Constants.h"
#include "Objects.h"
#include "SoundAll.h"
#include "Timer.h"

void initRendering();
void putImageToGrid(GLfloat x, GLfloat y, GLuint _textureId);
void drawScene();

//Initializes 3D rendering
void initRendering() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);

	//Loading all the textures from images to be used later
	loadTextures();
	initMap();

	for (int i = 0; i < ITEM_MAP_COUNT; i++) {
		placeItemAtRandomPos();
	}
	putCharToGrid(19, 1, H_SLOWER, true);
}

//Draws the 3D scene
void drawScene() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	glLoadIdentity(); //Reset the drawing perspective

	//printGrid();
	renderGrid();
	glutSwapBuffers(); //Send the 3D scene to the screen
	//cout << "inside draw scene" << endl;
}

int main(int argc, char** argv) {
	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(850, 700); //Set the window size

	//Create the window
	glutCreateWindow("Rendezvous!!!");
	initRendering(); //Initialize rendering

	//Set handler functions
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler);

	glutTimerFunc(REFRESH_RATE, timer, 0);
	playBgSound();
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
	return 0; //This line is never reached
}
