#ifndef HANDLE_GRID_CHAR_SWITCH_H
#define HANDLE_GRID_CHAR_SWITCH_H

#include "SoundAll.h"
#include "Globals.h"
#include "Objects.h"
#include "Constants.h"
#include "CustomVectorStruct.h"
#include "MapNGrid.h"
#include "AStar.h"

Coordinate_grid getRandomCoordinatesForItem(teamName name) {
	int randomRow;
	int randomCol;

	while (true) {
		randomRow = (rand() % (END_GRID_ROW - START_GRID_ROW + 1)) + 1; //1 extra since we want it to start from 1
		randomCol = (rand() % (END_INNER_GRID_COL - START_INNER_GRID_COL + 1))
				+ 1;

		if (name == ANGELS && randomRow < randomCol) {
			continue;
		}

		if (name == DEMONS && randomRow > randomCol) {
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
	case ANGELS:
		putCharToGrid(r, c, itemCharCell[g_item_index_angels++], true);
		break;

	case DEMONS:
		putCharToGrid(r, c, itemCharCell[g_item_index_demons++], true);
		break;

	case BOTH:
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
	PRINT_GRID, PROCESS_MOVE_CLICK, RENDER_GRID
};

void processCase(switchCallType callType, Coordinate_grid grid, GLuint texId,
		string toPrint, void( f)(void), bool isBackChar, int blocks = 1) {

	switch (callType) {
	case PRINT_GRID:
		cout << " " << toPrint;
		break;
	case PROCESS_MOVE_CLICK:
		/*if (!isValidCellForTeam()) { // check if the cell clicked
		 wrong();
		 return;
		 }*/
		f();
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

Coordinate_grid targetCell;

void f() {

}

//use downGrid
void aStarMove() {

	//TODO: eliminate if outerGrid click
	//assuming : the click is valid and AStar needs to be called
	//have to handle block status here istelf! phew :(

	for (int i = START_GRID_ROW; i <= END_GRID_ROW; i++) {
		for (int j = START_INNER_GRID_COL; j <= END_INNER_GRID_COL; j++) {
			if (isBlockedSite(i, j)) {
				blockSiteAStarGrid(i, j);
			} else {
				openSiteAStarGrid(i, j);
			}
		}

	}
	initAStar(HERO_MINE_1_LOC, targetCell);//source and target given
	AStar();
	//TODO:through or just to target!
	//now move his ass periodically

}

void takeItem() {
	//TODO: logic to check if the global_item_timer expires


	putCharToGrid(targetCell.row, targetCell.col, BG_GRASS, false);
	if (targetCell.row > targetCell.col) {
		placeItemAtRandomPos( ANGELS);
	}
	if (targetCell.row < targetCell.col) {
		placeItemAtRandomPos( DEMONS);
	}

}

void handleGridCharSwitch(Coordinate_grid grid, switchCallType callType) {
	targetCell = Coordinate_grid(grid.row, grid.col);

	switch (gridChar[grid.row][grid.col]) {
	case BG_GRASS:
		processCase(callType, grid, grass_texId, "Gra", aStarMove, false);
		break;
	case BG_SPAWN:
		processCase(callType, grid, spawn_texId, "BSp", aStarMove, false);
		break;
	case BG_WAR:
		processCase(callType, grid, war_texId, "BWa", aStarMove, false);
		break;
	case BG_ATTRIBUTE:
		processCase(callType, grid, attribute_bg_texId, "BAt", wrong, false);
		break;

	case STONE:
		processCase(callType, grid, stone_texId, "Sto", wrong, false);
		break;
	case TREE:
		processCase(callType, grid, tree_texId, "Tre", wrong, false);
		break;

	case TEMPLE_ANGELS:
		processCase(callType, grid, t_angels_texId, "TAn", wrong, false,
				TEMPLE_BLOCKS);
		break;
	case TEMPLE_DEMONS:
		processCase(callType, grid, t_demons_texId, "TDe", wrong, false,
				TEMPLE_BLOCKS);
		break;

	case H_DISABLER:
		processCase(callType, grid, h_disabler_texId, "HDi", wrong, false);
		break;
	case H_SLOWER:
		processCase(callType, grid, h_slower_texId, "HSl", wrong, false);
		break;
	case H_BURSTER:
		processCase(callType, grid, h_burster_texId, "HBu", wrong, false);
		break;
	case H_STUNNER:
		processCase(callType, grid, h_stunner_texId, "HSt", wrong, false);
		break;
	case I_SPEED_MOVE:
		processCase(callType, grid, i_speedMov_texId, "ISM", takeItem, false);
		break;
	case I_SPEED_ATTACK:
		processCase(callType, grid, i_speedAttack_texId, "ISA", takeItem, false);
		break;
	case I_HEALTH:
		processCase(callType, grid, i_health_texId, "IHe", takeItem, false);
		break;
	case I_DAMAGE:
		processCase(callType, grid, i_damage_texId, "IDa", takeItem, false);
		break;
	case I_TEMPLE_HEALER:
		processCase(callType, grid, i_tHealer_texId, "ITH", takeItem, false);
		break;

	case TREE_BACK:
		processCase(callType, grid, temp_texId, "TBa", wrong, true);
		break;
	case T_ANGELS_BACK:
		processCase(callType, grid, temp_texId, "TAB", wrong, true);
		break;
	case T_DEMONS_BACK:
		processCase(callType, grid, temp_texId, "TDB", wrong, true);
		break;

	default:
		cout << "should not happen - something's wrong" << endl;
	}
}

#endif
