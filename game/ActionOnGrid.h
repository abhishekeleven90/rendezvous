#ifndef HANDLE_GRID_CHAR_SWITCH_H
#define HANDLE_GRID_CHAR_SWITCH_H

#include "SoundAll.h"
#include "Globals.h"
#include "Objects.h"
#include "Constants.h"
#include "CustomVectorStruct.h"
#include "MapNGrid.h"
#include "Timer.h"
#include "Network.h"

Coordinate_grid targetCell;
Coordinate_grid itemCell;

void setItemCell(Coordinate_grid movingToCell, int whichPlayer) {
	players[whichPlayer].itemCell.row = movingToCell.row;
	players[whichPlayer].itemCell.col = movingToCell.col;
}

//TODO: Abhishek: if more items add or try to add to the complex handleGridCharSwitch
bool isItem(Coordinate_grid cell) {
	switch (gridChar[cell.row][cell.col]) {
	case I_DAMAGE:
	case I_HEALTH:
	case I_SPEED_MOVE:
	case I_TEMPLE_HEALER:
		return true;
		break;
	default:
		return false;
	}
	return false;
}

Coordinate_grid getRandomCoordinatesForItem(teamName name) {
	int randomRow;
	int randomCol;

	while (true) {
		randomRow = (rand() % (END_GRID_ROW - START_GRID_ROW + 1)) + 1; //1 extra since we want it to start from 1
		randomCol = (rand() % (END_INNER_GRID_COL - START_INNER_GRID_COL + 1))
				+ 1;

		if (name == TEAM_ANGELS && randomRow < randomCol) {
			continue;
		}

		if (name == TEAM_DEMONS && randomRow > randomCol) {
			continue;
		}

		if (getInnerGridChar(randomRow, randomCol) == BG_GRASS) { //assuming that items can come only on 'grass'
			break;
		}
	}

	return Coordinate_grid(randomRow, randomCol);
}

void placeItemAtRandomPos(teamName name) {
	Coordinate_grid grid = getRandomCoordinatesForItem(name);
	int r = grid.row;
	int c = grid.col;

	switch (name) {
	case TEAM_ANGELS:
		putCharToGrid(r, c, itemCharCell[g_item_index_angels++], true);
		break;

	case TEAM_DEMONS:
		putCharToGrid(r, c, itemCharCell[g_item_index_demons++], true);
		break;

	case TEAM_BOTH:
		if (r > c) {
			putCharToGrid(r, c, itemCharCell[g_item_index_angels++], true);
			putCharToGrid(c, r, itemCharCell[g_item_index_demons++], true);
		} else {
			putCharToGrid(r, c, itemCharCell[g_item_index_demons++], true);
			putCharToGrid(c, r, itemCharCell[g_item_index_angels++], true);
		}
		break;
	}

	g_item_index_angels = g_item_index_angels % ARRAY_SIZE(itemCharCell);
	g_item_index_demons = g_item_index_demons % ARRAY_SIZE(itemCharCell);
}

enum switchCallType {
	PRINT_GRID, PROCESS_MOVE_RIGHT_CLICK, PROCESS_MOVE_LEFT_CLICK, RENDER_GRID
};

void processCase(switchCallType callType, Coordinate_grid grid, GLuint texId,
		string toPrint, void( rightClick)(void), void( leftClick)(void),
		bool isBackChar, int xBlocks = 1, int yBlocks = 1) {

	switch (callType) {
	case PRINT_GRID:
		cout << " " << toPrint;
		break;

	case PROCESS_MOVE_RIGHT_CLICK:
		rightClick();
		break;

	case PROCESS_MOVE_LEFT_CLICK:
		leftClick();
		break;

	case RENDER_GRID:
		if (!isBackChar) {
			putBmpToCell(grid, texId, xBlocks, yBlocks);
		}
		break;
	}
}

void wrong() {
	playEventSound(PATH_SOUND_WRONG_CLICK);
}

void aStarMove(int whichPlayer, bool through) {
	//have to handle block status here itelf! phew :(
	for (int i = START_GRID_ROW; i <= END_GRID_ROW; i++) {
		for (int j = START_INNER_GRID_COL; j <= END_INNER_GRID_COL; j++) {
			if (isBlockedSite(i, j)) {
				players[whichPlayer].astar->blockSiteAStarGrid(i, j);
			} else {
				players[whichPlayer].astar->openSiteAStarGrid(i, j);
			}
		}
	}
	//for the not through, the target could be blocked actually, change it locally!
	if (!through) {
		players[whichPlayer].astar->openSiteAStarGrid(targetCell.row,
				targetCell.col - ATTRIBUTE_WIDTH);
	}
	players[whichPlayer].astar->initAStar(players[whichPlayer].location,
			targetCell);
	players[whichPlayer].astar->AStar(through);
}

void aStarMoveThrough() {
	aStarMove(currPlayerId, true); //TODO: Abhishek, changed for legacy code
	//for testing with one player
}

void aStarMoveNotThrough() {
	aStarMove(currPlayerId, false); //TODO: Abhishek, changed for legacy code
	//for testing with one player
}

void sendServerMove() {
	helperSendServerMove(targetCell);
}

itemType getItemTypeFromCharItem(Coordinate_grid cellForChar) {
	charCellType charItem = gridChar[cellForChar.row][cellForChar.col];
	itemType item;
	switch (charItem) {
	case I_DAMAGE:
		item = ITEM_DAMAGE;
		break;
	case I_HEALTH:
		item = ITEM_HEALTH;
		break;
	case I_SPEED_MOVE:
		item = ITEM_SPEED_MOVE;
		break;
	case I_TEMPLE_HEALER:
		item = ITEM_TEMPLE_HEALER;
		break;
	}
	return item;
}

void updateHeroAttributesTakingItem(int playerId) {
	itemType itemTaken = getItemTypeFromCharItem(players[playerId].itemCell);
	switch (itemTaken) {
	case ITEM_DAMAGE:
		cout << "item_damage taken" << endl;
		players[playerId].strength += GAIN_ITEM_DAMAGE;
		break;
	case ITEM_HEALTH:
		cout << "item_health taken" << endl;
		players[playerId].heroHealth += GAIN_ITEM_HEALTH;
		break;
	case ITEM_SPEED_MOVE:
		cout << "item_speed_move taken" << endl;
		players[playerId].speedMove += GAIN_ITEM_SPEED_MOVE;

		if (players[playerId].speedMove >= SPEED_MAX) {
			//TODO: notification
			cout << "Max Speed Reached" << endl;
			players[playerId].speedMove = SPEED_MAX;
		}
		break;
	case ITEM_TEMPLE_HEALER:

		cout << "item_temple_healer taken" << endl;
		switch (players[playerId].team->name) {
		case TEAM_ANGELS:
			angelsTeam.templeHealth += GAIN_ITEM_TEMPLE_HEALER;
			break;
		case TEAM_DEMONS:
			demonsTeam.templeHealth += GAIN_ITEM_TEMPLE_HEALER;
			break;
		}

		break;
	}
}

void takeItem(int whichPlayer) {
	if (!players[whichPlayer].isTimerItemGlobalRunning) {
		timerItemGlobal(whichPlayer);
		updateHeroAttributesTakingItem(whichPlayer);
	} else {
		cout << "Item not taken" << endl;
	}

	//Irrespective of the GLOBAL_ITEM_TIMER, a new item is displayed at random pos
	putCharToGrid(players[whichPlayer].itemCell.row,
			players[whichPlayer].itemCell.col, BG_GRASS, false);
	if (players[whichPlayer].itemCell.row > players[whichPlayer].itemCell.col) {
		placeItemAtRandomPos(TEAM_ANGELS);
	}
	if (players[whichPlayer].itemCell.row < players[whichPlayer].itemCell.col) {
		placeItemAtRandomPos(TEAM_DEMONS);
	}
}

void setAttackTemple(int whichPlayer, bool value) {
	players[whichPlayer].toAttackTemple = value;
}

void decreaseEnemyTempleHealth(int whichPlayer) { //this is ok, use it
	TeamStruct* enemyTeam;
	if (players[whichPlayer].team->name == TEAM_ANGELS)
		enemyTeam = &demonsTeam;
	else
		enemyTeam = &angelsTeam;

	enemyTeam->templeHealth -= players[whichPlayer].strength;
	if (enemyTeam->templeHealth < 0)
		enemyTeam->templeHealth = 0;
	setAttackTemple(whichPlayer, false);
	cout << "Yayy " << whichPlayer << " decreased enemy temple health to "
			<< enemyTeam->templeHealth << endl;
	//TODO: Abhishek play temple attack sound
	//TODO: Abhishek gif attack animation

}

void attackEnemyTemple() {
	aStarMoveNotThrough();
	setAttackTemple(currPlayerId, true);
}

void attackEnemyTempleGeneric(int whichPlayer) {
	setAttackTemple(whichPlayer, false);
	//above is imp, if path changed in between
	//then the previous value would be true
	//and if arrives by way of moving, and not attacking near temple
	//then will attack even when attack was cancelled

	aStarMove(whichPlayer, false);
	setAttackTemple(whichPlayer, true);
}

void attackAngelsTemple() { //this is ok
	if (players[currPlayerId].team->name == TEAM_DEMONS) {
		//TODO: Abhishek ask the server to attack the angels temple
		//TODO: Abhishek just send the player's ID,with attack temple msg
		//server will call attackEnemyTempleGeneric
		attackEnemyTempleGeneric(currPlayerId);
	}
}

void attackDemonsTemple() { //this is ok
	if (players[currPlayerId].team->name == TEAM_ANGELS) {
		//TODO: Abhishek ask the server to attack demons temple
		//TODO: Abhishek just send the player's ID,with attack temple msg
		attackEnemyTempleGeneric(currPlayerId);
	}
}

void handleGridCharSwitch(Coordinate_grid grid, switchCallType callType) {
	targetCell = Coordinate_grid(grid.row, grid.col);

	switch (gridChar[grid.row][grid.col]) {
	case BG_GRASS:
		processCase(callType, grid, texId_bg_grass, "Gra", sendServerMove,
				wrong, false);
		break;

	case BG_SPAWN:
		processCase(callType, grid, texId_bg_spawn, "BSp", aStarMoveThrough,
				wrong, false);
		break;

	case BG_WAR:
		processCase(callType, grid, texId_bg_war, "BWa", aStarMoveThrough,
				wrong, false);
		break;

	case BG_ATTRIBUTE:
		processCase(callType, grid, texId_bg_attribute, "BAt", wrong, wrong,
				false);
		break;

	case BG_BLOCKED:
		processCase(callType, grid, texId_bg_blocked, "BBl", wrong, wrong,
				false);
		break;

	case STONE:
		processCase(callType, grid, texId_stone, "Sto", wrong, wrong, false);
		break;

	case TREE:
		processCase(callType, grid, texId_tree, "Tre", wrong, wrong, false);
		break;

	case STONE_WAR:
		processCase(callType, grid, texId_stone_war, "StW", wrong, wrong, false);
		break;

	case TREE_WAR:
		processCase(callType, grid, texId_tree_war, "TrW", wrong, wrong, false);
		break;

	case TEMPLE_ANGELS:
		processCase(callType, grid, texId_t_angels, "TAn", wrong,
				attackAngelsTemple, false, TEMPLE_BLOCKS, TEMPLE_BLOCKS);
		break;

	case TEMPLE_DEMONS:
		processCase(callType, grid, texId_t_demons, "TDe", wrong,
				attackDemonsTemple, false, TEMPLE_BLOCKS, TEMPLE_BLOCKS);
		break;

		//TODO: Abhishek : leftClick on enemy and on friend and on self
	case H_DISABLER:
		processCase(callType, grid, texId_h_disabler, "HDi", wrong, wrong,
				false);
		break;

	case H_SLOWER:
		processCase(callType, grid, texId_h_slower, "HSl", wrong, wrong, false);
		break;

	case H_BURSTER:
		processCase(callType, grid, texId_h_burster, "HBu", wrong, wrong, false);
		break;

	case H_STUNNER:
		processCase(callType, grid, texId_h_stunner, "HSt", wrong, wrong, false);
		break;

	case I_SPEED_MOVE:
		//setItemCell(grid, currPlayerId); //TODO: Abhishek i think we do not need
		processCase(callType, grid, texId_i_speedMov, "ISM", aStarMoveThrough,
				wrong, false);
		break;

	case I_HEALTH:
		//setItemCell(grid, currPlayerId);
		processCase(callType, grid, texId_i_health, "IHe", aStarMoveThrough,
				wrong, false);
		break;

	case I_DAMAGE:
		//setItemCell(grid, currPlayerId);
		processCase(callType, grid, texId_i_damage, "IDa", aStarMoveThrough,
				wrong, false);
		break;

	case I_TEMPLE_HEALER:
		//setItemCell(grid, currPlayerId);
		processCase(callType, grid, texId_i_tHealer, "ITH", aStarMoveThrough,
				wrong, false);
		break;

	case TREE_BACK:
		processCase(callType, grid, temp_texId, "TBa", wrong, wrong, true);
		break;

	case T_ANGELS_BACK:
		processCase(callType, grid, temp_texId, "TAB", wrong,
				attackAngelsTemple, true);
		break;

	case T_DEMONS_BACK:
		processCase(callType, grid, temp_texId, "TDB", wrong,
				attackDemonsTemple, true);
		break;

	default:
		cout << "should not happen - something's wrong" << endl;
	}
}

#endif
