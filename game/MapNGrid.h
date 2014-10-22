#ifndef MAP_N_GRID_H
#define MAP_N_GRID_H

#include "HandleGridCharSwitch.h"
#include "Constants.h"
#include "Objects.h"
#include "FilePaths.h"
#include "Globals.h"
#include "Validations.h"

void putAttributeSpace();
void putGrass();
void putSpawnLocation();
void initMap();
GLfloat getXFromCell(int col);
GLfloat getYFromCell(int row);
void putCharToGrid(int row, int col, charCellType charType, bool isInner);
void putMultipleCharToGrid(int row, int col, charCellType charType,
		charCellType backChar, int blocks, bool isInner);
void putImageToCell(int row, int col, GLuint _textureId, int blocks = 1);
void putImageToGrid(GLfloat x, GLfloat y, GLuint _textureId, int blocks);
void renderGrid();
void copyInit();

void putAttributeSpace() {
	//left attribute space
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_LEFT_ATTRIBUTE_COL; c <= END_LEFT_ATTRIBUTE_COL; c++) {
			putCharToGrid(r, c, BG_ATTRIBUTE, false);
		}
	}
	//right attribute space
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_RIGHT_ATTRIBUTE_COL; c <= END_RIGHT_ATTRIBUTE_COL; c++) {
			putCharToGrid(r, c, BG_ATTRIBUTE, false);
		}
	}
}

void putGrass() {
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_INNER_GRID_COL; c <= END_INNER_GRID_COL; c++) {
			putCharToGrid(r, c, BG_GRASS, true);
		}
	}
}

void putSpawnLocation() {
	int k = 1;
	for (int i = END_GRID_ROW - SPAWN_BLOCKS + 1; i <= END_GRID_ROW; i++) {
		for (int j = 1; j <= k; j++) {
			putCharToGrid(i, j, BG_SPAWN, true);
			putCharToGrid(j, i, BG_SPAWN, true);
		}
		k++;
	}
}

void putWarGround() {
	int numOtherDiag = (DIAGONAL_BLOCKS - 1) / 2;

	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_INNER_GRID_COL; c <= END_INNER_GRID_COL; c++) {
			if (r == c) {
				for (int k = numOtherDiag; k >= 0; k--) {
					putCharToGrid(r + k, c, BG_WAR, true);
					putCharToGrid(r, c + k, BG_WAR, true);
				}
			}
		}
	}
}

void putTemple() {
	putMultipleCharToGrid(5, 4, TEMPLE_ANGELS, T_ANGELS_BACK, TEMPLE_BLOCKS,
			true);
	putMultipleCharToGrid(17, 16, TEMPLE_DEMONS, T_DEMONS_BACK, TEMPLE_BLOCKS,
			true);
}

void putToGridFromFile(string filePath, charCellType charCellType,
		bool isTeamArea, bool isInner) {
	ifstream infile(filePath.c_str());
	string line;
	while (getline(infile, line)) {
		istringstream iss(line);
		int a, b;
		if (!(iss >> a >> b)) {
			break;
		} // error

		putCharToGrid(a, b, charCellType, isInner);
		if (isTeamArea) {
			putCharToGrid(b, a, charCellType, isInner);
		}
	}
}

void putStonesInTeamArea() {
	putToGridFromFile(PATH_LOC_STONES_TEAM, STONE, true, true);
}

void putTreesInTeamArea() {
	putToGridFromFile(PATH_LOC_TREES_TEAM, TREE, true, true);
}

void putStonesInCommonArea() {
	putToGridFromFile(PATH_LOC_STONES_COMMON, STONE, false, true);
}

void putTreesInCommonArea() {
	putToGridFromFile(PATH_LOC_TREES_COMMON, TREE, false, true);
}

void putStonesNTrees() {
	putStonesInTeamArea();
	putTreesInTeamArea();
	putStonesInCommonArea();
	putTreesInCommonArea();
}

void initMap() {
	//Maintain the sequence
	putGrass();
	putAttributeSpace();
	putSpawnLocation();
	putWarGround();
	putTemple();

	putStonesNTrees();

	copyInit(); //Disclaimer: to be called before item put & hero put!!!
}

//copies the state of the initial map to another array
//not modifiable now, as discussed
void copyInit() {
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			initialGridChar[r][c] = gridChar[r][c];
		}
	}
}

//TODO - temp for motion of the H_SLOWER
void tempStunnerLocation() {
	static int lastRow = 20;
	static int lastCol = 1;
	//replacing with initial grid char
	putCharToGrid(lastRow, lastCol,
			initialGridChar[lastRow][lastCol + ATTRIBUTE_WIDTH], true);
	lastCol++;
	if (lastCol > 20)
		lastCol = 1;
	putCharToGrid(lastRow, lastCol, H_SLOWER, true);
}

Coordinate_grid getGridCoordinatesFromOpenGl(Coordinate_openGl openGl) {
	int row = (MAX_YCELL - openGl.y) / CELL_LENGTH + 1;
	int col = ((openGl.x - MIN_XCELL) / CELL_LENGTH) + 1;
	return Coordinate_grid(row, col);
}

charCellType getInnerGridChar(int randomRow, int randomCol) {
	return gridChar[randomRow][randomCol + ATTRIBUTE_WIDTH];
}

void putMultipleCharToGrid(int row, int col, charCellType charType,
		charCellType backChar, int blocks, bool isInner) {

	//Adding 'back' characters - required in case of covering multiple cells
	for (int i = 0; i < blocks; i++) {
		for (int j = 0; j < blocks; j++) {
			putCharToGrid(row - i, col + j, backChar, isInner);
		}
	}

	//The first position(bottom-left) should be actual character
	putCharToGrid(row, col, charType, isInner);
}

//In case some new charis added to grid, remember to add the some in renderGrid
void putCharToGrid(int row, int col, charCellType charType, bool isInner) {
	if (!isValidRowNColIndex(row, col, isInner)) {
		return;
	}

	if (isInner) {
		row = row;
		col = col + ATTRIBUTE_WIDTH;
	}

	gridChar[row][col] = charType;
}

void renderGrid() {
	//TODO - remove this method call from here and put in a thread or something
	tempStunnerLocation();

	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			handleGridCharSwitch(r, c, RENDER_GRID);
		}
	}
}

#endif
