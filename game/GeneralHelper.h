#ifndef GENERAL_HELPER_H
#define GENERAL_HELPER_H

#include "Globals.h"
#include "OpenGlHelper.h"
#include "FilePaths.h"
#include "CustomVectorStruct.h"
#include "Constants.h"
#include "Objects.h"
#include "MapNGrid.h"
#include "HandleGridCharSwitch.h"

void loadTextures() {
	grass_texId = getTextureFromImage(PATH_IMG_BG);
	spawn_texId = getTextureFromImage(PATH_IMG_SPAWN);
	war_texId = getTextureFromImage(PATH_IMG_WAR_GND);

	attribute_bg_texId = getTextureFromImage(PATH_IMG_ATTRIBUTE_BG);

	tree_texId = getTextureFromImage(PATH_IMG_TREE);
	stone_texId = getTextureFromImage(PATH_IMG_STONE);

	t_demons_texId = getTextureFromImage(PATH_IMG_TEMPLE_DEMONS);

	h_slower_texId = getTextureFromImage(PATH_IMG_HERO_MARIO);
	h_burster_texId = getTextureFromImage(PATH_IMG_HERO_SOLDIER);

	i_speedMov_texId = getTextureFromImage(PATH_IMG_ITEM_MOVEMENT_SPEED);
	i_speedAttack_texId = getTextureFromImage(PATH_IMG_ITEM_ATTACK_SPEED);
	i_damage_texId = getTextureFromImage(PATH_IMG_ITEM_DAMAGE);
	i_health_texId = getTextureFromImage(PATH_IMG_ITEM_HEALTH);
	i_templeHealer_texId = getTextureFromImage(PATH_IMG_ITEM_TEMPLE_HEALER);
}

Coordinate_grid getRandomCoordinatesForItem() {
	int randomRow;
	int randomCol;

	while (true) {
		randomRow = (rand() % (END_GRID_ROW - START_GRID_ROW + 1)) + 1; //1 extra since we want it to start from 1
		randomCol = (rand() % (END_INNER_GRID_COL - START_INNER_GRID_COL + 1))
				+ 1;

		if (getInnerGridChar(randomRow, randomCol) == BG_GRASS) { //assuming that items can come only on 'grass'
			break;
		}
	}
	return Coordinate_grid(randomRow, randomCol);
}

void placeItemAtRandomPos() {
	Coordinate_grid grid = getRandomCoordinatesForItem();
	putCharToGrid(grid.row, grid.col, itemCharCell[global_item_index], true);
	putCharToGrid(grid.col, grid.row, itemCharCell[global_item_index], true);
	global_item_index = (global_item_index + 1) % ARRAY_SIZE(itemCharCell);
}

void printGrid() {
	cout << "Printing grid" << "\n";
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			handleGridCharSwitch(r, c, PRINT_GRID);
		}
		cout << "\n";
	}
}

#endif
