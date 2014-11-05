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

bool isMyFriend(teamName team, int playerId) {
	return team == players[playerId].team->name;
}

bool isNotMyFriend(teamName team, int playerId) {
	return team != players[playerId].team->name;
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
	angelsTeam.name = TEAM_ANGELS;
	angelsTeam.templeHealth = HEALTH_FULL_TEMPLE;

	angelsTeam.name = TEAM_DEMONS;
	demonsTeam.templeHealth = HEALTH_FULL_TEMPLE;
}

void loadPlayerGeneralAttributes(int playerId) {
	players[playerId].astar = new AStarClass();
	players[playerId].astar->firstInitAStar();

	players[playerId].currentPowerMode = POWER_MODE_BASIC;
	players[playerId].curseType = CURSE_NONE;
	players[playerId].heroHealth = HEALTH_FULL_HERO;
	cout << "reached5" << endl;
	cout << players[playerId].heroType;
	switch (players[playerId].heroType) {
	case HERO_STUNNER:
		cout << "reached6" << endl;
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
	putCharToGrid(row, col, players[playerId].charType, true);

	//setting myFriend & enemyIds
	teamName myTeam = players[currPlayerId].team->name;
	int k = 0; //used to keep track of enemy index
	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		if (i == currPlayerId) {
			continue;
		}

		if (isMyFriend(myTeam, i)) {
			players[currPlayerId].idFriend = i;
		}

		else if (isNotMyFriend(myTeam, i)) {
			players[currPlayerId].idEnemy[k++] = i;
		}
	}

}

void loadPlayerSpecificAttributes() {
	players[currPlayerId].isTimerItemGlobalRunning = false;
	players[currPlayerId].isTimerMagicSpellRunning = false;
	players[currPlayerId].isTimerCurseRunning = false;

	server_port = players[currPlayerId].networkDetails->port;
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
	Node* nodeToMove =
			findLocToMove(players[whichPlayer].location, whichPlayer);
	if (nodeToMove == NULL) {
		if (players[whichPlayer].toAttackTemple)//TODO: Abhishek AND condition for is nearEnemyTemple
			decreaseEnemyTempleHealth(whichPlayer);
		return; //nothing to move
	}
	Coordinate_grid celltoMove = Coordinate_grid(nodeToMove->row,
			nodeToMove->col + ATTRIBUTE_WIDTH);

	if (isItem(celltoMove)) {
		setItemCell(celltoMove, whichPlayer);
		takeItem(whichPlayer);
	}

	putCharToGrid(
			players[whichPlayer].location.row,
			players[whichPlayer].location.col,
			initialGridChar[players[whichPlayer].location.row][players[whichPlayer].location.col
					+ ATTRIBUTE_WIDTH], true);

	players[whichPlayer].location.row = nodeToMove->row;
	players[whichPlayer].location.col = nodeToMove->col;
	putCharToGrid(players[whichPlayer].location.row,
			players[whichPlayer].location.col, players[whichPlayer].charType,
			true);
}

void putTeamTypeAndTemples() {
	putPngToLAttCell(Coordinate_grid(1, 1), texId_att_team_my, 2, 1);
	putPngToRAttCell(Coordinate_grid(1, 1), texId_att_team_enemy, 2, 1);

	switch (players[currPlayerId].team->name) {
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
	switch (players[currPlayerId].currentPowerMode) {
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
	case CURSE_SLOW:
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

	putPngToLAttCell(Coordinate_grid(17, 1), texId_att_health, 1, 1);
	putTextToLAttCell(Coordinate_grid(17, 2),
			numToStr(players[idFriend].heroHealth));

	putPngToRAttCell(Coordinate_grid(9, 1), texId_att_health, 1, 1);
	putTextToRAttCell(Coordinate_grid(9, 2),
			numToStr(players[idEnemy0].heroHealth));

	putPngToRAttCell(Coordinate_grid(13, 1), texId_att_health, 1, 1);
	putTextToRAttCell(Coordinate_grid(13, 2),
			numToStr(players[idEnemy1].heroHealth));
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
		players[currPlayerId].currentPowerMode = POWER_MODE_BASIC;
		timerCurse(CURSE_DISABLE);
		break;

	case CURSE_SLOW:
		cout << "slowed" << endl;
		timerCurse(CURSE_SLOW);
		break;

	case CURSE_BURST:
		cout << "bursted" << endl;
		players[currPlayerId].heroHealth -= CURSE_AMT_BURST_DAMAGE;
		break;

	case CURSE_NONE:
		cout << "---Inside cursePlayer: shall not come here---" << endl;
		return;
	}

	players[currPlayerId].curseType = curseType;
}

void togglePlayer() { //TODO: check if required
	if (currPlayerId == 1)
		currPlayerId = 2;
	else
		currPlayerId = 1;
}
#endif
