#ifndef GENERAL_HELPER_H
#define GENERAL_HELPER_H

#include "Globals.h"
#include "OpenGlHelper.h"
#include "FilePaths.h"
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

void loadPlayerSpecificAttributes() {
	//TODO: the below attributes shall be coming from earlier screens
	playerStats.team = TEAM_ANGELS;
	playerStats.heroType = HERO_SLOWER;

	playerStats.currentPower = POWER_BASIC;
	playerStats.heroHealth = HEALTH_FULL_HERO;
	playerStats.templeHealth = HEALTH_FULL_TEMPLE;

	switch (playerStats.heroType) {
	//TODO : decide & change attributes - strength,speedAttack,speedMove etc
	case HERO_STUNNER:
		playerStats.magic_power = MAGIC_STUN;
		playerStats.pic_path = PATH_IMG_HERO_STUNNER;
		playerStats.strength = STRENGTH_H_STUNNER;
		playerStats.speedMove = SPEED_MOVE_H_STUNNER;
		break;

	case HERO_DISABLER:
		playerStats.pic_path = PATH_IMG_HERO_DISABLER;
		playerStats.magic_power = MAGIC_DISABLE;
		playerStats.strength = STRENGTH_H_DISABLER;
		playerStats.speedMove = SPEED_MOVE_H_DISABLER;
		break;

	case HERO_SLOWER:
		playerStats.pic_path = PATH_IMG_HERO_SLOWER;
		playerStats.magic_power = MAGIC_SLOW;
		playerStats.strength = STRENGTH_H_SLOWER;
		playerStats.speedMove = SPEED_MOVE_H_SLOWER;
		break;

	case HERO_BURSTER:
		playerStats.pic_path = PATH_IMG_HERO_BURSTER;
		playerStats.magic_power = MAGIC_BURST;
		playerStats.strength = STRENGTH_H_BURSTER;
		playerStats.speedMove = SPEED_MOVE_H_BURSTER;
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
	Node* nodeToMove = findLocToMove(myTeamPlayers[type - 1].location, type);
	if (nodeToMove == NULL)
		return; //nothing to move

	Coordinate_grid celltoMove = Coordinate_grid(nodeToMove->row,
			nodeToMove->col + ATTRIBUTE_WIDTH);//TODO: constant
	if (isItem(celltoMove)) {
		setItemCell(celltoMove);
		takeItem();
	}

	putCharToGrid(
			myTeamPlayers[type - 1].location.row,
			myTeamPlayers[type - 1].location.col,
			initialGridChar[myTeamPlayers[type - 1].location.row][myTeamPlayers[type
					- 1].location.col + ATTRIBUTE_WIDTH], true);

	myTeamPlayers[type - 1].location.row = nodeToMove->row;
	myTeamPlayers[type - 1].location.col = nodeToMove->col;
	putCharToGrid(myTeamPlayers[type - 1].location.row,
			myTeamPlayers[type - 1].location.col, H_SLOWER, true);
}

void renderGrid() {
	moveHeroMine(1);
	moveHeroMine(2);

	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			handleGridCharSwitch(Coordinate_grid(r, c), RENDER_GRID);
		}
	}
}

void togglePlayer()
{
	if(currentPlayer==1)
		currentPlayer=2;
	else
		currentPlayer=1;
}

#endif
