#ifndef OPEN_GL_HELPER_H
#define OPEN_GL_HELPER_H

#include "ImageLoader.h"
#include "Constants.h"
#include "CustomVectorStruct.h"
#include "SoundAll.h"
#include "ActionOnGrid.h"
#include "Objects.h"
#include "Headers.h"

#include "text3d.h"

float t3dComputeScale(const char* str) {
	float width = t3dDrawWidth(str);
	return CELL_LENGTH / width;
}

/*void setOrthographicProjection() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, windowWidth, 0, windowHeight);
	glScalef(1, -1, 1);
	glTranslatef(0, -windowHeight, 0);
	glMatrixMode(GL_MODELVIEW);
}

void resetPerspectiveProjection() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

Coordinate_grid getGridCoordinatesFromWindow(Coordinate_window window) {
	int col = ceil(window.x / cellWidth);
	int row = ceil(window.y / cellHeight);
	return Coordinate_grid(row, col);
}

Coordinate_window getWindowCoordinatesFromGrid(Coordinate_grid grid) {
	float x = (grid.col - 1) * cellWidth;
	float y = (grid.row) * cellHeight;
	return Coordinate_window(x, y);
}

void renderBitmapString(Coordinate_grid grid, const char *string) {
	Coordinate_window window = getWindowCoordinatesFromGrid(grid);
	setOrthographicProjection();
	glPushMatrix();
	glLoadIdentity();

	const char *c;
	glRasterPos2f(window.x, window.y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
	}

	glPopMatrix();
	resetPerspectiveProjection();
}*/

//Makes the image into a texture, and returns the id of the texture
GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D, //Always GL_TEXTURE_2D
			0, //0 for now
			GL_RGB, //Format OpenGL uses for image
			image->width, image->height, //Width and height
			0, //The border of the image
			GL_RGB, //GL_RGB, because pixels are stored in RGB format
			GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
			//as unsigned numbers
			image->pixels); //The actual pixel data
	return textureId; //Returns the id of the texture
}

//converts window coordinates to openGL coordinates
//function not used now, but imp function : let it be here
Coordinate_openGl getOGLPos(float mX, float mY) {
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (float) mX;
	winY = (float) viewport[3] - (float) mY;
	glReadPixels(mX, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX,
			&posY, &posZ);

	return Coordinate_openGl(posX, posY, posZ);
}

//Called when the window is resized
void handleResize(int weight, int height) {
	windowWidth = weight;
	windowHeight = height;
	//Tell OpenGL how to convert from coordinates to pixel values
	glViewport(0, 0, windowWidth, windowHeight);
	glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective
	glPushMatrix();
	//Set the camera perspective
	glLoadIdentity(); //Reset the camera
	gluPerspective(45.0, //The camera angle
			(double) weight / (double) height, //The width-to-height ratio
			1.0, //The near z clipping coordinate
			200.0); //The far z clipping coordinate
}

GLuint getTextureFromImage(string path) {
	GLuint _textureId; //The id of the texture
	Image *bgImage = loadBMP(path.c_str());
	_textureId = loadTexture(bgImage);
	delete bgImage;
	return _textureId;
}

#endif
