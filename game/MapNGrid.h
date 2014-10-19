#include "Headers.h"
#include "Objects.h"

void initMap() {

}

void putImageToGrid(int x, int y) {

}

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
