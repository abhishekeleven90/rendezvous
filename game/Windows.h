#ifndef WINDOWS_H
#define WINDOWS_H

#include "text3d.h"
#include "Network.h"

void moveToWindow(void f()) {
	glutDestroyWindow(windowId_current);
	t3dCleanup();
	f();
}

void create_window_first();
void create_window_final();
void create_window_help();
void create_window_multiplayer();
void create_window_selectHero();
void create_window_selectTeam();
void create_window_main();
/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_FIRST--------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_first() {
	texId_bg = getTextureFromPng(PATH_IMG_BG);
	texId_logo = getTextureFromPng(PATH_IMG_LOGO);
	texId_help = getTextureFromPng(PATH_IMG_HELP);

	texId_player_single = getTextureFromPng(PATH_IMG_PLAYER_SINGLE);
	texId_player_multi = getTextureFromPng(PATH_IMG_PLAYER_MULTI);
}

void putImages_first() {
	putPngToCell(Coordinate_grid(5, 6), texId_logo, 14, 4);
	putPngWithChar(18, 12, texId_help, CLICK_HELP, 2, 2);
	putPngWithChar(13, 7, texId_player_single, CLICK_PLAYER_SINGLE, 4, 1);
	putPngWithChar(13, 15, texId_player_multi, CLICK_PLAYER_MULTI, 4, 1);

	putPngToCell(Coordinate_grid(20, 1), texId_bg, 24, 20);
}

//Initializes 3D rendering
void initRendering_first() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gameDetails.isConnectedToServer = false;

	//Loading all the textures from images
	loadTextures_first();
	t3dInit();
}

//Draws the 3D scene
void drawScene_first() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective


	putImages_first();
	//renderGrid();
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
	}
}

void processRightClick_first() {
	//handleGridCharSwitch(downGrid_click, PROCESS_MOVE_RIGHT_CLICK);
}

void processLeftClick_first() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_HELP:
		moveToWindow(create_window_help);
		break;
	case CLICK_PLAYER_SINGLE:
		gameDetails.isSinglePlayerGame = true;
		moveToWindow(create_window_selectTeam);
		break;
	case CLICK_PLAYER_MULTI:
		gameDetails.isSinglePlayerGame = false;
		moveToWindow(create_window_multiplayer);
		break;
	}
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

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_FINAL--------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_final() {

}

//Initializes 3D rendering
void initRendering_final() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_final();

	t3dInit();
}

//Draws the 3D scene
void drawScene_final() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective


	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_final(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void processRightClick_final() {
	//handleGridCharSwitch(downGrid_click, PROCESS_MOVE_RIGHT_CLICK);
}

void processLeftClick_final() {
	//handleGridCharSwitch(downGrid_click, PROCESS_MOVE_LEFT_CLICK);
}

void myMouseClickHandler_final(int button, int state, int x, int y) {

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
		processRightClick_final();
	} else {
		processLeftClick_final();
	}
}

void create_window_final() {
	windowId_current = glutCreateWindow("End!!!");
	initRendering_final(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_final);
	glutKeyboardFunc(handleKeypress_final);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_final);

	timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_HELP---------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_help() {
	texId_bg = getTextureFromPng(PATH_IMG_BG);

	texId_btn_back = getTextureFromPng(PATH_IMG_BACK);
}

void putImages_help() {
	putPngWithChar(18, 12, texId_btn_back, CLICK_BACK, 2, 2);

	putPngToCell(Coordinate_grid(20, 1), texId_bg, 24, 20);
}

//Initializes 3D rendering
void initRendering_help() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_help();
	t3dInit();
}

//Draws the 3D scene
void drawScene_help() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putImages_help();
	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_help(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void processRightClick_help() {
	//handleGridCharSwitch(downGrid_click, PROCESS_MOVE_RIGHT_CLICK);
}

void processLeftClick_help() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_BACK:
		moveToWindow(create_window_first);
		break;
	}
}

void myMouseClickHandler_help(int button, int state, int x, int y) {

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
		processRightClick_help();
	} else {
		processLeftClick_help();
	}
}

void create_window_help() {
	windowId_current = glutCreateWindow("Help!!!");
	initRendering_help(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_help);
	glutKeyboardFunc(handleKeypress_help);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_help);

	timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_MULTIPLAYER--------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
string hostIp;
bool isShowInput;

void loadTextures_multiplayer() {
	texId_bg = getTextureFromPng(PATH_IMG_BG);

	texId_btn_back = getTextureFromPng(PATH_IMG_BACK);

	texId_join_game = getTextureFromPng(PATH_IMG_JOIN_GAME);
	texId_host_game = getTextureFromPng(PATH_IMG_HOST_GAME);

	texId_white = getTextureFromPng(PATH_IMG_WHITE);
	texId_btn_next = getTextureFromPng(PATH_IMG_BTN_NEXT);
	texId_try_again = getTextureFromPng(PATH_IMG_TRY_AGAIN);
}

void putImages_multiplayer() {
	putPngWithChar(18, 12, texId_btn_back, CLICK_BACK, 2, 2);

	putPngWithChar(7, 6, texId_host_game, CLICK_HOST_GAME, 4, 2);
	putPngWithChar(7, 16, texId_join_game, CLICK_JOIN_GAME, 4, 2);

	if (isShowInput) {
		putTextToCell(Coordinate_grid(9, 16), concat("  ", hostIp));

		//Important - keep the "coordinates & blocks" same here & in below(else) part
		putPngWithChar(9, 16, texId_input, INPUT, 4, 1);

		putPngWithChar(9, 22, texId_btn_next, CLICK_NEXT, 1, 1);

		if (gameDetails.isIssueConnectingToServer) {
			putPngWithChar(11, 16, texId_try_again, CLICK_TRY_AGAIN, 2, 1);
		} else {
			putChars(2, 1, 11, 16, BG_GRASS);
		}
	} else {
		hostIp = "";
		//below is imp to avoid clicks if the button is not visibles
		putChars(4, 1, 9, 16, BG_GRASS); //input btn
		putChars(1, 1, 9, 22, BG_GRASS); //next btn
		putChars(2, 1, 11, 16, BG_GRASS); //tryAgain btn
	}

	putPngToCell(Coordinate_grid(20, 1), texId_bg, 24, 20);
}

//Initializes 3D rendering
void initRendering_multiplayer() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	gameDetails.isHost = true;
	gameDetails.isIssueConnectingToServer = false;
	isShowInput = false;
	loadTextures_multiplayer();

	t3dInit();
}

//Draws the 3D scene
void drawScene_multiplayer() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putImages_multiplayer();
	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_multiplayer(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	case 57: //'9'
		hostIp = concat(hostIp, "9");
		break;
	case 56://'8'
		hostIp = concat(hostIp, "8");
		break;
	case 55://'7'
		hostIp = concat(hostIp, "7");
		break;
	case 54://'6'
		hostIp = concat(hostIp, "6");
		break;
	case 53://'5'
		hostIp = concat(hostIp, "5");
		break;
	case 52://'4'
		hostIp = concat(hostIp, "4");
		break;
	case 51://'3'
		hostIp = concat(hostIp, "3");
		break;
	case 50://'2'
		hostIp = concat(hostIp, "2");
		break;
	case 49://'1'
		hostIp = concat(hostIp, "1");
		break;
	case 48://'0'
		hostIp = concat(hostIp, "0");
		break;
	case 46://'.'
		hostIp = concat(hostIp, ".");
		break;
	case 58://':'
		hostIp = concat(hostIp, ":");
		break;
	}
}

void processRightClick_multiplayer() {
	//handleGridCharSwitch(downGrid_click, PROCESS_MOVE_RIGHT_CLICK);
}

void processLeftClick_multiplayer() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_BACK:
		moveToWindow(create_window_first);
		break;
	case CLICK_JOIN_GAME:
		gameDetails.isHost = false;
		isShowInput = true;
		break;
	case CLICK_HOST_GAME:
		isShowInput = false;
		gameDetails.isHost = true;
		gameDetails.isIssueConnectingToServer = false;
		moveToWindow(create_window_selectTeam);
		break;
	case CLICK_NEXT:
		if (hostIp != "") {
			gameDetails.hostIp = hostIp;
			helperSendConnect();
			if (gameDetails.isConnectedToServer) {
				moveToWindow(create_window_selectTeam);
			}
		}
		break;
	case CLICK_TRY_AGAIN:
		hostIp = "";
		break;
	}
}

void myMouseClickHandler_multiplayer(int button, int state, int x, int y) {

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
		processRightClick_multiplayer();
	} else {
		processLeftClick_multiplayer();
	}
}

void create_window_multiplayer() {
	windowId_current = glutCreateWindow("Server Details!!!");
	initRendering_multiplayer(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_multiplayer);
	glutKeyboardFunc(handleKeypress_multiplayer);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_multiplayer);

	timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_WAITING------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/

void loadTextures_waiting() {
	texId_bg_waiting = getTextureFromPng(PATH_IMG_BG_WAITING);
}

void putImages_waiting() {
	putPngToCell(Coordinate_grid(20, 1), texId_bg_waiting, 24, 20);
}

//Initializes 3D rendering
void initRendering_waiting() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	gameDetails.isTimerHostWaitDone = false;
	timerHostWait(0);
	loadTextures_waiting();

	t3dInit();
}

//Draws the 3D scene
void drawScene_waiting() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putTextToCell(Coordinate_grid(17, 7),
			concat("Host Ip-> ", selfNode->ipWithPort));
	putImages_waiting();

	if (gameDetails.isTimerHostWaitDone) {
		moveToWindow(create_window_main);
	}

	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_waiting(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void processRightClick_waiting() {
}

void processLeftClick_waiting() {
}

void myMouseClickHandler_waiting(int button, int state, int x, int y) {

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
		processRightClick_waiting();
	} else {
		processLeftClick_waiting();
	}
}

void create_window_waiting() {
	windowId_current = glutCreateWindow("Waiting for others to join...");
	initRendering_waiting(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_waiting);
	glutKeyboardFunc(handleKeypress_waiting);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_waiting);

	timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_SELECT_HERO--------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_selectHero() {
	texId_bg = getTextureFromPng(PATH_IMG_BG);
	texId_pick_hero = getTextureFromPng(PATH_IMG_PICK_HERO);

	texId_h_stunner = getTextureFromBmp(PATH_IMG_HERO_STUNNER);
	texId_h_disabler = getTextureFromBmp(PATH_IMG_HERO_DISABLER);
	texId_h_slower = getTextureFromBmp(PATH_IMG_HERO_SLOWER);
	texId_h_burster = getTextureFromBmp(PATH_IMG_HERO_BURSTER);

	texId_att_h_burster = getTextureFromPng(PATH_IMG_ATT_HERO_BURSTER);
	texId_att_h_disabler = getTextureFromPng(PATH_IMG_ATT_HERO_DISABLER);
	texId_att_h_slower = getTextureFromPng(PATH_IMG_ATT_HERO_SLOWER);
	texId_att_h_stunner = getTextureFromPng(PATH_IMG_ATT_HERO_STUNNER);

	texId_att_team_angels = getTextureFromPng(PATH_IMG_ATT_TEAM_ANGELS);
	texId_att_team_demons = getTextureFromPng(PATH_IMG_ATT_TEAM_DEMONS);

	texId_t_angels = getTextureFromBmp(PATH_IMG_TEMPLE_ANGELS);
	texId_t_demons = getTextureFromBmp(PATH_IMG_TEMPLE_DEMONS);
}

void PutImages_selectHero() {
	putBmpWithChar(9, 6, texId_h_slower, H_SLOWER, 4, 4);
	putPngWithChar(10, 6, texId_att_h_slower, H_SLOWER, 4, 1);

	putBmpWithChar(9, 16, texId_h_burster, H_BURSTER, 4, 4);
	putPngWithChar(10, 16, texId_att_h_burster, H_BURSTER, 4, 1);

	putBmpWithChar(17, 6, texId_h_stunner, H_STUNNER, 4, 4);
	putPngWithChar(18, 6, texId_att_h_stunner, H_STUNNER, 4, 1);

	putBmpWithChar(17, 16, texId_h_disabler, H_DISABLER, 4, 4);
	putPngWithChar(18, 16, texId_att_h_disabler, H_DISABLER, 4, 1);

	putPngToCell(Coordinate_grid(3, 10), texId_pick_hero, 6, 1);
	putPngToCell(Coordinate_grid(20, 1), texId_bg, 24, 20);
}

//Initializes 3D rendering
void initRendering_selectHero() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	loadTextures_selectHero();
	putGrass();

	players[0].heroType = HERO_DISABLER; //TODO: check: shall be from server
	players[0].networkDetails = convertToNodeHelper("127.0.0.1:5000");
	//players[0].networkDetails = convertToNodeHelper("10.208.23.254:5000");
	players[0].team = &angelsTeam;
	players[0].isFirstPlayerInTeam = true;

	players[1].heroType = HERO_STUNNER;
	players[1].networkDetails = convertToNodeHelper("127.0.0.1:5001");
	players[1].team = &angelsTeam;

	players[2].heroType = HERO_BURSTER;
	players[2].networkDetails = convertToNodeHelper("127.0.0.1:5002");
	players[2].team = &demonsTeam;
	players[2].isFirstPlayerInTeam = true;

	players[3].heroType = HERO_SLOWER;
	players[3].networkDetails = convertToNodeHelper("127.0.0.1:5003");
	players[3].team = &demonsTeam;

	t3dInit();
}

//Draws the 3D scene
void drawScene_selectHero() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective


	PutImages_selectHero();
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

	case 48: //key - '0'
		currPlayerId = 0;
		break;

	case 49: //key - '1'
		currPlayerId = 1;
		break;

	case 50: //key - '2'
		currPlayerId = 2;
		break;

	case 51: //key - '3'
		currPlayerId = 3;
		break;
	}
}

void processRightClick_selectHero() {

}

void selectedHero_next() {
	if (gameDetails.isHost) {
		createServerThread();
		moveToWindow(create_window_waiting);
	}

	else {
		//TODO: validate hero from server
		createServerThread();
		moveToWindow(create_window_main);
	}
}

void processLeftClick_selectHero() {
	switch (getGridChar(downGrid_click)) {
	case H_SLOWER:
		gameDetails.myHero = HERO_SLOWER;
		selectedHero_next();
		break;
	case H_STUNNER:
		gameDetails.myHero = HERO_STUNNER;
		selectedHero_next();
		break;
	case H_DISABLER:
		gameDetails.myHero = HERO_DISABLER;
		selectedHero_next();
		break;
	case H_BURSTER:
		gameDetails.myHero = HERO_BURSTER;
		selectedHero_next();
		break;
	}
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

	if (button == GLUT_RIGHT_BUTTON) { //used for moving...reaches here only if GLUT_UP
		processRightClick_selectHero();
	} else {
		processLeftClick_selectHero();
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
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_SELECT_TEAM--------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_selectTeam() {
	texId_bg = getTextureFromPng(PATH_IMG_BG);
	texId_btn_back = getTextureFromPng(PATH_IMG_BACK);

	texId_pick_team = getTextureFromPng(PATH_IMG_PICK_TEAM);

	texId_att_team_angels = getTextureFromPng(PATH_IMG_ATT_TEAM_ANGELS);
	texId_att_team_demons = getTextureFromPng(PATH_IMG_ATT_TEAM_DEMONS);

	texId_t_angels = getTextureFromBmp(PATH_IMG_TEMPLE_ANGELS);
	texId_t_demons = getTextureFromBmp(PATH_IMG_TEMPLE_DEMONS);
}

void putImages_selectTeam() {
	putPngWithChar(18, 12, texId_btn_back, CLICK_BACK, 2, 2);

	putPngWithChar(5, 8, texId_pick_team, CLICK_PICK_TEAM, 9, 2);

	putBmpWithChar(11, 6, texId_t_angels, CLICK_ANGELS, 4, 4);
	putPngWithChar(13, 6, texId_att_team_angels, CLICK_ANGELS, 4, 2);
	putBmpWithChar(11, 16, texId_t_demons, CLICK_DEMONS, 4, 4);
	putPngWithChar(13, 16, texId_att_team_demons, CLICK_DEMONS, 4, 2);

	putPngToCell(Coordinate_grid(20, 1), texId_bg, 24, 20);
}

//Initializes 3D rendering
void initRendering_selectTeam() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_selectTeam();

	t3dInit();
}

//Draws the 3D scene
void drawScene_selectTeam() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putImages_selectTeam();
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
	}
}

void processRightClick_selectTeam() {
	//handleGridCharSwitch(downGrid_click, PROCESS_MOVE_RIGHT_CLICK);
}

void teamSelected_next() {
	if (!gameDetails.isHost) {
		//TODO: validate team from server
		moveToWindow(create_window_selectHero);
	}

	else {
		moveToWindow(create_window_selectHero);
	}
}

void processLeftClick_selectTeam() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_BACK:
		moveToWindow(create_window_first);
		break;
	case CLICK_ANGELS:
		gameDetails.myTeam = TEAM_ANGELS;
		teamSelected_next();
		break;
	case CLICK_DEMONS:
		gameDetails.myTeam = TEAM_DEMONS;
		teamSelected_next();
		break;
	}
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
	windowId_current = glutCreateWindow("Select a team!!!");
	initRendering_selectTeam(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_selectTeam);
	glutKeyboardFunc(handleKeypress_selectTeam);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_selectTeam);

	timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_MAIN---------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
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

	texId_att_team_angels = getTextureFromPng(PATH_IMG_ATT_TEAM_ANGELS);
	texId_att_team_demons = getTextureFromPng(PATH_IMG_ATT_TEAM_DEMONS);

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
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_main();
	initMap();

	//Placing items in the map
	for (int i = 0; i < ITEMS_ON_MAP_COUNT; i++) {
		initItemAtRandomPos();
	}

	//load Attributes
	loadTeamAttributes();

	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		loadPlayerGeneralAttributes(i);
	}

	copyPrimaryGrid();

	blockOpponentsArea();

	t3dInit();

	strcpy(primaryNodeIp, players[0].networkDetails->ip);
	primaryNodePort = players[0].networkDetails->port;
	server_port = players[currPlayerId].networkDetails->port;

	if (gameDetails.isHost) {
		createClientBroadcastThread();
		createUpdateServerThread();
	}
}

int i = 0;
void renderGridMainWindow() {
	if (gameDetails.isHost) {
		if (i % SPEED_ACTUAL(0) == 0) {
			moveHero(0);
		}
		if (i % SPEED_ACTUAL(1) == 0) {
			moveHero(1);
		}
		if (i % SPEED_ACTUAL(2) == 0) {
			moveHero(2);
		}
		if (i % SPEED_ACTUAL(3) == 0) {
			moveHero(3);
		}
		i++;
	}

	loadAttributeSpace();
	renderGrid();
}

//Draws the 3D scene
void drawScene_main() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective


	//printGrid();
	renderGridMainWindow();
	putTextToLAttCell(Coordinate_grid(3, 1), numToStr(server_port)); //TODO: not req here

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
		break;

	case 49: //key - '1' : select basicPower
		requestBasicPower();
		break;

	case 50: //key - '2' : select magicPower
		requestMagicPower();
		break;

	case 51: //key - '3' : curse //TODO: case may not be required later
		iAmCursed(CURSE_STUN);
		break;

	case 52: //key - '4' //TODO: may not be required later : just for testing purpose
		cout << "Printing message queue" << endl;
		printQueue(&queuePrimary);
		break;
	}
}

void processRightClick_main() {
	handleGridCharSwitch(downGrid_click, PROCESS_MOVE_RIGHT_CLICK);
}

void processLeftClick_main() {
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
	windowId_current = glutCreateWindow("Game is on!!!");
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
