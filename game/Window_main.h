#ifndef WINDOW_MAIN_H
#define WINDOW_MAIN_H

#include "text3d.h"
#include "Network.h"

void loadTextures_main() {
	texId_bg_grass = getTextureFromBmp(PATH_IMG_BG_WINDOW_MAIN);
	texId_bg_spawn = getTextureFromBmp(PATH_IMG_BG_SPAWN);
	texId_bg_war = getTextureFromBmp(PATH_IMG_BG_WAR);
	texId_bg_attribute = getTextureFromBmp(PATH_IMG_BG_ATTRIBUTE);
	texId_bg_blocked = getTextureFromBmp(PATH_IMG_BG_BLOCKED);

	texId_tree = getTextureFromBmp(PATH_IMG_TREE);
	texId_stone = getTextureFromBmp(PATH_IMG_STONE);
	texId_tree_war = getTextureFromBmp(PATH_IMG_TREE_WAR);
	texId_stone_war = getTextureFromBmp(PATH_IMG_STONE_WAR);

	texId_t_angels = getTextureFromBmp(PATH_IMG_TEMPLE_ANGELS);
	texId_t_demons = getTextureFromBmp(PATH_IMG_TEMPLE_DEMONS);

	texId_h_stunner = getTextureFromBmp(PATH_IMG_HERO_STUNNER);
	texId_h_disabler = getTextureFromBmp(PATH_IMG_HERO_DISABLER);
	texId_h_slower = getTextureFromBmp(PATH_IMG_HERO_SLOWER);
	texId_h_burster = getTextureFromBmp(PATH_IMG_HERO_BURSTER);

	texId_i_speedMov = getTextureFromBmp(PATH_IMG_ITEM_MOVEMENT_SPEED);
	texId_i_damage = getTextureFromBmp(PATH_IMG_ITEM_DAMAGE);
	texId_i_health = getTextureFromBmp(PATH_IMG_ITEM_HEALTH);
	texId_i_tHealer = getTextureFromBmp(PATH_IMG_ITEM_TEMPLE_HEALER);

	texId_att_cBurst = getTextureFromPng(PATH_IMG_ATT_CURSE_BURST);
	texId_att_cDisable = getTextureFromPng(PATH_IMG_ATT_CURSE_DISABLE);
	texId_att_cSlow = getTextureFromPng(PATH_IMG_ATT_CURSE_SLOW);
	texId_att_cStun = getTextureFromPng(PATH_IMG_ATT_CURSE_STUN);
	texId_att_cNone = getTextureFromPng(PATH_IMG_ATT_CURSE_NONE);

	texId_att_h_burster = getTextureFromPng(PATH_IMG_ATT_HERO_BURSTER);
	texId_att_h_disabler = getTextureFromPng(PATH_IMG_ATT_HERO_DISABLER);
	texId_att_h_slower = getTextureFromPng(PATH_IMG_ATT_HERO_SLOWER);
	texId_att_h_stunner = getTextureFromPng(PATH_IMG_ATT_HERO_STUNNER);

	texId_att_mBasic = getTextureFromPng(PATH_IMG_ATT_MODE_BASIC);
	texId_att_mMagic = getTextureFromPng(PATH_IMG_ATT_MODE_MAGIC);
	texId_att_mStun = getTextureFromPng(PATH_IMG_ATT_MODE_STUN);

	texId_att_team_Angels = getTextureFromPng(PATH_IMG_ATT_TEAM_ANGELS);
	texId_att_team_Demons = getTextureFromPng(PATH_IMG_ATT_TEAM_DEMONS);

	texId_att_team_enemy = getTextureFromPng(PATH_IMG_ATT_TEAM_ENEMY);
	texId_att_team_my = getTextureFromPng(PATH_IMG_ATT_TEAM_MY);

	texId_att_time_itemOff = getTextureFromPng(PATH_IMG_ATT_TIMERS_ITEM_OFF);
	texId_att_time_itemOn = getTextureFromPng(PATH_IMG_ATT_TIMERS_ITEM_ON);
	texId_att_time_magicOff = getTextureFromPng(PATH_IMG_ATT_TIMERS_MAGIC_OFF);
	texId_att_time_magicOn = getTextureFromPng(PATH_IMG_ATT_TIMERS_MAGIC_ON);

	texId_att_health = getTextureFromPng(PATH_IMG_ATT_HEALTH);
	texId_att_temple_health = getTextureFromPng(PATH_IMG_ATT_TEMPLE_HEALTH);
	texId_att_strength = getTextureFromPng(PATH_IMG_ATT_STRENGTH);
	texId_att_speed = getTextureFromPng(PATH_IMG_ATT_SPEED);

	texId_att_wall = getTextureFromPng(PATH_IMG_ATT_WALL);
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
	createServerThread();
}

void renderGridMainWindow() {
	moveHeroMine(1);
	moveHeroMine(2); //TODO: check if not required..later may be required

	loadAttributeSpace();
	renderGrid();
}

//Draws the 3D scene
void drawScene_main() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode( GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective


	//printGrid();
	renderGridMainWindow();
	putTextToLAttCell(Coordinate_grid(3, 1), numToStr(server_port)); //TODO: not req here

	glutSwapBuffers(); //Send the 3D scene to the screen
}

void selectBasicPower() {
	cout << "selected power_basic" << endl;
	myTeam.players[playerId - 1].currentPowerMode = POWER_MODE_BASIC;
}

void selectMagicPower() {
	if (currPlayer.curseType != CURSE_DISABLE) {
		cout << "selected power_magic" << endl;
		myTeam.players[playerId - 1].currentPowerMode = POWER_MODE_MAGIC;
	} else {
		cout << "can not use Magic power, you are cursed!!!" << endl; //TODO: show on wall
	}
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
		break;

	case 49: //key - '1' : select basicPower
		selectBasicPower();
		createClientBroadcastThread(); //TODO: not required here
		break;

	case 50: //key - '2' : select magicPower
		selectMagicPower();
		break;

	case 51: //key - '3' : curse //TODO: case may not be required later
		iAmCursed(CURSE_STUN);
		break;

	case 48: //key - '0' //TODO: may not be required later : just for testing purpose
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
