#include "Headers.h"
#include "ConstantsNGlobals.h"

char gridChar[END_GRID_ROW - START_GRID_ROW + 2][END_OUTER_GRID_COL
		- START_OUTER_GRID_COL + 2]; //+1 extra since taking 1-20

enum charCellType {
	GRASS,
	SPAWN,
	ATTRIBUTE_BG,
	ANGELS_TEMPLE,
	DEMONS_TEMPLE,
	H_STUNNER,
	H_SILENCER,
	H_SNATCHER,
	H_LEECHER,
	STONE,
	TREE
};

enum Heroes {
	STUNNER, SILENCER, SNATCHER, LEECHER
};

enum MagicPower {
	STUN, SILENCE, SNATCH, LEECH
};

enum ItemType {
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

enum TeamName {
	ANGELS, DEMONS
};

struct Item {
	ItemType item_type;
	int ItemUsetimer;

	// Used for one-time-items (like maybe Strength - don't want to have again)
	// Low priority, remove if not required
	bool isOneTimeItem;
	bool isDisplayed;
};

struct Temple {
	TeamName team_name;
	int health;
};

struct Hero {
	TeamName team_name;
	//string pic_path;

	int health;

	int pistol_ammo;

	MagicPower magic_power;
	int magicSpellTime;

	//TODO - pending item attributes

};

