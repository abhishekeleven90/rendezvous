#include "Headers.h"
#include "ConstantsNGlobals.h"

enum charCellType {
	GRASS,
	SPAWN,
	ATTRIBUTE_BG,
	TEMPLE_ANGELS,
	TEMPLE_DEMONS,
	H_STUNNER,
	H_SILENCER,
	H_SNATCHER,
	H_LEECHER,
	STONE,
	TREE,
	TREE_BACK,
	T_ANGELS_BACK,
	T_DEMONS_BACK,
	DEFAULT
};

charCellType gridChar[END_GRID_ROW - START_GRID_ROW + 2][END_OUTER_GRID_COL
		- START_OUTER_GRID_COL + 2]; //+1 extra since starting from 1

charCellType initialGridChar[END_GRID_ROW - START_GRID_ROW + 2][END_OUTER_GRID_COL
		- START_OUTER_GRID_COL + 2]; //+1 extra since starting from 1

enum heroes {
	STUNNER, SILENCER, SNATCHER, LEECHER
};

enum magicPower {
	STUN, SILENCE, SNATCH, LEECH
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
	int ItemUsetimer;

	// Used for one-time-items (like maybe Strength - don't want to have again)
	// Low priority, remove if not required
	bool isOneTimeItem;
	bool isDisplayed;
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

