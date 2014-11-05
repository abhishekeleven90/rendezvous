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
	//myTeam.name = TEAM_ANGELS;
	myTeam.templeHealth = HEALTH_FULL_TEMPLE;
	players[playerId].heroHealth = HEALTH_FULL_HERO;

	players[playerId].heroType = HERO_DISABLER;

	myTeam.players[1].heroType = HERO_STUNNER; //TODO : might not be one
	myTeam.players[1].heroHealth = HEALTH_FULL_HERO;

	//TODO: first player load attributes yours, plus others
	//TODO: first player may be the first player to join

	enemyTeam.name = TEAM_DEMONS;
	enemyTeam.templeHealth = HEALTH_FULL_TEMPLE;

	//enemyTeam.players[0].heroType = HERO_SLOWER;
	//enemyTeam.players[1].heroType = HERO_BURSTER;
	//TODO: load other attributes
	players[playerId].isTimerItemGlobalRunning = false;
	players[playerId].isTimerMagicSpellRunning = false;
	players[playerId].isTimerCurseRunning = false;

	//myTeam.players[1].location = Coordinate_grid(20, 3);
}

void loadPlayerSpecificAttributes(int playerId) {

	players[playerId].astar = new AStarClass();
	players[playerId].astar->firstInitAStar();

	players[playerId].currentPowerMode = POWER_MODE_BASIC;
	players[playerId].curseType = CURSE_NONE;
	players[playerId].heroHealth = HEALTH_FULL_HERO;

	switch (players[playerId].heroType) {
	case HERO_STUNNER:
		players[playerId].strength = STRENGTH_H_STUNNER;
		players[playerId].speedMove = SPEED_MOVE_H_STUNNER;
		players[playerId].charType = H_STUNNER;
		break;

	case HERO_DISABLER:
		players[playerId].strength = STRENGTH_H_DISABLER;
		players[playerId].speedMove = SPEED_MOVE_H_DISABLER;
		players[playerId].charType = H_DISABLER;
		break;

	case HERO_SLOWER:
		players[playerId].strength = STRENGTH_H_SLOWER;
		players[playerId].speedMove = SPEED_MOVE_H_SLOWER;
		players[playerId].charType = H_SLOWER;
		break;

	case HERO_BURSTER:
		players[playerId].strength = STRENGTH_H_BURSTER;
		players[playerId].speedMove = SPEED_MOVE_H_BURSTER;
		players[playerId].charType = H_BURSTER;
		break;
	}

	if(players[playerId].team->name == TEAM_ANGELS && playerId%2==0){

	}

	int row = players[playerId].location.row;
	int col = players[playerId].location.col;
	putCharToGrid(row, col, players[playerId].charType, true);
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
	switch (players[playerId].currentPowerMode) {
	case POWER_MODE_BASIC:
		putPngToLAttCell(Coordinate_grid(9, 1), texId_att_mBasic, 2, 1);
		break;
	case POWER_MODE_MAGIC:
		putPngToLAttCell(Coordinate_grid(9, 1), texId_att_mMagic, 2, 1);
		break;
	case POWER_MODE_STUN:
		putPngToLAttCell(Coordinate_grid(9, 1), texId_att_mStun, 2, 1);
		break;
	}

	switch (players[playerId].curseType) {
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
	putTextToLAttCell(Coordinate_grid(12, 2), numToStr(players[playerId].speedMove));

	//Strength
	putPngToLAttCell(Coordinate_grid(13, 1), texId_att_strength, 1, 1);
	putTextToLAttCell(Coordinate_grid(13, 2), numToStr(players[playerId].strength));
}

void putHealth() {
	putPngToLAttCell(Coordinate_grid(5, 1), texId_att_temple_health, 1, 1);
	putTextToLAttCell(Coordinate_grid(5, 2), numToStr(myTeam.templeHealth));

	putPngToRAttCell(Coordinate_grid(5, 1), texId_att_temple_health, 1, 1);
	putTextToRAttCell(Coordinate_grid(5, 2), numToStr(enemyTeam.templeHealth));

	putPngToLAttCell(Coordinate_grid(11, 1), texId_att_health, 1, 1);
	putTextToLAttCell(Coordinate_grid(11, 2), numToStr(players[playerId].heroHealth));

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
	switch (players[playerId].heroType) {
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
	if (players[playerId].isTimerItemGlobalRunning) {
		putPngToLAttCell(Coordinate_grid(19, 1), texId_att_time_itemOn, 2, 1);
	} else {
		putPngToLAttCell(Coordinate_grid(19, 1), texId_att_time_itemOff, 2, 1);
	}

	if (players[playerId].isTimerMagicSpellRunning) {
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
		players[playerId].currentPowerMode = POWER_MODE_BASIC;
		timerCurse(CURSE_DISABLE);
		break;

	case CURSE_SLOW:
		cout << "slowed" << endl;
		timerCurse(CURSE_SLOW);
		break;

	case CURSE_BURST:
		cout << "bursted" << endl;
		players[playerId].heroHealth -= CURSE_AMT_BURST_DAMAGE;
		break;

	case CURSE_NONE:
		cout << "---Inside cursePlayer: shall not come here---" << endl;
		return;
	}

	players[playerId].curseType = curseType;
}

void togglePlayer() { //TODO: check if required
	if (playerId == 1)
		playerId = 2;
	else
		playerId = 1;
}
#endif
