#ifndef WINDOW_FIRST_H
#define WINDOW_FIRST_H

void loadTextures_first() {
	btn_start_texId = getTextureFromImage(PATH_IMG_BTN_START);
}

//Initializes 3D rendering
void initRendering_first() {
	//Makes 3D drawing work when something is in front of something else
	glEnable( GL_DEPTH_TEST);
	glEnable(( GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_first();
	t3dInit();
}

//Draws the 3D scene
void drawScene_first() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode( GL_MODELVIEW); //Switch to the drawing perspective
	glLoadIdentity(); //Reset the drawing perspective


	//renderBitmapString(Coordinate_grid(5, 5), "Esc - Quit");

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -3.0f);
	float _scale = t3dComputeScale("helloooooo");
	glScalef(_scale, _scale, _scale);
	glColor3f(1.0f, 1.0f, 1.0f);

	//glTranslatef(0, 0, 1.5f / _scale);
	t3dDraw3D("hellooooo", 0, 0, 0.2f);
	glPopMatrix();

	glutSwapBuffers();
}

//Called when a key is pressed
void handleKeypress_first(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;

	case 51: //key - '3' //TODO:for testing - shall be while clicking start
		glutDestroyWindow(windowId_current);
		t3dCleanup();
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
