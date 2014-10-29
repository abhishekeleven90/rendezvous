#ifndef VALIDATIONS_H
#define VALIDATIONS_H

#include "CustomVectorStruct.h"

bool isValidRowNColIndex(int row, int col, int isInner) {
	if (row < START_GRID_ROW || row > END_GRID_ROW) {
		return false;
	}

	if (isInner) {
		if (col < START_INNER_GRID_COL || col > END_INNER_GRID_COL) {
			return false;
		}
	}

	else {
		if (col < START_OUTER_GRID_COL || col > END_OUTER_GRID_COL) {
			return false;
		}
	}

	return true;
}

bool isValidCell(Coordinate_grid grid) { //Checking it's not in black screen
	return isValidRowNColIndex(grid.row, grid.col, false);
}

bool isOponentCellForTeam(Coordinate_grid grid) { //returns true if the passed cell lies in my opponent area
	if (playerStats.team == TEAM_ANGELS && grid.col >= grid.row + DIAG_BLOCKS
			+ 1) {
		return true;
	}

	if (playerStats.team == TEAM_DEMONS && grid.row >= grid.col + DIAG_BLOCKS
			+ 1) {
		return true;
	}

	return false;
}
#endif
