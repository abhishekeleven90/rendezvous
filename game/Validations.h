#include "Constants.h"

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

bool isValidCell(Coordinate_grid grid) {
	if (grid.row < START_GRID_ROW || grid.row > END_GRID_ROW) {
		return false;
	}

	if (grid.col < START_OUTER_GRID_COL || grid.col > END_OUTER_GRID_COL) {
		return false;
	}

	return true;
}

