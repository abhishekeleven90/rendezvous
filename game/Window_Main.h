#ifndef WINDOW_MAIN_H
#define WINDOW_MAIN_H

#include "text3d.h"

void loadTextures_main() {
	texId_bg_grass = getTextureFromBmpImage(PATH_IMG_BG_WINDOW_MAIN);
	texId_bg_spawn = getTextureFromBmpImage(PATH_IMG_BG_SPAWN);
	texId_bg_war = getTextureFromBmpImage(PATH_IMG_BG_WAR);
	texId_bg_attribute = getTextureFromBmpImage(PATH_IMG_BG_ATTRIBUTE);
	texId_bg_blocked = getTextureFromBmpImage(PATH_IMG_BG_BLOCKED);

	texId_tree = getTextureFromBmpImage(PATH_IMG_TREE);
	texId_stone = getTextureFromBmpImage(PATH_IMG_STONE);

	texId_t_demons = getTextureFromBmpImage(PATH_IMG_TEMPLE_DEMONS);

	//TODO: uncomment
	//h_stunner_texId = getTextureFromImage(PATH_IMG_HERO_STUNNER);
	//h_disabler_texId = getTextureFromImage(PATH_IMG_HERO_DISABLER);
	texId_h_slower = getTextureFromBmpImage(PATH_IMG_HERO_SLOWER);
	texId_h_burster = getTextureFromBmpImage(PATH_IMG_HERO_BURSTER);

	texId_i_speedMov = getTextureFromBmpImage(PATH_IMG_ITEM_MOVEMENT_SPEED);
	texId_i_speedAttack = getTextureFromBmpImage(PATH_IMG_ITEM_ATTACK_SPEED);
	texId_i_damage = getTextureFromBmpImage(PATH_IMG_ITEM_DAMAGE);
	texId_i_health = getTextureFromBmpImage(PATH_IMG_ITEM_HEALTH);
	texId_i_tHealer = getTextureFromBmpImage(PATH_IMG_ITEM_TEMPLE_HEALER);
}

//Initializes 3D rendering
void initRendering_main() {
	//Makes 3D drawing work when something is in front of something else
	glEnable( GL_DEPTH_TEST);
	glEnable(( GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_main();

	initMap();

	putHeros();

	//Placing items in the map
	for (int i = 0; i < ITEMS_ON_MAP_COUNT; i++) {
		placeItemAtRandomPos(TEAM_BOTH);
	}

	loadTeamAttributes();
	loadPlayerSpecificAttributes();

	t3dInit();

}

//Draws the 3D scene
void drawScene_main() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode( GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective


	//printGrid();
	renderGrid();

	//putTextToCell(Coordinate_grid(1, 1), "type:");
	//putTextToCell(Coordinate_grid(1, 2), "stun");

	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_main(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;

	case 32: //key - 'space' : on/off bgMusic
		toggleBgMusic();
		togglePlayer();
		break;

	case 49: //key - '1' : select basicPower
		cout << "selected power_basic" << endl;
		//TODO : show in attribute space
		myTeam.players[currentPlayer - 1].currentPower = POWER_BASIC;
		break;

	case 50: //key - '2' : select magicPower
		//TODO : show in attribute space
		cout << "selected power_magic" << endl;
		myTeam.players[currentPlayer - 1].currentPower = POWER_MAGIC;
		break;

	case 48:
		cout << "Switching player" << endl;
		togglePlayer();
		break;
	}
}

void processRightClick_main() {

	//TODO: remove below cout(s)
	cout << " row: " << downGrid_click.row;
	cout << " col: " << downGrid_click.col << endl;

	handleGridCharSwitch(downGrid_click, PROCESS_MOVE_RIGHT_CLICK);
}

void processLeftClick_main() {

	//TODO: remove below cout(s)
	cout << " left click row: " << downGrid_click.row;
	cout << " left click col: " << downGrid_click.col << endl;

	handleGridCharSwitch(downGrid_click, PROCESS_MOVE_LEFT_CLICK);
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
		processRightClick_main();
	} else {
		processLeftClick_main();
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
