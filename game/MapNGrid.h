#include "Headers.h"
#include "Objects.h"

void putAttributeSpace();
void putGrass();
void putSpawnLocation();
void initMap();
GLfloat getXFromCell(int col);
GLfloat getYFromCell(int row);
void putCharToInnerGrid(int row, int col, charCellType charType);
void putCharToOuterGrid(int row, int col, charCellType charType);
void putMultipleCharToInnerGrid(int row, int col, charCellType charType,
		charCellType backChar, int blocks);
void putImageToCell(int row, int col, GLuint _textureId, int blocks);
void putImageToGrid(GLfloat x, GLfloat y, GLuint _textureId, int blocks);
void renderGrid();
void copyInit();

void putAttributeSpace() {
	//left attribute space
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_LEFT_ATTRIBUTE_COL; c <= END_LEFT_ATTRIBUTE_COL; c++) {
			putCharToOuterGrid(r, c, ATTRIBUTE_BG);
		}
	}
	//right attribute space
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_RIGHT_ATTRIBUTE_COL; c <= END_RIGHT_ATTRIBUTE_COL; c++) {
			putCharToOuterGrid(r, c, ATTRIBUTE_BG);
		}
	}
}

void putGrass() {
	//grass
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_INNER_GRID_COL; c <= END_INNER_GRID_COL; c++) {
			putCharToInnerGrid(r, c, GRASS);
		}
	}
}

void putSpawnLocation() {
	int k = 1;
	for (int i = END_GRID_ROW - SPAWN_BLOCKS + 1; i <= END_GRID_ROW; i++) {
		for (int j = 1; j <= k; j++) {
			putCharToInnerGrid(i, j, SPAWN);
			putCharToInnerGrid(j, i, SPAWN);
		}
		k++;
	}
}

void putTemple() {
	putMultipleCharToInnerGrid(5, 4, TEMPLE_ANGELS, T_ANGELS_BACK,
			TEMPLE_BLOCKS);
	putMultipleCharToInnerGrid(17, 16, TEMPLE_DEMONS, T_DEMONS_BACK,
			TEMPLE_BLOCKS);
}

void putStonesInTeamArea() {
	putCharToInnerGrid(11, 1, STONE);
	putCharToInnerGrid(11, 2, STONE);
	putCharToInnerGrid(12, 4, STONE);
	putCharToInnerGrid(13, 5, STONE);
	putCharToInnerGrid(15, 2, STONE);
	putCharToInnerGrid(16, 3, STONE);
	putCharToInnerGrid(19, 5, STONE);
	putCharToInnerGrid(11, 8, STONE);
	putCharToInnerGrid(12, 8, STONE);
	putCharToInnerGrid(12, 9, STONE);
	putCharToInnerGrid(16, 12, STONE);
	putCharToInnerGrid(19, 4, STONE);
	putCharToInnerGrid(20, 11, STONE);
	putCharToInnerGrid(20, 13, STONE);
	putCharToInnerGrid(20, 14, STONE);
	putCharToInnerGrid(20, 15, STONE);
}

void putTreesInTeamArea() {
	putCharToInnerGrid(4, 1, TREE);
	putCharToInnerGrid(5, 1, TREE);
	putCharToInnerGrid(5, 2, TREE);
	putCharToInnerGrid(6, 1, TREE);
	putCharToInnerGrid(6, 2, TREE);
	putCharToInnerGrid(8, 3, TREE);
	putCharToInnerGrid(8, 4, TREE);
	putCharToInnerGrid(9, 3, TREE);
	putCharToInnerGrid(9, 4, TREE);
	putCharToInnerGrid(12, 6, TREE);
	putCharToInnerGrid(13, 6, TREE);
	putCharToInnerGrid(14, 6, TREE);
	putCharToInnerGrid(15, 6, TREE);
	putCharToInnerGrid(16, 6, TREE);
	putCharToInnerGrid(17, 6, TREE);
	putCharToInnerGrid(18, 6, TREE);
	putCharToInnerGrid(19, 6, TREE);
	putCharToInnerGrid(19, 5, TREE);
	putCharToInnerGrid(15, 3, TREE);
	putCharToInnerGrid(16, 2, TREE);
	putCharToInnerGrid(15, 7, TREE);
	putCharToInnerGrid(15, 8, TREE);
	putCharToInnerGrid(16, 7, TREE);
	putCharToInnerGrid(16, 8, TREE);
	putCharToInnerGrid(20, 8, TREE);
	putCharToInnerGrid(19, 16, TREE);
	putCharToInnerGrid(20, 16, TREE);
	putCharToInnerGrid(20, 17, TREE);
	putCharToInnerGrid(13, 8, TREE);
	putCharToInnerGrid(13, 9, TREE);
	putCharToInnerGrid(13, 10, TREE);
	putCharToInnerGrid(15, 10, TREE);
	putCharToInnerGrid(16, 10, TREE);
	putCharToInnerGrid(17, 10, TREE);
	putCharToInnerGrid(18, 10, TREE);
	putCharToInnerGrid(18, 8, TREE);
	putCharToInnerGrid(18, 9, TREE);
	putCharToInnerGrid(19, 9, TREE);
	putCharToInnerGrid(19, 9, TREE);
}

void putStonesInCommonArea() {
	putCharToInnerGrid(7, 6, STONE);
	putCharToInnerGrid(7, 8, STONE);
	putCharToInnerGrid(8, 8, STONE);
	putCharToInnerGrid(9, 10, STONE);
	putCharToInnerGrid(10, 10, STONE);
	putCharToInnerGrid(11, 12, STONE);
	putCharToInnerGrid(12, 12, STONE);
	putCharToInnerGrid(13, 14, STONE);
	putCharToInnerGrid(14, 14, STONE);
}

void putTreesInCommonArea() {
	putCharToInnerGrid(1, 1, TREE);
	putCharToInnerGrid(1, 2, TREE);
	putCharToInnerGrid(1, 3, TREE);
	putCharToInnerGrid(2, 1, TREE);
	putCharToInnerGrid(2, 2, TREE);
	putCharToInnerGrid(3, 1, TREE);
	putCharToInnerGrid(8, 6, TREE);
	putCharToInnerGrid(9, 8, TREE);
	putCharToInnerGrid(11, 10, TREE);
	putCharToInnerGrid(13, 12, TREE);
	putCharToInnerGrid(15, 14, TREE);
	putCharToInnerGrid(18, 20, TREE);
	putCharToInnerGrid(19, 19, TREE);
	putCharToInnerGrid(19, 20, TREE);
	putCharToInnerGrid(20, 18, TREE);
	putCharToInnerGrid(20, 19, TREE);
	putCharToInnerGrid(20, 20, TREE);
}

void initMap() {
	putAttributeSpace();
	putGrass();
	putSpawnLocation();
	putTemple();

	putStonesInTeamArea();
	putTreesInTeamArea();
	putStonesInCommonArea();
	putTreesInCommonArea();

	copyInit(); //DISCLAIMER: TO BE CALLED BEFORE ITEM PUT AND HERO PUT!!!
	putCharToInnerGrid(19, 1, H_SLOWER);
}

//copies the state of the initial map to another array
//not modifiable now, as discussed
void copyInit()
{
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			initialGridChar[r][c] = gridChar[r][c];
		}
	}
}

//TODO - temp for motion of the H_STUNNER
void tempStunnerLocation() {
	static int lastRow = 20;
	static int lastCol = 1;
	//replacing with initial grid char
	putCharToInnerGrid(lastRow, lastCol,
			initialGridChar[lastRow][lastCol + ATTRIBUTE_WIDTH]);
	lastCol++;
	if (lastCol > 20)
		lastCol = 1;
	putCharToInnerGrid(lastRow, lastCol, H_SLOWER);
}

GLfloat getXFromCell(int col) {
	GLfloat x = MIN_XCELL + (col - 1) * CELL_LENGTH;
	return x;
}

GLfloat getYFromCell(int row) {
	GLfloat y = MAX_YCELL - (row * CELL_LENGTH);
	return y;
}

//Function shall not be used directly- use either 'putMultipleCharToInnerGrid' or 'putMultipleCharToOuterGrid
void putMultipleCharToGrid(int blocks, int row, int col, charCellType backChar,
		charCellType charType) {
	//Adding 'back' characters - required in case of covering multiple cells
	for (int i = 0; i < blocks; i++) {
		for (int j = 0; j < blocks; j++) {
			gridChar[row - i][col + j] = backChar;
		}
	}

	//The first position(bottom-left) should be actual character
	gridChar[row][col] = charType;
}

void putCharToInnerGrid(int row, int col, charCellType charType) {
	row = row;
	col = col + ATTRIBUTE_WIDTH;
	gridChar[row][col] = charType;
}

//to be used only in case of attributeSpace
void putCharToOuterGrid(int row, int col, charCellType charType) {
	gridChar[row][col] = charType;
}

void putMultipleCharToInnerGrid(int row, int col, charCellType charType,
		charCellType backChar, int blocks = 1) {
	row = row;
	col = col + ATTRIBUTE_WIDTH;
	putMultipleCharToGrid(blocks, row, col, backChar, charType);
}

//to be used only in case of attributeSpace
void putMultipleCharToOuterGrid(int row, int col, charCellType charType,
		charCellType backChar, int blocks = 1) {
	putMultipleCharToGrid(blocks, row, col, backChar, charType);
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
	//TODO - rmove this method call from here and put in a thread or something
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
