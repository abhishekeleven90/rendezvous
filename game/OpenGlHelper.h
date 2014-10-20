#include "Headers.h"

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

//Called when a key is pressed
void handleKeypress(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //Escape key
		exit(0); //Exit the program
	}
}

//Called when the window is resized
void handleResize(int w, int h) {
	//Tell OpenGL how to convert from coordinates to pixel values
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective

	glPushMatrix();

	//Set the camera perspective
	glLoadIdentity(); //Reset the camera
	gluPerspective(45.0, //The camera angle
			(double) w / (double) h, //The width-to-height ratio
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

void loadTextures() {
	grass_texId = getTextureFromImage("data/images/bg.bmp");
	spawn_texId = getTextureFromImage("data/images/spawn.bmp");

	attribute_bg_texId = getTextureFromImage("data/images/attribute_bg.bmp");

	tree_texId = getTextureFromImage("data/images/tree.bmp");
	stone_texId = getTextureFromImage("data/images/stone.bmp");

	t_demons_texId = getTextureFromImage("data/images/temple_demons.bmp");

	h_stunner_texId = getTextureFromImage("data/images/hero_mario.bmp");
	h_snatcher_texId = getTextureFromImage("data/images/hero_soldier.bmp");
}
