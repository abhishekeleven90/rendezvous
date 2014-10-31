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
	putPngImageToLeftAttCell(PATH_IMG_ATT_TEAM_MY, Coordinate_grid(1, 1), 2, 1);
	putPngImageToLeftAttCell(PATH_IMG_ATT_TIMERS, Coordinate_grid(17, 1), 2, 1);

	putPngImageToRightAttCell(PATH_IMG_ATT_TEAM_ENEMY, Coordinate_grid(1, 1),
			2, 1);
	putPngImageToRightAttCell(PATH_IMG_ATT_WALL, Coordinate_grid(16, 1), 2, 1);
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
