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

void loadPlayerSpecificAttributes() {
	//TODO: the below attributes shall be coming from earlier screens
	playerStats.team = TEAM_ANGELS;
	playerStats.heroType = HERO_SLOWER;

	playerStats.currentPower = POWER_BASIC;
	playerStats.heroHealth = HEALTH_FULL_HERO;
	playerStats.templeHealth = HEALTH_FULL_TEMPLE;

	switch (playerStats.heroType) {
	//TODO : decide & change attributes - strength,speedAttack,speedMove etc
	case HERO_STUNNER:
		playerStats.pic_path = PATH_IMG_HERO_STUNNER;
		playerStats.magic_power = MAGIC_STUN;
		playerStats.strength = STRENGTH_H_STUNNER;
		playerStats.speedAttack = SPEED_ATTACK_H_STUNNER;
		playerStats.speedMove = SPEED_MOVE_H_STUNNER;
		break;

	case HERO_DISABLER:
		playerStats.pic_path = PATH_IMG_HERO_DISABLER;
		playerStats.magic_power = MAGIC_DISABLE;
		playerStats.strength = STRENGTH_H_DISABLER;
		playerStats.speedAttack = SPEED_ATTACK_H_DISABLER;
		playerStats.speedMove = SPEED_MOVE_H_DISABLER;
		break;

	case HERO_SLOWER:
		playerStats.pic_path = PATH_IMG_HERO_SLOWER;
		playerStats.magic_power = MAGIC_SLOW;
		playerStats.strength = STRENGTH_H_SLOWER;
		playerStats.speedAttack = SPEED_ATTACK_H_SLOWER;
		playerStats.speedMove = SPEED_MOVE_H_SLOWER;
		break;

	case HERO_BURSTER:
		playerStats.pic_path = PATH_IMG_HERO_BURSTER;
		playerStats.magic_power = MAGIC_BURST;
		playerStats.strength = STRENGTH_H_BURSTER;
		playerStats.speedAttack = SPEED_ATTACK_H_BURSTER;
		playerStats.speedMove = SPEED_MOVE_H_BURSTER;
		break;
	}
}

void loadTextures() {
	bg_grass_texId = getTextureFromImage(PATH_IMG_BG);
	bg_spawn_texId = getTextureFromImage(PATH_IMG_BG_SPAWN);
	bg_war_texId = getTextureFromImage(PATH_IMG_BG_WAR);
	bg_attribute_texId = getTextureFromImage(PATH_IMG_BG_ATTRIBUTE);

	tree_texId = getTextureFromImage(PATH_IMG_TREE);
	stone_texId = getTextureFromImage(PATH_IMG_STONE);

	t_demons_texId = getTextureFromImage(PATH_IMG_TEMPLE_DEMONS);

	//TODO: uncomment
	//h_stunner_texId = getTextureFromImage(PATH_IMG_HERO_STUNNER);
	//h_disabler_texId = getTextureFromImage(PATH_IMG_HERO_DISABLER);
	h_slower_texId = getTextureFromImage(PATH_IMG_HERO_SLOWER);
	h_burster_texId = getTextureFromImage(PATH_IMG_HERO_BURSTER);

	i_speedMov_texId = getTextureFromImage(PATH_IMG_ITEM_MOVEMENT_SPEED);
	i_speedAttack_texId = getTextureFromImage(PATH_IMG_ITEM_ATTACK_SPEED);
	i_damage_texId = getTextureFromImage(PATH_IMG_ITEM_DAMAGE);
	i_health_texId = getTextureFromImage(PATH_IMG_ITEM_HEALTH);
	i_tHealer_texId = getTextureFromImage(PATH_IMG_ITEM_TEMPLE_HEALER);
}

void printGrid() {
	cout << "Printing grid" << "\n";
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			handleGridCharSwitch(Coordinate_grid(r, c), PRINT_GRID);
		}
		cout << "\n";
	}
}

void renderGrid() {
	//TODO - remove this method call from here and put in a thread or something
	tempStunnerLocation();
	moveHeroMine(1);

	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			handleGridCharSwitch(Coordinate_grid(r, c), RENDER_GRID);
		}
	}
}

bool isValidCellForTeam(Coordinate_grid grid) {
	if (playerStats.team == TEAM_ANGELS && !(grid.col >= grid.row + DIAG_BLOCKS
			+ 1)) {
		return true;
	}

	if (playerStats.team == TEAM_DEMONS && !(grid.row >= grid.col + DIAG_BLOCKS
			+ 1)) {
		return true;
	}

	return false;
}

#endif
