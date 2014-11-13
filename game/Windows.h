#ifndef WINDOWS_H
#define WINDOWS_H

#include "text3d.h"
#include "Network.h"
#include "AI.h"

void moveToWindow(void f()) {
	glutDestroyWindow(windowId_current);
	t3dCleanup();
	f();
}

void create_window_final();
void create_window_main();
void create_window_joiningGame();
void create_window_waiting();
void create_window_selectHero();
void create_window_selectTeam();
void create_window_multiplayer();
void create_window_rules();
void create_window_players();
void create_window_controls();
void create_window_about();
void create_window_first();

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_FIRST--------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_first() {
	texId_bg = getTextureFromPng(PATH_IMG_BG);
	texId_logo = getTextureFromPng(PATH_IMG_LOGO);

	texId_icon_about = getTextureFromPng(PATH_IMG_ICON_ABOUT);
	texId_icon_controls = getTextureFromPng(PATH_IMG_ICON_CONTROLS);
	texId_icon_players = getTextureFromPng(PATH_IMG_ICON_PLAYERS);
	texId_icon_rules = getTextureFromPng(PATH_IMG_ICON_RULES);

	texId_player_single = getTextureFromPng(PATH_IMG_PLAYER_SINGLE);
	texId_player_multi = getTextureFromPng(PATH_IMG_PLAYER_MULTI);
}

void putImages_first() {
	putPngToCell(Coordinate_grid(5, 6), texId_logo, 14, 4);

	putPngWithChar(18, 6, texId_icon_about, CLICK_ABOUT, 2, 2);
	putPngWithChar(18, 10, texId_icon_controls, CLICK_CONTROLS, 2, 2);
	putPngWithChar(18, 14, texId_icon_players, CLICK_PLAYERS, 2, 2);
	putPngWithChar(18, 18, texId_icon_rules, CLICK_RULES, 2, 2);

	putPngWithChar(12, 7, texId_player_single, CLICK_PLAYER_SINGLE, 4, 1);
	putPngWithChar(12, 15, texId_player_multi, CLICK_PLAYER_MULTI, 4, 1);

	putPngToCell(Coordinate_grid(20, 1), texId_bg, 24, 20);
}

//Initializes 3D rendering
void initRendering_first() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	angelsTeam.name = TEAM_ANGELS;
	demonsTeam.name = TEAM_DEMONS;

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

void processLeftClick_first() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_ABOUT:
		moveToWindow(create_window_about);
		break;
	case CLICK_CONTROLS:
		moveToWindow(create_window_controls);
		break;
	case CLICK_PLAYERS:
		moveToWindow(create_window_players);
		break;
	case CLICK_RULES:
		moveToWindow(create_window_rules);
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

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
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
 * ------------------------------------------------------------------WINDOW_ABOUT---------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_about() {
	texId_page_about = getTextureFromPng(PATH_IMG_PAGE_ABOUT);

	texId_btn_back = getTextureFromPng(PATH_IMG_BACK);
}

void putImages_about() {
	putPngWithChar(19, 12, texId_btn_back, CLICK_BACK, 2, 2);

	putPngToCell(Coordinate_grid(20, 1), texId_page_about, 24, 20);
}

//Initializes 3D rendering
void initRendering_about() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_about();
	t3dInit();
}

//Draws the 3D scene
void drawScene_about() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putImages_about();
	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_about(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void processLeftClick_about() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_BACK:
		moveToWindow(create_window_first);
		break;
	}
}

void myMouseClickHandler_about(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
		processLeftClick_about();
	}
}

void create_window_about() {
	windowId_current = glutCreateWindow("About Game");
	initRendering_about(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_about);
	glutKeyboardFunc(handleKeypress_about);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_about);

	//timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_CONTROLS---------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_controls() {
	texId_page_controls = getTextureFromPng(PATH_IMG_PAGE_CONTROLS);

	texId_btn_back = getTextureFromPng(PATH_IMG_BACK);
}

void putImages_controls() {
	putPngWithChar(19, 12, texId_btn_back, CLICK_BACK, 2, 2);

	putPngToCell(Coordinate_grid(20, 1), texId_page_controls, 24, 20);
}

//Initializes 3D rendering
void initRendering_controls() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_controls();
	t3dInit();
}

//Draws the 3D scene
void drawScene_controls() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putImages_controls();
	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_controls(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void processLeftClick_controls() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_BACK:
		moveToWindow(create_window_first);
		break;
	}
}

void myMouseClickHandler_controls(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
		processLeftClick_controls();
	}
}

void create_window_controls() {
	windowId_current = glutCreateWindow("Controls");
	initRendering_controls(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_controls);
	glutKeyboardFunc(handleKeypress_controls);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_controls);

	//timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_PLAYERS---------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_players() {
	texId_page_players = getTextureFromPng(PATH_IMG_PAGE_PLAYERS);

	texId_btn_back = getTextureFromPng(PATH_IMG_BACK);
}

void putImages_players() {
	putPngWithChar(19, 12, texId_btn_back, CLICK_BACK, 2, 2);

	putPngToCell(Coordinate_grid(20, 1), texId_page_players, 24, 20);
}

//Initializes 3D rendering
void initRendering_players() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_players();
	t3dInit();
}

//Draws the 3D scene
void drawScene_players() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putImages_players();
	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_players(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void processLeftClick_players() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_BACK:
		moveToWindow(create_window_first);
		break;
	}
}

void myMouseClickHandler_players(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
		processLeftClick_players();
	}
}

void create_window_players() {
	windowId_current = glutCreateWindow("Players");
	initRendering_players(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_players);
	glutKeyboardFunc(handleKeypress_players);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_players);

	//timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_RULES---------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_rules() {
	texId_page_rules = getTextureFromPng(PATH_IMG_PAGE_RULES);

	texId_btn_back = getTextureFromPng(PATH_IMG_BACK);
}

void putImages_rules() {
	putPngWithChar(19, 12, texId_btn_back, CLICK_BACK, 2, 2);

	putPngToCell(Coordinate_grid(20, 1), texId_page_rules, 24, 20);
}

//Initializes 3D rendering
void initRendering_rules() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_rules();
	t3dInit();
}

//Draws the 3D scene
void drawScene_rules() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putImages_rules();
	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_rules(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void processLeftClick_rules() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_BACK:
		moveToWindow(create_window_first);
		break;
	}
}

void myMouseClickHandler_rules(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
		processLeftClick_rules();
	}
}

void create_window_rules() {
	windowId_current = glutCreateWindow("Rules");
	initRendering_rules(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_rules);
	glutKeyboardFunc(handleKeypress_rules);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_rules);

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

	texId_join_game = getTextureFromPng(PATH_IMG_JOIN_GAME);
	texId_host_game = getTextureFromPng(PATH_IMG_HOST_GAME);

	texId_white = getTextureFromPng(PATH_IMG_WHITE);
	texId_btn_next = getTextureFromPng(PATH_IMG_BTN_NEXT);
	texId_try_again = getTextureFromPng(PATH_IMG_TRY_AGAIN);
	texId_btn_reset = getTextureFromPng(PATH_IMG_BTN_RESET);
}

void putImages_multiplayer() {
	putPngWithChar(7, 6, texId_host_game, CLICK_HOST_GAME, 4, 2);
	putPngWithChar(7, 16, texId_join_game, CLICK_JOIN_GAME, 4, 2);

	if (isShowInput) {
		putTextToCell(Coordinate_grid(9, 16), hostIp);

		//Important - keep the "coordinates & blocks" same here & in below(else) part
		putPngWithChar(9, 16, texId_input, INPUT, 4, 1);
		putPngWithChar(9, 20, texId_btn_reset, CLICK_RESET, 1, 1);

		putPngWithChar(9, 22, texId_btn_next, CLICK_NEXT, 1, 1);

		if (gameDetails.isIssueConnectingToServer) {
			putPngToCell(Coordinate_grid(11, 16), texId_try_again, 4, 1);
		}
	} else {
		hostIp = "";
		//below is imp to avoid clicks if the button is not visible
		putChars(4, 1, 9, 16, BG_GRASS); //input button
		putChars(1, 1, 9, 20, BG_GRASS); //input button
		putChars(1, 1, 9, 22, BG_GRASS); //next button
		putChars(4, 1, 11, 16, BG_GRASS); //tryAgain button
	}

	putPngToCell(Coordinate_grid(20, 1), texId_bg, 24, 20);
}

//Initializes 3D rendering
void initRendering_multiplayer() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gameDetails.isIssueConnectingToServer = false;
	isShowInput = false;

	loadTextures_multiplayer();
	createServerThread();
	gameDetails.isTimerNotHostWaiting = true; //making this true over here because we don't want any client to connect

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

void processClickNext() {
	if (hostIp == "") {
		return;
	}

	gameDetails.hostDetails = convertToNodeHelper(str2Char(hostIp));

	connectStatus connect = helperSendConnect();
	switch (connect) {
	case CONNECTED_NOT:
		break;
	case CONNECTED_ALREADY:
		moveToWindow(create_window_joiningGame);
		break;
	case CONNECTED_NEW:
		moveToWindow(create_window_selectTeam);
		break;
	}
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
	case 8://'backspace'
		hostIp = remLastCharFromStr(hostIp);
		break;
	case 13://'enter'
		processClickNext();
		break;
	}
}

void processClickHost() {
	isShowInput = false;
	gameDetails.isHost = true;

	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		players[i].status = STATUS_NOT_JOINED;
	}

	gameDetails.isIssueConnectingToServer = false;
	moveToWindow(create_window_selectTeam);
}

void processLeftClick_multiplayer() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_JOIN_GAME:
		gameDetails.isHost = false;
		isShowInput = true;
		break;

	case CLICK_HOST_GAME:
		processClickHost();
		break;

	case CLICK_NEXT:
		processClickNext();
		break;

	case CLICK_RESET:
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

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
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
 * ------------------------------------------------------------------WINDOW_SELECT_TEAM--------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
bool isTeamVisible[2];

void loadTextures_selectTeam() {
	texId_bg = getTextureFromPng(PATH_IMG_BG);

	texId_pick_team = getTextureFromPng(PATH_IMG_PICK_TEAM);

	texId_att_team_angels = getTextureFromPng(PATH_IMG_ATT_TEAM_ANGELS);
	texId_att_team_demons = getTextureFromPng(PATH_IMG_ATT_TEAM_DEMONS);

	texId_t_angels = getTextureFromBmp(PATH_IMG_TEMPLE_ANGELS);
	texId_t_demons = getTextureFromBmp(PATH_IMG_TEMPLE_DEMONS);
}

void putImages_selectTeam() {
	putPngWithChar(5, 8, texId_pick_team, CLICK_PICK_TEAM, 9, 2);

	if (isTeamVisible[TEAM_ANGELS]) {
		putBmpWithChar(11, 6, texId_t_angels, CLICK_ANGELS, 4, 4);
		putPngWithChar(13, 6, texId_att_team_angels, CLICK_ANGELS, 4, 2);
	} else {
		putChars(4, 4, 11, 6, BG_GRASS);
		putChars(4, 2, 13, 6, BG_GRASS);
	}

	if (isTeamVisible[TEAM_DEMONS]) {
		putBmpWithChar(11, 16, texId_t_demons, CLICK_DEMONS, 4, 4);
		putPngWithChar(13, 16, texId_att_team_demons, CLICK_DEMONS, 4, 2);
	} else {
		putChars(4, 4, 11, 16, BG_GRASS);
		putChars(4, 2, 13, 16, BG_GRASS);
	}

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

	isTeamVisible[TEAM_ANGELS] = true;
	isTeamVisible[TEAM_DEMONS] = true;

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

void teamSelected_next(TeamStruct* team) {
	//------------------------------------Change for single player (start)----------
	if (gameDetails.isSinglePlayerGame) {
		gameDetails.isHost = true;

		players[PLAYER_ID_PRIMARY].team = team;
		cout << "set primary team as: " << team->name << endl;
		moveToWindow(create_window_selectHero);
	}
	//------------------------------------Change for single player (end)----------

	if (gameDetails.isHost) {
		players[PLAYER_ID_PRIMARY].team = team; //setting teamName in case of host only
		moveToWindow(create_window_selectHero);
	}

	else {
		if (helperValidateTeam(team->name)) {
			moveToWindow(create_window_selectHero);
		}

		else {
			isTeamVisible[team->name] = false;
		}
	}
}

void processLeftClick_selectTeam() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_ANGELS:
		teamSelected_next(&angelsTeam);
		break;
	case CLICK_DEMONS:
		teamSelected_next(&demonsTeam);
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

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
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
 * ------------------------------------------------------------------WINDOW_SELECT_HERO--------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
bool isHeroVisible[4];

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
	if (isHeroVisible[HERO_SLOWER]) {
		putBmpWithChar(9, 6, texId_h_slower, H_SLOWER, 4, 4);
		putPngWithChar(10, 6, texId_att_h_slower, H_SLOWER, 4, 1);
	} else {
		putChars(4, 4, 9, 6, BG_GRASS);
		putChars(4, 1, 10, 6, BG_GRASS);
	}

	if (isHeroVisible[HERO_BURSTER]) {
		putBmpWithChar(9, 16, texId_h_burster, H_BURSTER, 4, 4);
		putPngWithChar(10, 16, texId_att_h_burster, H_BURSTER, 4, 1);
	} else {
		putChars(4, 4, 9, 16, BG_GRASS);
		putChars(4, 1, 10, 16, BG_GRASS);
	}

	if (isHeroVisible[HERO_STUNNER]) {
		putBmpWithChar(17, 6, texId_h_stunner, H_STUNNER, 4, 4);
		putPngWithChar(18, 6, texId_att_h_stunner, H_STUNNER, 4, 1);
	} else {
		putChars(4, 4, 17, 6, BG_GRASS);
		putChars(4, 1, 18, 6, BG_GRASS);
	}

	if (isHeroVisible[HERO_DISABLER]) {
		putBmpWithChar(17, 16, texId_h_disabler, H_DISABLER, 4, 4);
		putPngWithChar(18, 16, texId_att_h_disabler, H_DISABLER, 4, 1);
	} else {
		putChars(4, 4, 17, 16, BG_GRASS);
		putChars(4, 1, 18, 16, BG_GRASS);
	}

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

	isHeroVisible[HERO_STUNNER] = true;
	isHeroVisible[HERO_SLOWER] = true;
	isHeroVisible[HERO_BURSTER] = true;
	isHeroVisible[HERO_DISABLER] = true;

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
	}
}

void heroSelected_next(heroes hero) {

	//------------------------------------Change for single player (start)----------
	if (gameDetails.isSinglePlayerGame) {
		players[PLAYER_ID_PRIMARY].heroType = hero;
		cout << "set primary hero as: " << hero << endl;
		moveToWindow(create_window_joiningGame);
	}
	//------------------------------------Change for single player (end)----------


	if (gameDetails.isHost) {
		players[PLAYER_ID_PRIMARY].heroType = hero; //setting hero in case of host only
		moveToWindow(create_window_waiting);
	}

	else {
		if (helperValidateHero(hero)) {
			moveToWindow(create_window_joiningGame);
		}

		else {
			isHeroVisible[hero] = false;
		}
	}
}

void processLeftClick_selectHero() {
	switch (getGridChar(downGrid_click)) {
	case H_SLOWER:
		heroSelected_next(HERO_SLOWER);
		break;
	case H_STUNNER:
		heroSelected_next(HERO_STUNNER);
		break;
	case H_DISABLER:
		heroSelected_next(HERO_DISABLER);
		break;
	case H_BURSTER:
		heroSelected_next(HERO_BURSTER);
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

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
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

	gameDetails.isTimerNotHostWaiting = false;

	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		//No player has joined yet
		players[i].status = STATUS_NOT_JOINED;
	}

	//setting some details here since else not set anywhere ;)
	currPlayerId = PLAYER_ID_PRIMARY;
	players[currPlayerId].networkDetails = selfNode;
	gameDetails.hostDetails = selfNode;
	players[currPlayerId].status = STATUS_PRESENT;
	players[currPlayerId].isFirstPlayerInTeam = true;

	//Initializing the herotypes to default
	for (int i = 1; i < NUM_OF_PLAYERS; i++) {
		players[i].heroType = HERO_NOT_PRESENT;
	}

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

	if (gameDetails.isTimerNotHostWaiting) {
		supportBroadCast(BROADCAST_JOINING);
		moveToWindow(create_window_joiningGame);
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

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
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
 * -----------------------------------------------------------WINDOW_JOINING_GAME---------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_joiningGame() {
	texId_page_join = getTextureFromPng(PATH_IMG_PAGE_JOIN);
}

void putImages_joiningGame() {
	putPngToCell(Coordinate_grid(20, 1), texId_page_join, 24, 20);
}

void setAttributes() {
	initMap();

	//Placing items in the map
	for (int i = 0; i < ITEMS_ON_MAP_COUNT; i++) {
		initItemAtRandomPos();
	}

	loadTeamAttributes();
	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		if (players[i].status == STATUS_PRESENT) {
			loadPlayerGeneralAttributes(i);
		}
	}
	copyPrimaryGrid();

	blockOpponentsArea();
}

void processJoinForSinglePlayer() {
	currPlayerId = PLAYER_ID_PRIMARY;
	players[currPlayerId].networkDetails = selfNode;
	gameDetails.hostDetails = selfNode;

	//--selecting teams
	players[1].team = players[0].team; //my friend

	if (players[0].team->name == TEAM_ANGELS) {
		players[2].team = &demonsTeam;
		players[3].team = &demonsTeam;
	} else {
		players[2].team = &angelsTeam;
		players[3].team = &angelsTeam;
	}

	//--selecting heroes
	int k = 1;
	for (int i = 0; i < 4; i++) {
		if (players[0].heroType != i) {
			players[k++].heroType = static_cast<heroes> (i);
		}
	}

	//---setting is firstPlayerInTeam
	players[0].isFirstPlayerInTeam = true;
	players[1].isFirstPlayerInTeam = false;
	players[2].isFirstPlayerInTeam = true;
	players[3].isFirstPlayerInTeam = false;

	//---all are present
	players[0].status = STATUS_PRESENT;
	players[1].status = STATUS_PRESENT;
	players[2].status = STATUS_PRESENT;
	players[3].status = STATUS_PRESENT;

	setAttributes();
}

//Initializes 3D rendering
void initRendering_joiningGame() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	loadTextures_joiningGame();
	//------------------------------------Change for single player (start)----------
	if (gameDetails.isSinglePlayerGame) {
		processJoinForSinglePlayer();
	}
	//------------------------------------Change for single player (end)----------
	gameDetails.isStartJoiningTimer = false;
	gameDetails.isDoneWithJoining = false;
	t3dInit();
}

bool isCalledJoiningFunctions = false;
bool isFineToCallJoiningFunctions() {
	if (isCalledJoiningFunctions) {
		return false;
	}
	if (gameDetails.isHost) {
		isCalledJoiningFunctions = true;
		return true;
	}
	if (gameDetails.isStartJoiningTimer) {
		isCalledJoiningFunctions = true;
		return true;
	}
	return false;
}

//Draws the 3D scene
void drawScene_joiningGame() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putImages_joiningGame();

	//------------------------------------Change for single player (start)----------
	if (gameDetails.isSinglePlayerGame) {
		moveToWindow(create_window_main);
	}
	//------------------------------------Change for single player (end)----------
	if (isFineToCallJoiningFunctions()) {
		timerPageCreatingGame(0);
		helperRequestPlayersDetails();
		setAttributes();
		if (gameDetails.isHost) {
			createClientBroadcastThread();
		}
	}

	if (gameDetails.isDoneWithJoining) {
		moveToWindow(create_window_main);
	}

	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_joiningGame(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void processLeftClick_joiningGame() {

}

void myMouseClickHandler_joiningGame(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
		processLeftClick_joiningGame();
	}
}

void create_window_joiningGame() {
	windowId_current = glutCreateWindow("Joining Game!!!");
	initRendering_joiningGame(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_joiningGame);
	glutKeyboardFunc(handleKeypress_joiningGame);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_joiningGame);

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

	t3dInit();

	gameDetails.isGameOver = false;
	if (gameDetails.isHost) {
		isGameOver = false;
		//createClientBroadcastThread(); //Created in joining page
		createUpdateServerThread();
		createRectifyItemsThread();
	}

	//------------------------------------Change for single player (start)----------
	if (gameDetails.isSinglePlayerGame) {
		//Creating AI threads
		ai1 = new AI(1, players[1].location, AI_NORMAL);
		ai2 = new AI(2, players[2].location, AI_NORMAL);
		ai3 = new AI(3, players[3].location, AI_OFFENSIVE);

		gameDetails.isBotsPaused = false;

		createAIThread1();
		createAIThread2();
		createAIThread3();
	}
	//------------------------------------Change for single player (end)----------
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

	if (gameDetails.isGameOver) {
		moveToWindow(create_window_final);
	}

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

		//------------------------------------Change for single player (start)----------
	case 112: //key - 'p' : pause game
		if (gameDetails.isSinglePlayerGame) {
			if (gameDetails.isBotsPaused) {
				cout << "Resuming bots" << endl;
				gameDetails.isBotsPaused = false;
			} else {
				cout << "Pausing bots" << endl;
				gameDetails.isBotsPaused = true;
			}
		}
		break;

	case 113: //key - 'q'
		if (gameDetails.isSinglePlayerGame) {
			if (ai1->modeAi == AI_NORMAL) {
				cout << "Changing AI mode to Offensive" << endl;
				ai1->modeAi = AI_OFFENSIVE;
			}

			else if (ai1->modeAi == AI_OFFENSIVE) {
				cout << "Changing AI mode to Normal" << endl;
				ai1->modeAi = AI_NORMAL;
			}
		}
		break;

		//------------------------------------Change for single player (end)----------

	case 49: //key - '1' : select basicPower
		//------------------------------------Change for single player (start)----------
		if (gameDetails.isSinglePlayerGame) {
			requestBasicPowerAI(currPlayerId);
		}
		//------------------------------------Change for single player (end)----------

		else {
			requestBasicPower();
		}
		break;

	case 50: //key - '2' : select magicPower
		//------------------------------------Change for single player (start)----------
		if (gameDetails.isSinglePlayerGame) {
			requestMagicPowerAI(currPlayerId);
		}
		//------------------------------------Change for single player (end)----------

		else {
			requestMagicPower();
		}

		break;

	case 52: //key - '4' //for testing purpose
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

	if (button == GLUT_RIGHT_BUTTON) { //reaches here only if GLUT_UP
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

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_FINAL--------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_final() {
	texId_page_win_angels = getTextureFromPng(PATH_IMG_PAGE_WIN_ANGELS);
	texId_page_win_demons = getTextureFromPng(PATH_IMG_PAGE_WIN_DEMONS);
}

void putImages_final() {
	if (gameDetails.winningTeam == TEAM_ANGELS) {
		putPngToCell(Coordinate_grid(20, 1), texId_page_win_angels, 24, 20);
	} else if (gameDetails.winningTeam == TEAM_DEMONS) {
		putPngToCell(Coordinate_grid(20, 1), texId_page_win_demons, 24, 20);
	}
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

	putImages_final();
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

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
		processLeftClick_final();
	}
}

void create_window_final() {
	windowId_current = glutCreateWindow("Game Over, press 'esc' to exit");
	initRendering_final(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_final);
	glutKeyboardFunc(handleKeypress_final);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_final);

	//timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}
