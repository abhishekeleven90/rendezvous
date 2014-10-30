#ifndef HANDLE_GRID_CHAR_SWITCH_H
#define HANDLE_GRID_CHAR_SWITCH_H

#include "SoundAll.h"
#include "Globals.h"
#include "Objects.h"
#include "Constants.h"
#include "CustomVectorStruct.h"
#include "MapNGrid.h"
#include "Timer.h"

Coordinate_grid targetCell;
Coordinate_grid itemCell;

void setItemCell(Coordinate_grid movingToCell) {
	itemCell.row = movingToCell.row;
	itemCell.col = movingToCell.col;
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
		bool isBackChar, int blocks = 1) {

	switch (callType) {
	case PRINT_GRID:
		cout << " " << toPrint;
		break;
	case PROCESS_MOVE_RIGHT_CLICK:
		/*if (!isValidCellForTeam()) { // check if the cell clicked
		 wrong();
		 return;
		 }*/
		rightClick();
		break;
	case PROCESS_MOVE_LEFT_CLICK:
		/*if (!isValidCellForTeam()) { // check if the cell clicked
		 wrong();
		 return;
		 }*/
		leftClick();
		break;
	case RENDER_GRID:
		if (!isBackChar) {
			putImageToCell(grid, texId, blocks);
		}
		break;
	}
}

void wrong() {
	playEventSound(PATH_SOUND_WRONG_CLICK);
}

void aStarMove(bool through) {
	//have to handle block status here istelf! phew :(
	for (int i = START_GRID_ROW; i <= END_GRID_ROW; i++) {
		for (int j = START_INNER_GRID_COL; j <= END_INNER_GRID_COL; j++) {
			if (isBlockedSite(i, j)) {
				myTeam.players[currentPlayer - 1].astar->blockSiteAStarGrid(i, j);
			} else {
				myTeam.players[currentPlayer - 1].astar->openSiteAStarGrid(i, j);
			}
		}
	}
	//for the not through, the target could be blocked actually, change it locally!
	if (!through) {
		myTeam.players[currentPlayer - 1].astar->openSiteAStarGrid(
				targetCell.row, targetCell.col - ATTRIBUTE_WIDTH);
	}
	myTeam.players[currentPlayer - 1].astar->initAStar(
			myTeam.players[currentPlayer - 1].location, targetCell);
	myTeam.players[currentPlayer - 1].astar->AStar(through);

}

void aStarMoveThrough() {
	aStarMove(true);
}

void aStarMoveNotThrough() {
	aStarMove(false);
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

void updateHeroAttributesTakingItem() {
	//TODO: update hero attributes properly, notify & display in attribute space
	itemType itemTaken = getItemTypeFromCharItem(itemCell);
	switch (itemTaken) {
	case ITEM_DAMAGE:
		cout << "item_damage taken" << endl;
		myTeam.players[currentPlayer - 1].strength += GAIN_ITEM_DAMAGE;
		break;
	case ITEM_HEALTH:
		cout << "item_health taken" << endl;
		myTeam.players[currentPlayer - 1].heroHealth += GAIN_ITEM_HEALTH;
		break;
	case ITEM_SPEED_MOVE:
		cout << "item_speed_move taken" << endl;
		myTeam.players[currentPlayer - 1].speedMove += GAIN_ITEM_SPEED_MOVE;
		break;
	case ITEM_TEMPLE_HEALER:
		cout << "item_temple_healer taken" << endl;
		myTeam.players[currentPlayer - 1].templeHealth += GAIN_ITEM_TEMPLE_HEALER;
		//myTeam.players[currentPlayer - 1].itemsBag.push_back(&itemTaken);
		break;
	}
}

void takeItem() {
	//In actual, not taking the item if globalItemTimer is running
	if (!isTimerItemGlobalRunning) {
		timerItemGlobal(0);
		updateHeroAttributesTakingItem();
	} else {
		cout << "Item not taken" << endl;
	}

	//Irrespective of the GLOBAL_ITEM_TIMER, a new item is displayed at random pos
	putCharToGrid(itemCell.row, itemCell.col, BG_GRASS, false);
	if (itemCell.row > itemCell.col) {
		placeItemAtRandomPos(TEAM_ANGELS);
	}
	if (itemCell.row < itemCell.col) {
		placeItemAtRandomPos(TEAM_DEMONS);
	}
}

void setAttackTemple(int player, bool value) {
	myTeam.players[player - 1].toAttackTemple = value;
}

void decreaseEnemyTempleHealth() {

	enemyTeam.health -= myTeam.players[currentPlayer - 1].strength;
	if (enemyTeam.health < 0)
		enemyTeam.health = 0;
	setAttackTemple(currentPlayer, false);
	cout << "Yayy decreased enemy temple health to " << enemyTeam.health
			<< endl;
}

void attackEnemyTemple() {
	cout << "Gear up! We are attacking opponent's temple!" << endl;
	//TODO: Abhishek for both type of teams
	//write now for demons
	aStarMoveNotThrough();
	setAttackTemple(currentPlayer, true);
	//TODO: Abhishek play temple attack sound
	//TODO: gif attack animation
}

void handleGridCharSwitch(Coordinate_grid grid, switchCallType callType) {
	targetCell = Coordinate_grid(grid.row, grid.col);

	switch (gridChar[grid.row][grid.col]) {
	case BG_GRASS:
		processCase(callType, grid, bg_grass_texId, "Gra", aStarMoveThrough,
				wrong, false);
		break;
	case BG_SPAWN:
		processCase(callType, grid, bg_spawn_texId, "BSp", aStarMoveThrough,
				wrong, false);
		break;
	case BG_WAR:
		processCase(callType, grid, bg_war_texId, "BWa", aStarMoveThrough,
				wrong, false);
		break;
	case BG_ATTRIBUTE:
		processCase(callType, grid, bg_attribute_texId, "BAt", wrong, wrong,
				false);
		break;
	case BG_BLOCKED:
		processCase(callType, grid, bg_blocked_texId, "BBl", wrong, wrong,
				false);
		break;

	case STONE:
		processCase(callType, grid, stone_texId, "Sto", wrong, wrong, false);
		break;
	case TREE:
		processCase(callType, grid, tree_texId, "Tre", wrong, wrong, false);
		break;

	case TEMPLE_ANGELS:
		processCase(callType, grid, t_angels_texId, "TAn", wrong, wrong, false,
				TEMPLE_BLOCKS);
		break;

		//TODO: Abhishek: friend temple attack
	case TEMPLE_DEMONS:
		processCase(callType, grid, t_demons_texId, "TDe", wrong,
				attackEnemyTemple, false, TEMPLE_BLOCKS);
		break;

		//TODO: Abhishek : leftClick on enemy and on friend and on on self
	case H_DISABLER:
		processCase(callType, grid, h_disabler_texId, "HDi", wrong, wrong,
				false);
		break;
	case H_SLOWER:
		processCase(callType, grid, h_slower_texId, "HSl", wrong, wrong, false);
		break;
	case H_BURSTER:
		processCase(callType, grid, h_burster_texId, "HBu", wrong, wrong, false);
		break;
	case H_STUNNER:
		processCase(callType, grid, h_stunner_texId, "HSt", wrong, wrong, false);
		break;

	case I_SPEED_MOVE:
		itemCell = Coordinate_grid(grid.row, grid.col);
		processCase(callType, grid, i_speedMov_texId, "ISM", aStarMoveThrough,
				wrong, false);
		break;
	case I_HEALTH:
		itemCell = Coordinate_grid(grid.row, grid.col);
		processCase(callType, grid, i_health_texId, "IHe", aStarMoveThrough,
				wrong, false);
		break;
	case I_DAMAGE:
		itemCell = Coordinate_grid(grid.row, grid.col);
		processCase(callType, grid, i_damage_texId, "IDa", aStarMoveThrough,
				wrong, false);
		break;
	case I_TEMPLE_HEALER:
		itemCell = Coordinate_grid(grid.row, grid.col);
		processCase(callType, grid, i_tHealer_texId, "ITH", aStarMoveThrough,
				wrong, false);
		break;

	case TREE_BACK:
		processCase(callType, grid, temp_texId, "TBa", wrong, wrong, true);
		break;

		//TODO: if else for demons and angels
	case T_ANGELS_BACK:
		processCase(callType, grid, temp_texId, "TAB", wrong, wrong, true);
		break;
	case T_DEMONS_BACK:
		processCase(callType, grid, temp_texId, "TDB", wrong,
				attackEnemyTemple, true);
		break;

	default:
		cout << "should not happen - something's wrong" << endl;
	}
}

#endif
