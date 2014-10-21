#include "Headers.h"
#include "Objects.h"
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
void putImageToCell(int row, int col, GLuint _textureId, int blocks);
void putImageToGrid(GLfloat x, GLfloat y, GLuint _textureId, int blocks);
void renderGrid();
void copyInit();

void putAttributeSpace() {
	//left attribute space
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_LEFT_ATTRIBUTE_COL; c <= END_LEFT_ATTRIBUTE_COL; c++) {
			putCharToGrid(r, c, ATTRIBUTE_BG, false);
		}
	}
	//right attribute space
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_RIGHT_ATTRIBUTE_COL; c <= END_RIGHT_ATTRIBUTE_COL; c++) {
			putCharToGrid(r, c, ATTRIBUTE_BG, false);
		}
	}
}

void putGrass() {
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_INNER_GRID_COL; c <= END_INNER_GRID_COL; c++) {
			putCharToGrid(r, c, GRASS, true);
		}
	}
}

void putSpawnLocation() {
	int k = 1;
	for (int i = END_GRID_ROW - SPAWN_BLOCKS + 1; i <= END_GRID_ROW; i++) {
		for (int j = 1; j <= k; j++) {
			putCharToGrid(i, j, SPAWN, true);
			putCharToGrid(j, i, SPAWN, true);
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
					putCharToGrid(r + k, c, WAR_GROUND, true);
					putCharToGrid(r, c + k, WAR_GROUND, true);
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

	copyInit(); //Disclaimer: toe be called before item put & hero put!!!
	putCharToGrid(19, 1, H_SLOWER, true);
}

//copies the state of the initial map to another array
//this is modifiable as item changes
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

GLfloat getXFromCell(int col) {
	GLfloat x = MIN_XCELL + (col - 1) * CELL_LENGTH;
	return x;
}

GLfloat getYFromCell(int row) {
	GLfloat y = MAX_YCELL - (row * CELL_LENGTH);
	return y;
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

void putImageToCell(int row, int col, GLuint _textureId, int blocks = 1) {
	GLfloat x = getXFromCell(col);
	GLfloat y = getYFromCell(row);
	putImageToGrid(x, y, _textureId, blocks);
}

void putImageToGrid(GLfloat x, GLfloat y, GLuint _textureId, int blocks) {
	GLfloat size1D = blocks * CELL_LENGTH;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(x, y, -5.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(x, y + size1D, -5.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(x + size1D, y + size1D, -5.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(x + size1D, y, -5.0f);

	glEnd();
}

void renderGrid() {
	//TODO - remove this method call from here and put in a thread or something
	tempStunnerLocation();
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {

			switch (gridChar[r][c]) {
			case GRASS:
				putImageToCell(r, c, grass_texId);
				break;
			case SPAWN:
				putImageToCell(r, c, spawn_texId);
				break;
			case WAR_GROUND:
				putImageToCell(r, c, war_texId);
				break;
			case ATTRIBUTE_BG:
				putImageToCell(r, c, attribute_bg_texId);
				break;
			case TEMPLE_ANGELS:
				putImageToCell(r, c, t_angels_texId, TEMPLE_BLOCKS);
				break;
			case TEMPLE_DEMONS:
				putImageToCell(r, c, t_demons_texId, TEMPLE_BLOCKS);
				break;
			case H_DISABLER:
				putImageToCell(r, c, h_disabler_texId);
				break;
			case H_SLOWER:
				putImageToCell(r, c, h_slower_texId);
				break;
			case H_BURSTER:
				putImageToCell(r, c, h_burster_texId);
				break;
			case H_STUNNER:
				putImageToCell(r, c, h_stunner_texId);
				break;
			case STONE:
				putImageToCell(r, c, stone_texId);
				break;
			case TREE:
				putImageToCell(r, c, tree_texId);
				break;
			case TREE_BACK:
				break;
			case T_ANGELS_BACK:
				break;
			case T_DEMONS_BACK:
				break;
			case DEFAULT:
				break;
			default:
				cout << "--SOMETHING WENT WRONG while rendering grid--" << endl;
				break;
			}
		}
	}
}
