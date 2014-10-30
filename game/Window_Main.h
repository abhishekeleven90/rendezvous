#ifndef WINDOW_MAIN_H
#define WINDOW_MAIN_H

void loadTextures_main() {
	bg_grass_texId = getTextureFromImage(PATH_IMG_BG_WINDOW_MAIN);
	bg_spawn_texId = getTextureFromImage(PATH_IMG_BG_SPAWN);
	bg_war_texId = getTextureFromImage(PATH_IMG_BG_WAR);
	bg_attribute_texId = getTextureFromImage(PATH_IMG_BG_ATTRIBUTE);
	bg_blocked_texId = getTextureFromImage(PATH_IMG_BG_BLOCKED);

	tree_texId = getTextureFromImage(PATH_IMG_TREE);
	stone_texId = getTextureFromImage(PATH_IMG_STONE);

	t_demons_texId = getTextureFromImage(PATH_IMG_TEMPLE_DEMONS);

	//TODO: uncomment
	//h_stunner_texId = getTextureFromImage(PATH_IMG_HERO_STUNNER);
	//h_disabler_texId = getTextureFromImage(PATH_IMG_HERO_DISABLER);
	h_slower_texId = getTextureFromImage(PATH_IMG_HERO_SLOWER);
	h_burster_texId = getTextureFromImage(PATH_IMG_HERO_BURSTER);

	i_speedMov_texId = getTextureFromImage(PATH_IMG_ITEM_MOVEMENT_SPEED);
	i_speedAttack_texId = getTextureFromImage(PATH_IMG_ITEM_ATTACK_SPEED);
	i_damage_texId = getTextureFromImage(PATH_IMG_ITEM_DAMAGE);
	i_health_texId = getTextureFromImage(PATH_IMG_ITEM_HEALTH);
	i_tHealer_texId = getTextureFromImage(PATH_IMG_ITEM_TEMPLE_HEALER);
}

//Initializes 3D rendering
void initRendering_main() {
	//Makes 3D drawing work when something is in front of something else
	glEnable( GL_DEPTH_TEST);

	//Loading all the textures from images
	loadTextures_main();

	initMap();

	putHeros();

	//Placing items in the map
	for (int i = 0; i < ITEM_MAP_COUNT; i++) {
		placeItemAtRandomPos(TEAM_BOTH);
	}

	loadPlayerSpecificAttributes();

}

//Draws the 3D scene
void drawScene_main() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode( GL_MODELVIEW); //Switch to the drawing perspective
	glLoadIdentity(); //Reset the drawing perspective

	//printGrid();
	renderGrid();
	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_main(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		exit(0);
		break;

	case 32: //key - 'space' : on/off bgMusic
		toggleBgMusic();
		togglePlayer();
		break;

	case 49: //key - '1' : select basicPower
		cout << "selected power_basic" << endl;
		//TODO : show in attribute space
		playerStats.currentPower = POWER_BASIC;
		break;

	case 50: //key - '2' : select magicPower
		//TODO : show in attribute space
		cout << "selected power_magic" << endl;
		playerStats.currentPower = POWER_MAGIC;
		break;
	}
}

void processRightClick_main(Coordinate_grid grid) {

	//TODO: remove below cout(s)
	cout << " row: " << downGrid_click.row;
	cout << " col: " << downGrid_click.col << endl;

	handleGridCharSwitch(downGrid_click, PROCESS_MOVE_RIGHT_CLICK);
}

void myMouseClickHandler_main(int button, int state, int x, int y) {

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
		processRightClick_main(downGrid_click);
	}
}

void create_window_main() {
	windowId_current = glutCreateWindow("Rendezvous!!!");
	initRendering_main(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_main);
	glutKeyboardFunc(handleKeypress_main);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_main);

	timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec
	playBgSound();
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

#endif
