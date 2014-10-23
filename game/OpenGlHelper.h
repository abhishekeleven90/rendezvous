#include "CustomVectorStruct.h"
#include "AStar.h"
#include "MapNGrid.h"
#include "SoundAll.h"

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

//converts window coordinates to openGL coordinates
Coordinate_openGl getOGLPos(int mX, int mY) {
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

Coordinate_grid downGrid;
void processClick(charCellType charType) {
	//handleGridCharSwitch( PROCESS_MOVE_CLICK);
}

//use downGrid
void processClickForAStar() {

	//TODO: eliminate if outerGrid click
	//assuming : the click is valid and AStar needs to be called
	//have to handle block status here istelf! phew :(

	for(int i=START_GRID_ROW;i<=END_GRID_ROW;i++)
	{
		for(int j=START_INNER_GRID_COL;j<=END_INNER_GRID_COL;j++)
		{
			if(isBlockedSite(i,j))
			{
				blockSiteAStarGrid(i,j);
			}
			else{
				openSiteAStarGrid(i,j);
			}
		}

	}
	initAStar(HERO_MINE_1_LOC, downGrid);//source and target given
	AStar();
	//TODO:through or just to target!
	//now move his ass periodically

}

void myMouseClickHandler(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid)) {
		playEventSound(PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { //used for moving

		playEventSound(PATH_SOUND_WRONG_CLICK);

		charCellType charType = getInnerGridChar(downGrid.row, downGrid.col);
		processClick(charType);

		//TODO: remove below cout(s)
		cout << " row: " << downGrid.row;
		cout << " col: " << downGrid.col << endl;

		processClickForAStar();
	}
}

//Called when the window is resized
void handleResize(int w, int h) {
	//Tell OpenGL how to convert from coordinates to pixel values
	glViewport(0, 0, w, h);

	glMatrixMode( GL_PROJECTION); //Switch to setting the camera perspective

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
