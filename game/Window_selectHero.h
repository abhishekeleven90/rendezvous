#ifndef WINDOW_SELECT_HERO_H
#define WINDOW_SELECT_HERO_H

void loadTextures_first() {
	btn_start_texId = getTextureFromBmp(PATH_IMG_BTN_START);
}

//Initializes 3D rendering
void initRendering_selectHero() {
	//Makes 3D drawing work when something is in front of something else
	glEnable( GL_DEPTH_TEST);
	glEnable(( GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	//loadTextures_first(); //TODO: uncomment
	loadTextures_main();//TODO: remove
	putGrass();
	t3dInit();
}

void PutHeros() {
	putBmpToCell(Coordinate_grid(7, 6), texId_h_slower, 4, 4);
	putPngToCell(Coordinate_grid(8, 6), texId_att_h_slower, 4, 1);

	putBmpToCell(Coordinate_grid(7, 16), texId_h_burster, 4, 4);
	putPngToCell(Coordinate_grid(8, 16), texId_att_h_burster, 4, 1);

	putBmpToCell(Coordinate_grid(17, 6), texId_h_stunner, 4, 4);
	putPngToCell(Coordinate_grid(18, 6), texId_att_h_stunner, 4, 1);

	putBmpToCell(Coordinate_grid(17, 16), texId_h_disabler, 4, 4);
	putPngToCell(Coordinate_grid(18, 16), texId_att_h_disabler, 4, 1);
}

//Draws the 3D scene
void drawScene_selectHero() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode( GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective
	PutHeros();

	renderGrid();

	glutSwapBuffers();
}

//Called when a key is pressed
void handleKeypress_selectHero(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;

	case 57: //key - '9' //TODO:for testing - shall be while clicking start
		glutDestroyWindow(windowId_current);
		t3dCleanup();
		create_window_main();
		break;

	case 48: //key - '0'
		isPrimaryNode = true;
		currPlayerId = 0;
		players[0].heroType = HERO_DISABLER;
		players[0].networkDetails = convertToNodeHelper("127.0.0.1:5000");
		players[0].team = &angelsTeam;
		break;

	case 49: //key - '1'
		currPlayerId = 1;
		players[1].heroType = HERO_STUNNER;
		players[1].networkDetails = convertToNodeHelper("127.0.0.1:5001");
		players[1].team = &angelsTeam;
		cout << "reached1" << endl;
		break;

	case 50: //key - '2'
		currPlayerId = 2;
		players[2].heroType = HERO_BURSTER;
		players[2].networkDetails = convertToNodeHelper("127.0.0.1:5002");
		players[2].team = &demonsTeam;
		break;

	case 51: //key - '3'
		currPlayerId = 3;
		players[3].heroType = HERO_SLOWER;
		players[3].networkDetails = convertToNodeHelper("127.0.0.1:5003");
		players[3].team = &demonsTeam;
		currPlayerId = 1;
		break;
	}
}

void processRightClick_selectHero(Coordinate_grid grid) {

	//TODO: remove below cout(s)
	cout << " row: " << downGrid_click.row;
	cout << " col: " << downGrid_click.col << endl;

}

void myMouseClickHandler_selectHero(int button, int state, int x, int y) {

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

void create_window_selectHero() {
	windowId_current = glutCreateWindow("Select a Hero!!!");
	initRendering_selectHero(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_selectHero);
	glutKeyboardFunc(handleKeypress_selectHero);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_selectHero);

	//timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec
	//playBgSound();
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

#endif
