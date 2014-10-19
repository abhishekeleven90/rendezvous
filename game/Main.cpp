#include "ImageLoader.h"
#include "MapNGrid.h"
#include "OpenGlHelper.h"
#include "GeneralHelper.h"
#include "Timer.h"

void initRendering();
void putImageToGrid(GLfloat x, GLfloat y, GLuint _textureId);
void drawScene();

//Initializes 3D rendering
void initRendering() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);

	grass_textureId = getTextureFromImage("data/images/bg.bmp");
	tree_textureId = getTextureFromImage("data/images/tree.bmp");
	//TODO - loadAlltextures to a new fxn
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_GRID_COL; c <= END_GRID_COL; c++) {
			putCharToGrid(r, c, GRASS);
		}
	}
	//TODO-temp
	putCharToGrid(10, 10, TREE);
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
}

int main(int argc, char** argv) {
	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(700, 700); //Set the window size

	//Create the window
	glutCreateWindow("Rendezvous!!!");
	initRendering(); //Initialize rendering

	//Set handler functions for drawing, keypresses, and window resizes
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);

	glutTimerFunc(100, timer, 0); //TODO: keep refreshRate in constants
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
	return 0; //This line is never reached
}
