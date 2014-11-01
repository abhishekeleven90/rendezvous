#ifndef OPEN_GL_HELPER_H
#define OPEN_GL_HELPER_H

#include "ImageLoader.h"
#include "Constants.h"
#include "CustomVectorStruct.h"
#include "SoundAll.h"
#include "ActionOnGrid.h"
#include "Objects.h"
#include "Headers.h"
#include "lodepng.h"

//Makes the image into a texture, and returns the id of the texture
GLuint loadBmpTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

GLuint getTextureFromBmp(string path) {
	GLuint _textureId; //The id of the texture
	Image *bgImage = loadBMP(path.c_str());
	_textureId = loadBmpTexture(bgImage);
	delete bgImage;
	return _textureId;
}

MyTexture getTextureFromPng(string path) {

	MyTexture myTexture;
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, path);

	// If there's an error, display it.
	if (error != 0) {
		std::cout << "error " << error << ": " << lodepng_error_text(error)
				<< std::endl;
		return myTexture;
	}

	// Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
	size_t u2 = 1;
	while (u2 < width)
		u2 *= 2;
	size_t v2 = 1;
	while (v2 < height)
		v2 *= 2;
	// Ratio for power of two version compared to actual version, to render the non power of two image with proper size.
	double u3 = (double) width / u2;
	double v3 = (double) height / v2;

	// Make power of two version of the image.
	std::vector<unsigned char> image2(u2 * v2 * 4);
	for (size_t y = 0; y < height; y++)
		for (size_t x = 0; x < width; x++)
			for (size_t c = 0; c < 4; c++) {
				image2[4 * u2 * y + 4 * x + c] = image[4 * width * y + 4 * x
						+ c];
			}

	// Enable the texture for OpenGL.


	glGenTextures(1, &myTexture.textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, myTexture.textureId); //Tell OpenGL which texture to edit
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST = no smoothing
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			&image2[0]);

	myTexture.u3 = u3;
	myTexture.v3 = v3;

	return myTexture;
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

#endif
