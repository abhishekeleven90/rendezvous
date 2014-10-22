#ifndef HANDLE_GRID_CHAR_SWITCH_H
#define HANDLE_GRID_CHAR_SWITCH_H

#include "SoundAll.h"
#include "Globals.h"
#include "Objects.h"
#include "Constants.h"
#include "CustomVectorStruct.h"
#include "MapNGrid.h"

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

void processCase(switchCallType callType, int r, int c, GLuint texId,
		string toPrint, void( f)(void), bool isBackChar, int blocks = 1) {

	switch (callType) {
	case PRINT_GRID:
		cout << " " << toPrint;
		break;
	case PROCESS_MOVE_CLICK:
		f();
		break;
	case RENDER_GRID:
		if (!isBackChar) {
			putImageToCell(r, c, texId, blocks);
		}
		break;
	}
}

void wrong() {
	playEventSound(PATH_SOUND_WRONG_CLICK);
}

int row, col;

void f() {

}

void takeItem() {
	//TODO: logic to check if the global_item_timer expires
	putCharToGrid(row, col, BG_GRASS, false);
	if (row > col) {
		placeItemAtRandomPos( ANGELS);
	}
	if (row < col) {
		placeItemAtRandomPos( DEMONS);
	}

}

void handleGridCharSwitch(int r, int c, switchCallType callType) {
	row = r; //TODO : remove, just for testing
	col = c;
	switch (gridChar[r][c]) {
	case BG_GRASS:
		processCase(callType, r, c, grass_texId, "Gra", f, false);
		break;
	case BG_SPAWN:
		processCase(callType, r, c, spawn_texId, "BSp", f, false);
		break;
	case BG_WAR:
		processCase(callType, r, c, war_texId, "BWa", f, false);
		break;
	case BG_ATTRIBUTE:
		processCase(callType, r, c, attribute_bg_texId, "BAt", wrong, false);
		break;

	case STONE:
		processCase(callType, r, c, stone_texId, "Sto", wrong, false);
		break;
	case TREE:
		processCase(callType, r, c, tree_texId, "Tre", wrong, false);
		break;

	case TEMPLE_ANGELS:
		processCase(callType, r, c, t_angels_texId, "TAn", wrong, false,
				TEMPLE_BLOCKS);
		break;
	case TEMPLE_DEMONS:
		processCase(callType, r, c, t_demons_texId, "TDe", wrong, false,
				TEMPLE_BLOCKS);
		break;

	case H_DISABLER:
		processCase(callType, r, c, h_disabler_texId, "HDi", wrong, false);
		break;
	case H_SLOWER:
		processCase(callType, r, c, h_slower_texId, "HSl", wrong, false);
		break;
	case H_BURSTER:
		processCase(callType, r, c, h_burster_texId, "HBu", wrong, false);
		break;
	case H_STUNNER:
		processCase(callType, r, c, h_stunner_texId, "HSt", wrong, false);
		break;
	case I_SPEED_MOVE:
		processCase(callType, r, c, i_speedMov_texId, "ISM", takeItem, false);
		break;
	case I_SPEED_ATTACK:
		processCase(callType, r, c, i_speedAttack_texId, "ISA", takeItem, false);
		break;
	case I_HEALTH:
		processCase(callType, r, c, i_health_texId, "IHe", takeItem, false);
		break;
	case I_DAMAGE:
		processCase(callType, r, c, i_damage_texId, "IDa", takeItem, false);
		break;
	case I_TEMPLE_HEALER:
		processCase(callType, r, c, i_tHealer_texId, "ITH", takeItem, false);
		break;

	case TREE_BACK:
		processCase(callType, r, c, temp_texId, "TBa", wrong, true);
		break;
	case T_ANGELS_BACK:
		processCase(callType, r, c, temp_texId, "TAB", wrong, true);
		break;
	case T_DEMONS_BACK:
		processCase(callType, r, c, temp_texId, "TDB", wrong, true);
		break;

	default:
		cout << "should not happen - something's wrong" << endl;
	}
}

#endif
