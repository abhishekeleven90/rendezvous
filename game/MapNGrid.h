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

void initMap() {
	putAttributeSpace();
	putGrass();
	putSpawnLocation();
	putTemple();

	putCharToInnerGrid(19, 1, H_SLOWER);
}

//TODO- temp for motion of the H_STUNNER
void tempStunnerLocation() {
	static int lastRow = 14;
	static int lastCol = 1;
	//TODO - will have to have a basic init grid always for replacing
	putCharToInnerGrid(lastRow, lastCol, GRASS);
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
				putImageToCell(r, c, tree_texId, 2);
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
