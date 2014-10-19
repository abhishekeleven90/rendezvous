#include "Headers.h"
#include "ImageLoader.h"
#include "MapNGrid.h"
#include "OpenGlHelper.h"
#include "GeneralHelper.h"

using namespace std;

void initRendering();
GLuint getTextureFromImage(string path);
void putImageToGrid(GLfloat x, GLfloat y, GLuint _textureId);
void drawScene();

//Initializes 3D rendering
void initRendering() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
}

GLuint getTextureFromImage(string path) {
	GLuint _textureId; //The id of the texture
	Image *bgImage = loadBMP(path.c_str());
	_textureId = loadTexture(bgImage);
	delete bgImage;
	return _textureId;
}

void putImageToGrid(GLfloat x, GLfloat y, GLuint _textureId) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(x, y, -5.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(x, y + CELL_LENGTH, -5.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(x + CELL_LENGTH, y + CELL_LENGTH, -5.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(x + CELL_LENGTH, y, -5.0f);

	glEnd();
}

void putImageToCell(int row, int col, GLuint _textureId) {
	GLfloat x = getXFromCell(col);
	GLfloat y = getYFromCell(row);
	putImageToGrid(x, y, _textureId);
}

//Draws the 3D scene
void drawScene() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	glLoadIdentity(); //Reset the drawing perspective

	GLuint _textureId = getTextureFromImage("data/images/bg.bmp");
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_GRID_COL; c <= END_GRID_COL; c++) {
			putCharToGrid(r, c, GRASS);
			putImageToCell(r, c, _textureId);
		}
	}

	printGrid();
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

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
	return 0; //This line is never reached
}
