#include "Headers.h"
#include "ConstantsNGlobals.h"

enum charCellType {
	GRASS,
	SPAWN,
	ATTRIBUTE_BG,
	TEMPLE_ANGELS,
	TEMPLE_DEMONS,
	H_DISABLER,
	H_SLOWER,
	H_BURSTER,
	H_STUNNER,
	STONE,
	TREE,
	TREE_BACK,
	T_ANGELS_BACK,
	T_DEMONS_BACK,
	DEFAULT
};

charCellType gridChar[END_GRID_ROW - START_GRID_ROW + 2][END_OUTER_GRID_COL
		- START_OUTER_GRID_COL + 2]; //+1 extra since starting from 1

enum heroes {
	DISABLER, SLOWER, BURSTER, STUNNER
};

enum magicPower {
	DISABLE, SLOW, BURST, STUN
};

enum itemType {
	PISTOL_AMMO,
	STRENGTH,
	HEALTH,
	TELEPORT,
	BOMB,
	GOD_MODE,
	SPEED,
	TEMPLE_HEALER,
	ATTACK_RANGE
};

enum teamName {
	ANGELS, DEMONS
};

struct Item {
	itemType item_type;
	int itemUsetimer;
};

struct Temple {
	teamName team_name;
	int health;
};

struct Hero {
	teamName team_name;
	//string pic_path;

	int health;

	int pistol_ammo;

	magicPower magic_power;
	int magicSpellTime;

	//TODO - pending item attributes

};

