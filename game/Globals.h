#ifndef GLOBALS_H
#define GLOBALS_H

#include "Objects.h"
#include "Constants.h"

charCellType gridChar[END_GRID_ROW - START_GRID_ROW + 2][END_OUTER_GRID_COL
		- START_OUTER_GRID_COL + 2]; //+1 extra since starting from 1

charCellType
		initialGridChar[END_GRID_ROW - START_GRID_ROW + 2][END_OUTER_GRID_COL
				- START_OUTER_GRID_COL + 2]; //+1 extra since starting from 1

int g_item_index_angels = 0; //holds the global index of next item to be placed
int g_item_index_demons = 0;

//TextureIds
GLuint grass_texId;
GLuint spawn_texId;
GLuint war_texId;

GLuint attribute_bg_texId;

GLuint tree_texId;
GLuint stone_texId;

GLuint t_angels_texId; //temples
GLuint t_demons_texId;

GLuint h_disabler_texId;
GLuint h_slower_texId; //hero_mario
GLuint h_burster_texId; //hero_soldier
GLuint h_stunner_texId;

GLuint i_speedMov_texId;
GLuint i_speedAttack_texId;
GLuint i_damage_texId;
GLuint i_health_texId;
GLuint i_tHealer_texId;

GLuint temp_texId;

#endif
