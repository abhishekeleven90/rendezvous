#ifndef WINDOW_first_H
#define WINDOW_first_H

#include "text3d.h"
#include "Window_selectHero.h"
#include "MapNGrid.h"

void loadTextures_first() {
	texId_bg_grass = getTextureFromBmp(PATH_IMG_BG_WINDOW_MAIN);
	textId_logo = getTextureFromPng(PATH_IMG_LOGO);
	textId_help = getTextureFromPng(PATH_IMG_HELP);
}

//Initializes 3D rendering
void initRendering_first() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_first();
	putGrass();

	t3dInit();
}

void putImages() {
	putPngToCell(Coordinate_grid(5, 6), textId_logo, 14, 4);
}

//Draws the 3D scene
void drawScene_first() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putImages();
	renderGrid();
	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_first(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;

	case 57: //key - '9' //TODO:for testing - shall be while clicking start
		glutDestroyWindow(windowId_current);
		t3dCleanup();
		create_window_selectHero();
		break;
	}
}

void processRightClick_first() {
	//handleGridCharSwitch(downGrid_click, PROCESS_MOVE_RIGHT_CLICK);
}

void processLeftClick_first() {
	//handleGridCharSwitch(downGrid_click, PROCESS_MOVE_LEFT_CLICK);
}

void myMouseClickHandler_first(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound(PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_RIGHT_BUTTON) { //used for moving...reaches here only if GLUT_UP
		processRightClick_first();
	} else {
		processLeftClick_first();
	}
}

void create_window_first() {
	windowId_current = glutCreateWindow("Welcome!!!");
	initRendering_first(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_first);
	glutKeyboardFunc(handleKeypress_first);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_first);

	timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec
	playBgSound();
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

#endif
