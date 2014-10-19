#include "Headers.h"
#include "ImageLoader.h"
#include "MapNGrid.h"
#include "Constants.h"

using namespace std;

//Initializes 3D rendering
void initRendering() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
}

void putImageToGrid(GLfloat x, GLfloat y, string path) {
	GLuint _textureId; //The id of the texture

	Image* bgImage = loadBMP(path.c_str());
	_textureId = loadTexture(bgImage);
	delete bgImage;

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

//Draws the 3D scene
void drawScene() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	glLoadIdentity(); //Reset the drawing perspective

	for (GLfloat x = -2.0f; x < 2.0f; x += CELL_LENGTH) {
		for (GLfloat y = -2.0f; y < 2.0f; y += CELL_LENGTH) {
			putImageToGrid(x, y, "data/images/bg.bmp");
		}
	}

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
