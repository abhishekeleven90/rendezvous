#include "Headers.h"
#include "Objects.h"

GLfloat getXFromCell(int col);
GLfloat getYFromCell(int row);
void putCharToGrid(int row, int col, charCellType charType);
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

void putCharToGrid(int row, int col, charCellType charType) {
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

GLuint grass_textureId;
GLuint tree_textureId;

void renderGrid() {
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_GRID_COL; c <= END_GRID_COL; c++) {
			switch (gridChar[r][c]) {
			case GRASS:
				putImageToCell(r, c, grass_textureId);
				break;
			case SPAWN:
				break;
			case ANGELS_TEMPLE:
				break;
			case DEMONS_TEMPLE:
				break;
			case H_STUNNER:
				break;
			case H_SILENCER:
				break;
			case H_SNATCHER:
				break;
			case H_LEECHER:
				break;
			case STONE:
				break;
			case TREE:
				putImageToCell(r, c, tree_textureId);
				break;
			}
		}
	}
}
