#ifndef OBJECTS_H
#define OBJECTS_H

#include "Headers.h"

enum charCellType {
	BG_GRASS, BG_SPAWN, BG_WAR, BG_ATTRIBUTE, BG_BLOCKED,

	STONE, TREE,

	TEMPLE_ANGELS, TEMPLE_DEMONS,

	H_DISABLER, H_SLOWER, H_BURSTER, H_STUNNER,

	I_SPEED_MOVE, I_HEALTH, I_DAMAGE, I_TEMPLE_HEALER,

	TREE_BACK, T_ANGELS_BACK, T_DEMONS_BACK
};

charCellType itemCharCell[] = { I_SPEED_MOVE, I_HEALTH, I_DAMAGE,
		I_TEMPLE_HEALER };

enum heroes {
	HERO_DISABLER, HERO_SLOWER, HERO_BURSTER, HERO_STUNNER
};

enum magicPower {
	MAGIC_DISABLE, MAGIC_SLOW, MAGIC_BURST, MAGIC_STUN
};

//TODO : check if required
enum itemType {
	ITEM_SPEED_MOVE, ITEM_HEALTH, ITEM_DAMAGE, ITEM_TEMPLE_HEALER
};

enum heroPower {
	POWER_BASIC, POWER_MAGIC
};

//Angels - left, Demons - right
enum teamName {
	TEAM_ANGELS, TEAM_DEMONS, TEAM_BOTH
};

//TODO: check if required
struct Item {
public:
	Item() {

	}

	Item(itemType type) {
		item_type = type;
	}

	itemType item_type;

	//TODO: check if required
	//int itemUsetimer;
};

struct Temple {
	teamName team_name;
	int health;
};

struct Player {
	teamName team;
	heroes heroType;

	string pic_path;
	magicPower magic_power;

	heroPower currentPower;
	int heroHealth;
	int templeHealth;

	int strength;
	int speedMove;

	//list<itemType*> itemsBag;
};

#endif
