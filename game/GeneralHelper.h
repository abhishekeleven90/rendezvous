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

void blockOponentsArea() {
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_INNER_GRID_COL; c <= END_INNER_GRID_COL; c++) {
			if (isOponentCellForTeam(Coordinate_grid(r, c))) {
				putCharToGrid(r, c, BG_BLOCKED, true);
			}
		}
	}
}

void loadTeamAttributes() {
	//TODO: Abhishek from other screens, or as decided

	myTeam.team_name = TEAM_ANGELS;
	myTeam.health = HEALTH_FULL_TEMPLE;
	//first player load attributes yours, plus others
	//first player may be the first player to join

	enemyTeam.health = HEALTH_FULL_TEMPLE;
	enemyTeam.team_name = TEAM_DEMONS;
	//load other attributes

}

void loadPlayerSpecificAttributes() {
	//TODO: the below attributes shall be coming from earlier screens
	myTeam.players[currentPlayer - 1].team = TEAM_ANGELS;
	myTeam.players[currentPlayer - 1].heroType = HERO_SLOWER;

	myTeam.players[currentPlayer - 1].currentPower = POWER_BASIC;
	myTeam.players[currentPlayer - 1].heroHealth = HEALTH_FULL_HERO;
	myTeam.players[currentPlayer - 1].templeHealth = HEALTH_FULL_TEMPLE;

	switch (myTeam.players[currentPlayer - 1].heroType) {
	//TODO : decide & change attributes - strength,speedAttack,speedMove etc
	case HERO_STUNNER:
		myTeam.players[currentPlayer - 1].magic_power = MAGIC_STUN;
		myTeam.players[currentPlayer - 1].pic_path = PATH_IMG_HERO_STUNNER;
		myTeam.players[currentPlayer - 1].strength = STRENGTH_H_STUNNER;
		myTeam.players[currentPlayer - 1].speedMove = SPEED_MOVE_H_STUNNER;
		break;

	case HERO_DISABLER:
		myTeam.players[currentPlayer - 1].pic_path = PATH_IMG_HERO_DISABLER;
		myTeam.players[currentPlayer - 1].magic_power = MAGIC_DISABLE;
		myTeam.players[currentPlayer - 1].strength = STRENGTH_H_DISABLER;
		myTeam.players[currentPlayer - 1].speedMove = SPEED_MOVE_H_DISABLER;
		break;

	case HERO_SLOWER:
		myTeam.players[currentPlayer - 1].pic_path = PATH_IMG_HERO_SLOWER;
		myTeam.players[currentPlayer - 1].magic_power = MAGIC_SLOW;
		myTeam.players[currentPlayer - 1].strength = STRENGTH_H_SLOWER;
		myTeam.players[currentPlayer - 1].speedMove = SPEED_MOVE_H_SLOWER;
		break;

	case HERO_BURSTER:
		myTeam.players[currentPlayer - 1].pic_path = PATH_IMG_HERO_BURSTER;
		myTeam.players[currentPlayer - 1].magic_power = MAGIC_BURST;
		myTeam.players[currentPlayer - 1].strength = STRENGTH_H_BURSTER;
		myTeam.players[currentPlayer - 1].speedMove = SPEED_MOVE_H_BURSTER;
		break;
	}

	blockOponentsArea();
}

void printGrid() {
	cout << "Printing grid" << "\n";
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			handleGridCharSwitch(Coordinate_grid(r, c), PRINT_GRID);
		}
		cout << "\n";
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
			myTeam.players[type - 1].location.col, H_SLOWER, true);
}

void loadAttributeSpace() {
	putImageToLeftAttCell(Coordinate_grid(1, 1), texId_att_team_my, 2, 1);
	putImageToLeftAttCell(Coordinate_grid(6, 1), texId_att_health, 1, 1);
	putImageToLeftAttCell(Coordinate_grid(12, 1), texId_att_health, 1, 1);
	putImageToLeftAttCell(Coordinate_grid(15, 1), texId_att_health, 1, 1);
	putImageToLeftAttCell(Coordinate_grid(17, 1), texId_att_timers, 2, 1);

	putImageToRightAttCell(Coordinate_grid(1, 1), texId_att_team_enemy, 2, 1);
	putImageToRightAttCell(Coordinate_grid(6, 1), texId_att_health, 1, 1);
	putImageToRightAttCell(Coordinate_grid(10, 1), texId_att_health, 1, 1);
	putImageToRightAttCell(Coordinate_grid(13, 1), texId_att_health, 1, 1);
	putImageToRightAttCell(Coordinate_grid(15, 1), texId_att_wall, 2, 1);

	/*putImageToLeftAttCell(Coordinate_grid(2, 1), texId_att_time_iOff, 2, 1);

	 putImageToRightAttCell(Coordinate_grid(3, 1), texId_att_time_iOn, 2, 1);

	 putImageToLeftAttCell(Coordinate_grid(3, 1), texId_att_timers, 2, 1);

	 putImageToRightAttCell(Coordinate_grid(4, 1), texId_att_time_mOn, 2, 1);
	 putImageToLeftAttCell(Coordinate_grid(4, 1), texId_att_time_mOff, 2, 1);

	 putImageToRightAttCell(Coordinate_grid(5, 1), texId_att_team_demons, 2, 1);
	 putImageToLeftAttCell(Coordinate_grid(5, 1), texId_att_team_angels, 2, 1);

	 putImageToRightAttCell(Coordinate_grid(6, 1), texId_att_mode_magic, 2, 1);
	 putImageToLeftAttCell(Coordinate_grid(6, 1), texId_att_mode_basic, 2, 1);

	 putImageToRightAttCell(Coordinate_grid(7, 1), texId_att_h_stunner, 2, 1);
	 putImageToLeftAttCell(Coordinate_grid(7, 1), texId_att_h_slower, 2, 1);

	 putImageToRightAttCell(Coordinate_grid(8, 1), texId_att_h_dsabler, 2, 1);
	 putImageToLeftAttCell(Coordinate_grid(8, 1), texId_att_h_burster, 2, 1);

	 putImageToRightAttCell(Coordinate_grid(9, 1), texId_att_cNone, 2, 1);
	 putImageToLeftAttCell(Coordinate_grid(9, 1), texId_att_cStun, 2, 1);

	 putImageToRightAttCell(Coordinate_grid(10, 1), texId_att_cSlow, 2, 1);
	 putImageToLeftAttCell(Coordinate_grid(10, 1), texId_att_cDisable, 2, 1);

	 putImageToRightAttCell(Coordinate_grid(11, 1), texId_att_cBurst, 2, 1);
	 putImageToRightAttCell(Coordinate_grid(11, 1), texId_att_wall, 2, 1);*/
}

void renderGrid() {
	moveHeroMine(1);
	moveHeroMine(2);

	loadAttributeSpace();

	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			handleGridCharSwitch(Coordinate_grid(r, c), RENDER_GRID);
		}
	}

}

void togglePlayer() {
	if (currentPlayer == 1)
		currentPlayer = 2;
	else
		currentPlayer = 1;
}
#endif
