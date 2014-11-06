#ifndef VALIDATIONS_H
#define VALIDATIONS_H

#include "CustomVectorStruct.h"

bool isValidRowNColIndex(Coordinate_grid grid, int isInner) {
	if (grid.row < START_GRID_ROW || grid.row > END_GRID_ROW) {
		return false;
	}

	if (isInner) {
		if (grid.col < START_INNER_GRID_COL || grid.col > END_INNER_GRID_COL) {
			return false;
		}
	}

	else {
		if (grid.col < START_OUTER_GRID_COL || grid.col > END_OUTER_GRID_COL) {
			return false;
		}
	}

	return true;
}

bool isValidCell(Coordinate_grid grid) { //Checking it's not in black screen
	return isValidRowNColIndex(grid, false);
}

bool isOponentCellForTeam(Coordinate_grid grid, int whichPlayer) { //returns true if the passed cell lies in my opponent area
	int myTeam = players[whichPlayer].team->name;

	if (myTeam == TEAM_ANGELS && grid.col >= grid.row + DIAG_BLOCKS + 1) {
		return true;
	}

	if (myTeam == TEAM_DEMONS && grid.row >= grid.col + DIAG_BLOCKS + 1) {
		return true;
	}

	return false;
}
#endif
