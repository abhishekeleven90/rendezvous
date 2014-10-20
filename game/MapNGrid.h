#include "Headers.h"
#include "Objects.h"

GLfloat getXFromCell(int col);
GLfloat getYFromCell(int row);
void putCharToInnerGrid(int row, int col, charCellType charType);
void putImageToGrid(GLfloat x, GLfloat y, GLuint _textureId);
void renderGrid();

GLfloat getXFromCell(int col) {
	GLfloat x = MIN_XCELL + (col - 1) * CELL_LENGTH;
	return x;
}

GLfloat getYFromCell(int row) {
	GLfloat y = MAX_YCELL - (row * CELL_LENGTH);
	return y;
}

void putCharToInnerGrid(int row, int col, charCellType charType) {
	row = row;
	col = col + ATTRIBUTE_WIDTH;
	gridChar[row][col] = charType;
}

//Avoid using : use only in case of attributeSpace
//TODO: check whether we can have a specific function for attribute space
void putCharToOuterGrid(int row, int col, charCellType charType) {
	gridChar[row][col] = charType;
}

void putImageToCell(int row, int col, GLuint _textureId) {
	GLfloat x = getXFromCell(col);
	GLfloat y = getYFromCell(row);
	putImageToGrid(x, y, _textureId);
}

void putImageToGrid(GLfloat x, GLfloat y, GLuint _textureId) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(x, y, -5.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(x, y + CELL_LENGTH, -5.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(x + CELL_LENGTH, y + CELL_LENGTH, -5.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(x + CELL_LENGTH, y, -5.0f);

	glEnd();
}

void renderGrid() {
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
			case ANGELS_TEMPLE:
				putImageToCell(r, c, t_angels_texId);
				break;
			case DEMONS_TEMPLE:
				putImageToCell(r, c, t_demons_texId);
				break;
			case H_STUNNER:
				putImageToCell(r, c, h_stunner_texId);
				break;
			case H_SILENCER:
				putImageToCell(r, c, h_silencer_texId);
				break;
			case H_SNATCHER:
				putImageToCell(r, c, h_snatcher_texId);
				break;
			case H_LEECHER:
				putImageToCell(r, c, h_leecher_texId);
				break;
			case STONE:
				putImageToCell(r, c, stone_texId);
				break;
			case TREE:
				putImageToCell(r, c, tree_texId);
				break;
			default:
				cout << "--SOMETHING WENT WRONG while rendering grid--" << endl;
			}
		}
	}
}
