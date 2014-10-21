#include "Headers.h"
#include "ConstantsNGlobals.h"

enum charCellType {
	GRASS,
	SPAWN,
	WAR_GROUND,
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

charCellType
		initialGridChar[END_GRID_ROW - START_GRID_ROW + 2][END_OUTER_GRID_COL
				- START_OUTER_GRID_COL + 2]; //+1 extra since starting from 1

enum heroes {
	DISABLER, SLOWER, BURSTER, STUNNER
};

enum magicPower {
	DISABLE, SLOW, BURST, STUN
};

enum itemType {
	SPEED_MOVE, SPEED_ATTACK, HEALTH, DAMAGE, TEMPLE_HEALER
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
	string pic_path;

	magicPower magic_power;

	int strength;
	int health;
	int speedAttack;
	int speedMove;

	itemType itemsBag[];

	int magicSpellTime;
	int globalItemTime;
};

