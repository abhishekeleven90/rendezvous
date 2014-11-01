#ifndef GENERAL_HELPER_H
#define GENERAL_HELPER_H

#include "Globals.h"
#include "OpenGlHelper.h"
#include "FilePaths.h"
#include "FilePaths_att.h"
#include "CustomVectorStruct.h"
#include "Constants.h"
#include "AStarClass.h"
#include "Objects.h"
#include "MapNGrid.h"
#include "ActionOnGrid.h"
#include "Validations.h"

string numToStr(int num) {
	stringstream ss;
	ss << num;
	return ss.str();
}

void blockOpponentsArea() {
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_INNER_GRID_COL; c <= END_INNER_GRID_COL; c++) {
			if (isOponentCellForTeam(Coordinate_grid(r, c))) {
				putCharToGrid(r, c, BG_BLOCKED, true);
			}
		}
	}
}

void loadTeamAttributes() {

	//TODO: the below attributes shall be coming from earlier screens
	myTeam.name = TEAM_ANGELS;
	myTeam.templeHealth = HEALTH_FULL_TEMPLE;
	currPlayer.heroHealth = HEALTH_FULL_HERO;

	currPlayer.heroType = HERO_STUNNER;

	myTeam.players[1].heroType = HERO_BURSTER; //TODO : might not be one
	myTeam.players[1].heroHealth = HEALTH_FULL_HERO;

	//TODO: first player load attributes yours, plus others
	//TODO: first player may be the first player to join

	enemyTeam.name = TEAM_DEMONS;
	enemyTeam.templeHealth = HEALTH_FULL_TEMPLE;

	enemyTeam.players[0].heroType = HERO_DISABLER;
	enemyTeam.players[1].heroType = HERO_SLOWER;
	enemyTeam.players[0].heroHealth = HEALTH_FULL_HERO;
	enemyTeam.players[1].heroHealth = HEALTH_FULL_HERO;
	//TODO: load other attributes
	currPlayer.isTimerItemGlobalRunning = false;
	currPlayer.isTimerMagicSpellRunning = false;
	currPlayer.isTimerCurseRunning = false;
}

void loadPlayerSpecificAttributes() {

	//1st player:playerId=0, 2nd player:playerId=1
	//TODO: change 'playerId - 1' to playerId everywhere & related like 'which-1'
	currPlayer.astar = new AStarClass();
	currPlayer.astar->firstInitAStar();

	currPlayer.currentPowerMode = POWER_MODE_BASIC;
	currPlayer.curseType = CURSE_NONE;

	switch (currPlayer.heroType) {
	//TODO : decide & change attributes - strength,speedAttack,speedMove etc
	case HERO_STUNNER:
		currPlayer.strength = STRENGTH_H_STUNNER;
		currPlayer.speedMove = SPEED_MOVE_H_STUNNER;
		currPlayer.charType = H_STUNNER;
		break;

	case HERO_DISABLER:
		currPlayer.strength = STRENGTH_H_DISABLER;
		currPlayer.speedMove = SPEED_MOVE_H_DISABLER;
		currPlayer.charType = H_DISABLER;
		break;

	case HERO_SLOWER:
		currPlayer.strength = STRENGTH_H_SLOWER;
		currPlayer.speedMove = SPEED_MOVE_H_SLOWER;
		currPlayer.charType = H_SLOWER;
		break;

	case HERO_BURSTER:
		currPlayer.strength = STRENGTH_H_BURSTER;
		currPlayer.speedMove = SPEED_MOVE_H_BURSTER;
		currPlayer.charType = H_BURSTER;
		break;
	}

	currPlayer.location = Coordinate_grid(19, 1);

	int row = currPlayer.location.row;
	int col = currPlayer.location.col;
	putCharToGrid(row, col, currPlayer.charType, true);

	//TODO: temp, below remove
	/*
	 int row1 = 20;
	 int col1 = 3;
	 myTeam.players[1].location.row=row1;
	 myTeam.players[1].location.col=col1;
	 putCharToGrid(row1, col1, H_BURSTER, true);
	 myTeam.players[1].speedMove = SPEED_MOVE_H_BURSTER;

	 myTeam.players[1].astar = new AStarClass();
	 myTeam.players[1].astar->firstInitAStar();
	 myTeam.players[1].currentPowerMode = POWER_MODE_BASIC;
	 myTeam.players[1].charType=H_BURSTER;
	 */
	blockOpponentsArea();
}

void printGrid() {
	cout << "Printing grid" << endl;
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			handleGridCharSwitch(Coordinate_grid(r, c), PRINT_GRID);
		}
		cout << endl;
	}
}

//call this from the render function periodically
void moveHeroMine(int type) {
	Node* nodeToMove = findLocToMove(myTeam.players[type - 1].location, type);
	if (nodeToMove == NULL) {
		if (myTeam.players[type - 1].toAttackTemple)
			decreaseEnemyTempleHealth();
		return; //nothing to move
	}
	Coordinate_grid celltoMove = Coordinate_grid(nodeToMove->row,
			nodeToMove->col + ATTRIBUTE_WIDTH);
	if (isItem(celltoMove)) {
		setItemCell(celltoMove);
		takeItem();
	}

	putCharToGrid(
			myTeam.players[type - 1].location.row,
			myTeam.players[type - 1].location.col,
			initialGridChar[myTeam.players[type - 1].location.row][myTeam.players[type
					- 1].location.col + ATTRIBUTE_WIDTH], true);

	myTeam.players[type - 1].location.row = nodeToMove->row;
	myTeam.players[type - 1].location.col = nodeToMove->col;
	putCharToGrid(myTeam.players[type - 1].location.row,
			myTeam.players[type - 1].location.col,
			myTeam.players[type - 1].charType, true);
}

void putTeamTypeAndTemples() {
	putPngToLAttCell(Coordinate_grid(1, 1), texId_att_team_my, 2, 1);
	putPngToRAttCell(Coordinate_grid(1, 1), texId_att_team_enemy, 2, 1);

	switch (myTeam.name) {
	case TEAM_ANGELS:
		putPngToLAttCell(Coordinate_grid(2, 1), texId_att_team_Angels, 2, 1);
		putPngToRAttCell(Coordinate_grid(2, 1), texId_att_team_Demons, 2, 1);

		putBmpToLAttCell(Coordinate_grid(4, 1), texId_t_angels, 2, 1); //temples
		putBmpToRAttCell(Coordinate_grid(4, 1), texId_t_demons, 2, 1);
		break;

	case TEAM_DEMONS:
		putPngToLAttCell(Coordinate_grid(2, 1), texId_att_team_Demons, 2, 1);
		putPngToRAttCell(Coordinate_grid(2, 1), texId_att_team_Angels, 2, 1);

		putBmpToLAttCell(Coordinate_grid(4, 1), texId_t_demons, 2, 1); //temples
		putBmpToRAttCell(Coordinate_grid(4, 1), texId_t_angels, 2, 1);
		break;

	case TEAM_BOTH:
		cout << "---shall never occur---" << endl;
		break;
	}
}

void putMyAttributes() {
	switch (currPlayer.currentPowerMode) {
	case POWER_MODE_BASIC:
		putPngToLAttCell(Coordinate_grid(9, 1), texId_att_mBasic, 2, 1);
		break;
	case POWER_MODE_MAGIC:
		putPngToLAttCell(Coordinate_grid(9, 1), texId_att_mMagic, 2, 1);
		break;
	}

	switch (currPlayer.curseType) {
	case CURSE_NONE:
		putPngToLAttCell(Coordinate_grid(10, 1), texId_att_cNone, 2, 1);
		break;
	case CURSE_STUN:
		putPngToLAttCell(Coordinate_grid(10, 1), texId_att_cStun, 2, 1);
		break;
	case CURSE_DISABLE:
		putPngToLAttCell(Coordinate_grid(10, 1), texId_att_cDisable, 2, 1);
		break;
	case CURSE_SLOW:
		putPngToLAttCell(Coordinate_grid(10, 1), texId_att_cSlow, 2, 1);
		break;
	case CURSE_BURST:
		putPngToLAttCell(Coordinate_grid(10, 1), texId_att_cBurst, 2, 1);
		break;
	}

	//Speed
	putPngToLAttCell(Coordinate_grid(12, 1), texId_att_speed, 1, 1);
	putTextToLAttCell(Coordinate_grid(12, 2), numToStr(currPlayer.speedMove));

	//Strength
	putPngToLAttCell(Coordinate_grid(13, 1), texId_att_strength, 1, 1);
	putTextToLAttCell(Coordinate_grid(13, 2), numToStr(currPlayer.strength));
}

void putHealth() {
	putPngToLAttCell(Coordinate_grid(5, 1), texId_att_temple_health, 1, 1);
	putTextToLAttCell(Coordinate_grid(5, 2), numToStr(myTeam.templeHealth));

	putPngToRAttCell(Coordinate_grid(5, 1), texId_att_temple_health, 1, 1);
	putTextToRAttCell(Coordinate_grid(5, 2), numToStr(enemyTeam.templeHealth));

	putPngToLAttCell(Coordinate_grid(11, 1), texId_att_health, 1, 1);
	putTextToLAttCell(Coordinate_grid(11, 2), numToStr(currPlayer.heroHealth));

	putPngToLAttCell(Coordinate_grid(17, 1), texId_att_health, 1, 1);
	putTextToLAttCell(Coordinate_grid(17, 2),
			numToStr(myTeam.players[1].heroHealth)); //TODO: may not be '1'

	putPngToRAttCell(Coordinate_grid(9, 1), texId_att_health, 1, 1);
	putTextToRAttCell(Coordinate_grid(9, 2),
			numToStr(enemyTeam.players[0].heroHealth));

	putPngToRAttCell(Coordinate_grid(13, 1), texId_att_health, 1, 1);
	putTextToRAttCell(Coordinate_grid(13, 2),
			numToStr(enemyTeam.players[1].heroHealth));
}

void putHeroes() {
	switch (currPlayer.heroType) {
	case HERO_STUNNER:
		putBmpToLAttCell(Coordinate_grid(7, 1), texId_h_stunner, 1, 1);
		putPngToLAttCell(Coordinate_grid(8, 1), texId_att_h_stunner, 2, 1);
		break;

	case HERO_DISABLER:
		putBmpToLAttCell(Coordinate_grid(7, 1), texId_h_disabler, 1, 1);
		putPngToLAttCell(Coordinate_grid(8, 1), texId_att_h_disabler, 2, 1);
		break;

	case HERO_SLOWER:
		putBmpToLAttCell(Coordinate_grid(7, 1), texId_h_slower, 1, 1);
		putPngToLAttCell(Coordinate_grid(8, 1), texId_att_h_slower, 2, 1);
		break;

	case HERO_BURSTER:
		putBmpToLAttCell(Coordinate_grid(7, 1), texId_h_burster, 1, 1);
		putPngToLAttCell(Coordinate_grid(8, 1), texId_att_h_burster, 2, 1);
		break;
	}

	//TODO: may not be '1' below
	switch (myTeam.players[1].heroType) {
	case HERO_STUNNER:
		putBmpToLAttCell(Coordinate_grid(15, 1), texId_h_stunner, 1, 1);
		putPngToLAttCell(Coordinate_grid(16, 1), texId_att_h_stunner, 2, 1);
		break;

	case HERO_DISABLER:
		putBmpToLAttCell(Coordinate_grid(15, 1), texId_h_disabler, 1, 1);
		putPngToLAttCell(Coordinate_grid(16, 1), texId_att_h_disabler, 2, 1);
		break;

	case HERO_SLOWER:
		putBmpToLAttCell(Coordinate_grid(15, 1), texId_h_slower, 1, 1);
		putPngToLAttCell(Coordinate_grid(16, 1), texId_att_h_slower, 2, 1);
		break;

	case HERO_BURSTER:
		putBmpToLAttCell(Coordinate_grid(15, 1), texId_h_burster, 1, 1);
		putPngToLAttCell(Coordinate_grid(16, 1), texId_att_h_burster, 2, 1);
		break;
	}

	switch (enemyTeam.players[0].heroType) {
	case HERO_STUNNER:
		putBmpToRAttCell(Coordinate_grid(7, 1), texId_h_stunner, 1, 1);
		putPngToRAttCell(Coordinate_grid(8, 1), texId_att_h_stunner, 2, 1);
		break;

	case HERO_DISABLER:
		putBmpToRAttCell(Coordinate_grid(7, 1), texId_h_disabler, 1, 1);
		putPngToRAttCell(Coordinate_grid(8, 1), texId_att_h_disabler, 2, 1);
		break;

	case HERO_SLOWER:
		putBmpToRAttCell(Coordinate_grid(7, 1), texId_h_slower, 1, 1);
		putPngToRAttCell(Coordinate_grid(8, 1), texId_att_h_slower, 2, 1);
		break;

	case HERO_BURSTER:
		putBmpToRAttCell(Coordinate_grid(7, 1), texId_h_burster, 1, 1);
		putPngToRAttCell(Coordinate_grid(8, 1), texId_att_h_burster, 2, 1);
		break;
	}

	switch (enemyTeam.players[1].heroType) {
	case HERO_STUNNER:
		putBmpToRAttCell(Coordinate_grid(11, 1), texId_h_stunner, 1, 1);
		putPngToRAttCell(Coordinate_grid(12, 1), texId_att_h_stunner, 2, 1);
		break;

	case HERO_DISABLER:
		putBmpToRAttCell(Coordinate_grid(11, 1), texId_h_disabler, 1, 1);
		putPngToRAttCell(Coordinate_grid(12, 1), texId_att_h_disabler, 2, 1);
		break;

	case HERO_SLOWER:
		putBmpToRAttCell(Coordinate_grid(11, 1), texId_h_slower, 1, 1);
		putPngToRAttCell(Coordinate_grid(12, 1), texId_att_h_slower, 2, 1);
		break;

	case HERO_BURSTER:
		putBmpToRAttCell(Coordinate_grid(11, 1), texId_h_burster, 1, 1);
		putPngToRAttCell(Coordinate_grid(12, 1), texId_att_h_burster, 2, 1);
		break;
	}
}

void putTimers() {
	if (currPlayer.isTimerItemGlobalRunning) {
		putPngToLAttCell(Coordinate_grid(19, 1), texId_att_time_itemOn, 2, 1);
	} else {
		putPngToLAttCell(Coordinate_grid(19, 1), texId_att_time_itemOff, 2, 1);
	}

	if (currPlayer.isTimerMagicSpellRunning) {
		putPngToLAttCell(Coordinate_grid(20, 1), texId_att_time_magicOn, 2, 1);
	} else {
		putPngToLAttCell(Coordinate_grid(20, 1), texId_att_time_magicOff, 2, 1);
	}
}

void putWall() {
	putPngToRAttCell(Coordinate_grid(17, 1), texId_att_wall, 2, 1);
}

void loadAttributeSpace() {
	putTeamTypeAndTemples();

	putHeroes();
	putMyAttributes();

	putHealth();

	putTimers();
	putWall();
}

void renderGrid() {
	moveHeroMine(1);
	moveHeroMine(2); //TODO: check if not required..later may be required

	loadAttributeSpace();

	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			handleGridCharSwitch(Coordinate_grid(r, c), RENDER_GRID);
		}
	}
}

void iAmCursed(curse curseType) {
	switch (curseType) {
	case CURSE_STUN:
		cout << "stunned" << endl;
		timerCurse(CURSE_STUN);
		break;

	case CURSE_DISABLE:
		cout << "disabled" << endl;
		currPlayer.currentPowerMode = POWER_MODE_BASIC;
		timerCurse(CURSE_DISABLE);
		break;

	case CURSE_SLOW:
		cout << "slowed" << endl;
		timerCurse(CURSE_SLOW);
		break;

	case CURSE_BURST:
		cout << "bursted" << endl;
		currPlayer.heroHealth -= CURSE_AMT_BURST_DAMAGE;
		break;

	case CURSE_NONE:
		cout << "---Inside cursePlayer: shall not come here---" << endl;
		return;
	}

	currPlayer.curseType = curseType;
}

void togglePlayer() { //TODO: check if required
	if (playerId == 1)
		playerId = 2;
	else
		playerId = 1;
}
#endif
