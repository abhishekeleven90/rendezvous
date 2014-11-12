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
#include "Network.h"

bool isSameLocation(Coordinate_grid one, Coordinate_grid two) {
	if (one.row == two.row && one.col == two.col)
		return true;
	else
		return false;
}

void ifSpawnLocationFullHealth(int whichPlayer) {
	int k = 1;
	for (int i = END_GRID_ROW - SPAWN_BLOCKS + 1; i <= END_GRID_ROW; i++) {
		for (int j = 1; j <= k; j++) {
			bool oneSide = isSameLocation(Coordinate_grid(i, j),
					players[whichPlayer].location);
			bool otherSide = isSameLocation(Coordinate_grid(j, i),
					players[whichPlayer].location);
			if (oneSide || otherSide) {
				if (players[whichPlayer].heroHealth < HEALTH_FULL_HERO) {
					players[whichPlayer].heroHealth = HEALTH_FULL_HERO;
					return;
				}
			}
		}
		k++;
	}
}

//to be called by server
bool isNearEnemyTempleArea(int whichPlayer) {
	Coordinate_grid pLoc = players[whichPlayer].location;
	charCellType neighbours[4];
	neighbours[0] = getGridChar(pLoc.row + 1, pLoc.col, true, true);
	neighbours[1] = getGridChar(pLoc.row, pLoc.col + 1, true, true);
	neighbours[2] = getGridChar(pLoc.row - 1, pLoc.col, true, true);
	neighbours[3] = getGridChar(pLoc.row, pLoc.col - 1, true, true);

	charCellType enemyTemple, enemyTempleBack;

	if (players[whichPlayer].team->name == TEAM_ANGELS) {
		enemyTemple = TEMPLE_DEMONS;
		enemyTempleBack = T_DEMONS_BACK;
	} else {
		enemyTemple = TEMPLE_ANGELS;
		enemyTempleBack = T_ANGELS_BACK;
	}

	for (int i = 0; i <= 3; i++) {
		if (neighbours[i] == enemyTemple || neighbours[i] == enemyTempleBack) {
			return true;
		}
	}
	return false;

}

//to be called by server
bool isNearEnemyPlayerArea(int whichPlayer) {
	int enemyPlayer = players[whichPlayer].whichEnemyPlayerToAttack;
	Coordinate_grid myLoc = players[whichPlayer].location;
	Coordinate_grid enemyLoc = players[enemyPlayer].location;

	Coordinate_grid myNeighbours[4];
	myNeighbours[0] = Coordinate_grid(myLoc.row + 1, myLoc.col);
	myNeighbours[1] = Coordinate_grid(myLoc.row, myLoc.col + 1);
	myNeighbours[2] = Coordinate_grid(myLoc.row - 1, myLoc.col);
	myNeighbours[3] = Coordinate_grid(myLoc.row, myLoc.col - 1);

	for (int i = 0; i <= 3; i++) {
		if (isSameLocation(enemyLoc, myNeighbours[i])) {
			return true;
		}
	}
	return false;
}

//------------------------------------Change for single player (start)----------
Coordinate_grid isNearEnemyPlayerAreaAI(int botAI, int enemyPlayer) {

	Coordinate_grid myLoc = players[botAI].location;
	Coordinate_grid enemyLoc = players[enemyPlayer].location;

	Coordinate_grid myNeighbours[4];
	myNeighbours[0] = Coordinate_grid(myLoc.row + 1, myLoc.col);
	myNeighbours[1] = Coordinate_grid(myLoc.row, myLoc.col + 1);
	myNeighbours[2] = Coordinate_grid(myLoc.row - 1, myLoc.col);
	myNeighbours[3] = Coordinate_grid(myLoc.row, myLoc.col - 1);

	for (int i = 0; i <= 3; i++) {
		if (isSameLocation(enemyLoc, myNeighbours[i])) {
			return Coordinate_grid(enemyLoc.row, enemyLoc.col + ATTRIBUTE_WIDTH);
		}
	}
	return Coordinate_grid(-1, -1);
}
//------------------------------------Change for single player (end)----------

void blockOpponentsArea() {
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_INNER_GRID_COL; c <= END_INNER_GRID_COL; c++) {
			if (isOponentCellForTeam(Coordinate_grid(r, c), currPlayerId)) {
				putChar2Grid(r, c, BG_BLOCKED, true, false);

			}
		}
	}
}

void loadTeamAttributes() {
	angelsTeam.templeHealth = HEALTH_FULL_TEMPLE;
	demonsTeam.templeHealth = HEALTH_FULL_TEMPLE;
}

void loadPlayerGeneralAttributes(int playerId) {
	players[playerId].isTimerItemGlobalRunning = false;
	players[playerId].isTimerMagicSpellRunning = false;
	players[playerId].isTimerCurseRunning = false;
	players[playerId].atleastOnceAstar = false;
	players[playerId].toAttackTemple = false;
	players[playerId].whichEnemyPlayerToAttack = -1;

	players[playerId].astar = new AStarClass();
	players[playerId].astar->firstInitAStar();

	players[playerId].currPowerMode = POWER_MODE_BASIC;
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

	//setting player starting locations
	if (players[playerId].team->name == TEAM_ANGELS) {
		if (players[playerId].isFirstPlayerInTeam) {
			players[playerId].location = Coordinate_grid(19, 1);
		}

		else {
			players[playerId].location = Coordinate_grid(20, 2);
		}
	}

	if (players[playerId].team->name == TEAM_DEMONS) {
		if (players[playerId].isFirstPlayerInTeam) {
			players[playerId].location = Coordinate_grid(1, 19);
		}

		else {
			players[playerId].location = Coordinate_grid(2, 20);
		}
	}

	int row = players[playerId].location.row;
	int col = players[playerId].location.col;
	putChar2Grid(row, col, players[playerId].charType, true, false);

	//setting myFriend & enemyIds
	teamName myTeam = players[playerId].team->name;
	int k = 0; //used to keep track of enemy index

	players[playerId].idFriend = -1; //Setting default ids
	players[playerId].idEnemy[0] = -1;
	players[playerId].idEnemy[1] = -1;

	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		if (i == playerId || players[i].status == STATUS_NOT_JOINED) {
			continue;
		}

		if (isMyFriend(myTeam, i)) {
			players[playerId].idFriend = i;
		}

		else if (isNotMyFriend(myTeam, i)) {
			players[playerId].idEnemy[k++] = i;
		}
	}
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
//on the master node, rest will be communicated to others
void moveHero(int whichPlayer) {

	if (!players[whichPlayer].atleastOnceAstar)
		return;

	//Collision Detection patch
	aStarMove(whichPlayer, players[whichPlayer].aStarThrough);

	Node* nodeToMove =
			findLocToMove(players[whichPlayer].location, whichPlayer);
	if (nodeToMove == NULL) {
		if (players[whichPlayer].toAttackTemple) {
			if (isNearEnemyTempleArea(whichPlayer))
				decreaseEnemyTempleHealth(whichPlayer);
			else {
				players[whichPlayer].toAttackTemple = false;
				//Abhishek temple attack remove the attack as stooped in between?
				//or continue??? //both seem fine
				//first option has a problem that, the player may not find any area to move
				//due to some player blocking, then!!!
				//but can't save all clicks that way someone can cheat -->
			}
		} else if (players[whichPlayer].whichEnemyPlayerToAttack != -1) {
			if (isNearEnemyPlayerArea(whichPlayer))
				decreaseEnemyPlayerHealth(whichPlayer);
			else {
				players[whichPlayer].whichEnemyPlayerToAttack = -1;
				//Abhishek enemy attack remove the attack as stopped in between?
				//or continue??? //both seem fine
				//first option has a problem that, the player may not find any area to move
				//due to some player blocking, then!!!
				//but can't save all clicks that way someone can cheat -->
			}
		}
		return; //nothing to move
	}
	Coordinate_grid celltoMove = Coordinate_grid(nodeToMove->row,
			nodeToMove->col + ATTRIBUTE_WIDTH);

	if (isItem(celltoMove)) {
		setItemCell(celltoMove, whichPlayer);
		takeItem(whichPlayer);
	}

	putChar2Grid(
			players[whichPlayer].location.row,
			players[whichPlayer].location.col,
			initialGridChar[players[whichPlayer].location.row][players[whichPlayer].location.col
					+ ATTRIBUTE_WIDTH], true, true);

	players[whichPlayer].location.row = nodeToMove->row;
	players[whichPlayer].location.col = nodeToMove->col;

	ifSpawnLocationFullHealth(whichPlayer);

	putChar2Grid(players[whichPlayer].location.row,
			players[whichPlayer].location.col, players[whichPlayer].charType,
			true, true);
}

void putTeamTypeAndTemples() {
	putPngToLAttCell(Coordinate_grid(1, 1), texId_att_team_my, 2, 1);
	putPngToRAttCell(Coordinate_grid(1, 1), texId_att_team_enemy, 2, 1);

	switch (players[currPlayerId].team->name) {
	case TEAM_ANGELS:
		putPngToLAttCell(Coordinate_grid(2, 1), texId_att_team_angels, 2, 1);
		putPngToRAttCell(Coordinate_grid(2, 1), texId_att_team_demons, 2, 1);

		putBmpToLAttCell(Coordinate_grid(4, 1), texId_t_angels, 2, 1); //temples
		putBmpToRAttCell(Coordinate_grid(4, 1), texId_t_demons, 2, 1);
		break;

	case TEAM_DEMONS:
		putPngToLAttCell(Coordinate_grid(2, 1), texId_att_team_demons, 2, 1);
		putPngToRAttCell(Coordinate_grid(2, 1), texId_att_team_angels, 2, 1);

		putBmpToLAttCell(Coordinate_grid(4, 1), texId_t_demons, 2, 1); //temples
		putBmpToRAttCell(Coordinate_grid(4, 1), texId_t_angels, 2, 1);
		break;

	case TEAM_BOTH:
		cout << "---shall never occur---" << endl;
		break;
	}
}

void putMyAttributes() {
	switch (players[currPlayerId].currPowerMode) {
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

	switch (players[currPlayerId].curseType) {
	case CURSE_NONE:
		putPngToLAttCell(Coordinate_grid(10, 1), texId_att_cNone, 2, 1);
		break;
	case CURSE_STUN:
		putPngToLAttCell(Coordinate_grid(10, 1), texId_att_cStun, 2, 1);
		break;
	case CURSE_DISABLE:
		putPngToLAttCell(Coordinate_grid(10, 1), texId_att_cDisable, 2, 1);
		break;
	case CURSE_WEAK:
		putPngToLAttCell(Coordinate_grid(10, 1), texId_att_cSlow, 2, 1);
		break;
	case CURSE_BURST:
		putPngToLAttCell(Coordinate_grid(10, 1), texId_att_cBurst, 2, 1);
		break;
	}

	//Speed
	putPngToLAttCell(Coordinate_grid(12, 1), texId_att_speed, 1, 1);
	putTextToLAttCell(Coordinate_grid(12, 2),
			numToStr(players[currPlayerId].speedMove));

	//Strength
	putPngToLAttCell(Coordinate_grid(13, 1), texId_att_strength, 1, 1);
	putTextToLAttCell(Coordinate_grid(13, 2),
			numToStr(players[currPlayerId].strength));
}

void putHealth() {
	putPngToLAttCell(Coordinate_grid(5, 1), texId_att_temple_health, 1, 1);
	putPngToRAttCell(Coordinate_grid(5, 1), texId_att_temple_health, 1, 1);

	switch (players[currPlayerId].team->name) {
	case TEAM_ANGELS:
		putTextToLAttCell(Coordinate_grid(5, 2),
				numToStr(angelsTeam.templeHealth));
		putTextToRAttCell(Coordinate_grid(5, 2),
				numToStr(demonsTeam.templeHealth));
		break;
	case TEAM_DEMONS:
		putTextToLAttCell(Coordinate_grid(5, 2),
				numToStr(demonsTeam.templeHealth));
		putTextToRAttCell(Coordinate_grid(5, 2),
				numToStr(angelsTeam.templeHealth));
		break;
	}

	int idFriend = players[currPlayerId].idFriend;
	int idEnemy0 = players[currPlayerId].idEnemy[0];
	int idEnemy1 = players[currPlayerId].idEnemy[1];

	putPngToLAttCell(Coordinate_grid(11, 1), texId_att_health, 1, 1);
	putTextToLAttCell(Coordinate_grid(11, 2),
			numToStr(players[currPlayerId].heroHealth));

	if (idFriend != -1) {
		putPngToLAttCell(Coordinate_grid(17, 1), texId_att_health, 1, 1);
		putTextToLAttCell(Coordinate_grid(17, 2),
				numToStr(players[idFriend].heroHealth));
	}

	if (idEnemy0 != -1) {
		putPngToRAttCell(Coordinate_grid(9, 1), texId_att_health, 1, 1);
		putTextToRAttCell(Coordinate_grid(9, 2),
				numToStr(players[idEnemy0].heroHealth));
	}

	if (idEnemy1 != -1) {
		putPngToRAttCell(Coordinate_grid(13, 1), texId_att_health, 1, 1);
		putTextToRAttCell(Coordinate_grid(13, 2),
				numToStr(players[idEnemy1].heroHealth));
	}
}

void putHeroes() {
	int idFriend = players[currPlayerId].idFriend;
	int idEnemy0 = players[currPlayerId].idEnemy[0];
	int idEnemy1 = players[currPlayerId].idEnemy[1];

	switch (players[currPlayerId].heroType) {
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

	if (idFriend != -1) {
		switch (players[idFriend].heroType) {
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
	}

	if (idEnemy0 != -1) {
		switch (players[idEnemy0].heroType) {
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
	}

	if (idEnemy1 != -1) {
		switch (players[idEnemy1].heroType) {
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
}

void putTimers() {
	if (players[currPlayerId].isTimerItemGlobalRunning) {
		putPngToLAttCell(Coordinate_grid(19, 1), texId_att_time_itemOn, 2, 1);
	} else {
		putPngToLAttCell(Coordinate_grid(19, 1), texId_att_time_itemOff, 2, 1);
	}

	if (players[currPlayerId].isTimerMagicSpellRunning) {
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
	//putWall();
}

void renderGrid() {
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			handleGridCharSwitch(Coordinate_grid(r, c), RENDER_GRID);
		}
	}
}

void requestBasicPower() {
	cout << "requesting basic_power by player " << currPlayerId << endl;
	if (players[currPlayerId].currPowerMode != POWER_MODE_BASIC) {
		helperSendPowerMode(0);
	}
}

//------------------------------------Change for single player (start)----------
void requestBasicPowerAI(int botId) {
	cout << "requesting basic_power by player " << botId << endl;
	if (players[botId].currPowerMode != POWER_MODE_BASIC) {
		helperSendPowerModeAI(0, botId);
	}
}

void requestMagicPowerAI(int botId) {
	if (players[botId].curseType != CURSE_DISABLE
			&& players[botId].currPowerMode != POWER_MODE_MAGIC
			&& !players[botId].isTimerMagicSpellRunning) { //filter at client
		cout << "requesting power_magic by player " << botId << endl;
		helperSendPowerModeAI(1, botId);
	} /*else {
	 cout << "Player can not use Magic power, you are cursed!!!" << endl;
	 }*/
}
//------------------------------------Change for single player (end)----------

void requestMagicPower() {
	if (players[currPlayerId].curseType != CURSE_DISABLE
			&& players[currPlayerId].currPowerMode != POWER_MODE_MAGIC
			&& !players[currPlayerId].isTimerMagicSpellRunning) { //filter at client
		cout << "requesting power_magic by player " << currPlayerId << endl;
		helperSendPowerMode(1);
	} /*else {
	 cout << "Player can not use Magic power, you are cursed!!!" << endl;
	 }*/
}

#endif
