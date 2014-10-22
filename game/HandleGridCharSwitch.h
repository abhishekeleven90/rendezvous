#ifndef HANDLE_GRID_CHAR_SWITCH_H
#define HANDLE_GRID_CHAR_SWITCH_H

#include "SoundAll.h"
#include "Globals.h"
#include "Objects.h"
#include "Constants.h"
#include "CustomVectorStruct.h"

enum switchCallType {
	PRINT_GRID, PROCESS_MOVE_CLICK, RENDER_GRID
};

Coordinate_openGl getOpenGlCoordinatesFromGrid(Coordinate_grid grid) {
	GLfloat x = MIN_XCELL + (grid.col - 1) * CELL_LENGTH;
	GLfloat y = MAX_YCELL - (grid.row * CELL_LENGTH);
	return Coordinate_openGl(x, y);
}

void putImageToGrid(GLfloat x, GLfloat y, GLuint _textureId, int blocks) {
	GLfloat size1D = blocks * CELL_LENGTH;
	glEnable( GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBegin( GL_QUADS);

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

void putImageToCell(int row, int col, GLuint _textureId, int blocks) {
	Coordinate_grid grid = Coordinate_grid(row, col);
	Coordinate_openGl openGl = getOpenGlCoordinatesFromGrid(grid);
	putImageToGrid(openGl.x, openGl.y, _textureId, blocks);
}

void processCase(switchCallType callType, int r, int c, GLuint texId,
		string toPrint, void( f)(void), bool isBackChar, int blocks = 1) {

	switch (callType) {
	case PRINT_GRID:
		cout << " " << toPrint;
		break;
	case PROCESS_MOVE_CLICK:
		f();
		break;
	case RENDER_GRID:
		if (!isBackChar) {
			putImageToCell(r, c, texId, blocks);
		}
		break;
	}
}

void wrong() {
	playEventSound(PATH_SOUND_WRONG_CLICK);
}

void f() {

}

void handleGridCharSwitch(int r, int c, switchCallType callType) {

	switch (gridChar[r][c]) {
	case BG_GRASS:
		processCase(callType, r, c, grass_texId, "Gra", f, false);
		break;
	case BG_SPAWN:
		processCase(callType, r, c, spawn_texId, "BSp", f, false);
		break;
	case BG_WAR:
		processCase(callType, r, c, war_texId, "BWa", f, false);
		break;
	case BG_ATTRIBUTE:
		processCase(callType, r, c, attribute_bg_texId, "BAt", wrong, false);
		break;

	case STONE:
		processCase(callType, r, c, stone_texId, "Sto", wrong, false);
		break;
	case TREE:
		processCase(callType, r, c, tree_texId, "Tre", wrong, false);
		break;

	case TEMPLE_ANGELS:
		processCase(callType, r, c, t_angels_texId, "TAn", wrong, false,
				TEMPLE_BLOCKS);
		break;
	case TEMPLE_DEMONS:
		processCase(callType, r, c, t_demons_texId, "TDe", wrong, false,
				TEMPLE_BLOCKS);
		break;

	case H_DISABLER:
		processCase(callType, r, c, h_disabler_texId, "HDi", wrong, false);
		break;
	case H_SLOWER:
		processCase(callType, r, c, h_slower_texId, "HSl", wrong, false);
		break;
	case H_BURSTER:
		processCase(callType, r, c, h_burster_texId, "HBu", wrong, false);
		break;
	case H_STUNNER:
		processCase(callType, r, c, h_stunner_texId, "HSt", wrong, false);
		break;

	case I_SPEED_MOVE:
		processCase(callType, r, c, i_speedMov_texId, "ISM", f, false);
		break;
	case I_SPEED_ATTACK:
		processCase(callType, r, c, i_speedAttack_texId, "ISA", f, false);
		break;
	case I_HEALTH:
		processCase(callType, r, c, i_health_texId, "IHe", f, false);
		break;
	case I_DAMAGE:
		processCase(callType, r, c, i_damage_texId, "IDa", f, false);
		break;
	case I_TEMPLE_HEALER:
		processCase(callType, r, c, i_templeHealer_texId, "ITH", f, false);
		break;

	case TREE_BACK:
		processCase(callType, r, c, temp_texId, "TBa", wrong, true);
		break;
	case T_ANGELS_BACK:
		processCase(callType, r, c, temp_texId, "TAB", wrong, true);
		break;
	case T_DEMONS_BACK:
		processCase(callType, r, c, temp_texId, "TDB", wrong, true);
		break;

	default:
		cout << "should not happen - something's wrong";
	}
}

#endif
