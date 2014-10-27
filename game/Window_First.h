#ifndef WINDOW_FIRST_H
#define WINDOW_FIRST_H

void loadTextures_first() {
	btn_start_texId = getTextureFromImage(PATH_IMG_BTN_START);
}

//Initializes 3D rendering
void initRendering_first() {
	//Makes 3D drawing work when something is in front of something else
	glEnable( GL_DEPTH_TEST);

	//Loading all the textures from images
	loadTextures_first();
}

//Draws the 3D scene
void drawScene_first() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode( GL_MODELVIEW); //Switch to the drawing perspective
	glLoadIdentity(); //Reset the drawing perspective

	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_first(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		exit(0);
		break;

	case 51: //key - '3' //TODO:for testing - shall be while clicking start
		glutDestroyWindow(windowId_current);
		create_window_main();
		break;
	}
}

void processRightClick_first(Coordinate_grid grid) {

	//TODO: remove below cout(s)
	cout << " row: " << downGrid_click.row;
	cout << " col: " << downGrid_click.col << endl;

}

void myMouseClickHandler_first(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_RIGHT_BUTTON) { //used for moving
		//processRightClick_main(downGrid);
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

	//timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec
	//playBgSound();
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

#endif
