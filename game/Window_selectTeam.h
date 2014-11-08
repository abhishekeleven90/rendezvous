#ifndef WINDOW_selectTeam_H
#define WINDOW_selectTeam_H

#include "text3d.h"
#include "Window_selectHero.h"

void loadTextures_selectTeam() {

}

//Initializes 3D rendering
void initRendering_selectTeam() {
	//Makes 3D drawing work when something is in front of something else
	glEnable( GL_DEPTH_TEST);
	glEnable(( GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_selectTeam();

	t3dInit();
}

//Draws the 3D scene
void drawScene_selectTeam() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode( GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective


	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_selectTeam(unsigned char key, //The key that was pressed
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

void processRightClick_selectTeam() {
	//handleGridCharSwitch(downGrid_click, PROCESS_MOVE_RIGHT_CLICK);
}

void processLeftClick_selectTeam() {
	//handleGridCharSwitch(downGrid_click, PROCESS_MOVE_LEFT_CLICK);
}

void myMouseClickHandler_selectTeam(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_RIGHT_BUTTON) { //used for moving...reaches here only if GLUT_UP
		processRightClick_selectTeam();
	} else {
		processLeftClick_selectTeam();
	}
}

void create_window_selectTeam() {
	windowId_current = glutCreateWindow("Help!!!");
	initRendering_selectTeam(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_selectTeam);
	glutKeyboardFunc(handleKeypress_selectTeam);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_selectTeam);

	timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec
	playBgSound();
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

#endif
