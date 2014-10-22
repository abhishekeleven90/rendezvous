#ifndef OBJECTS_H
#define OBJECTS_H

enum charCellType {
	BG_GRASS, BG_SPAWN, BG_WAR, BG_ATTRIBUTE,

	STONE, TREE,

	TEMPLE_ANGELS, TEMPLE_DEMONS,

	H_DISABLER, H_SLOWER, H_BURSTER, H_STUNNER,

	I_SPEED_MOVE, I_SPEED_ATTACK, I_HEALTH, I_DAMAGE, I_TEMPLE_HEALER,

	TREE_BACK, T_ANGELS_BACK, T_DEMONS_BACK
};

charCellType itemCharCell[] = { I_SPEED_MOVE, I_SPEED_ATTACK, I_HEALTH,
		I_DAMAGE, I_TEMPLE_HEALER };

enum heroes {
	DISABLER, SLOWER, BURSTER, STUNNER
};

enum magicPower {
	DISABLE, SLOW, BURST, STUN
};

//TODO : check if required
enum itemType {
	SPEED_MOVE, SPEED_ATTACK, HEALTH, DAMAGE, TEMPLE_HEALER, ITEM_COUNT
//ITEM_COUNT helps to get the number of items, no other purpose
};

enum teamName {
	ANGELS, DEMONS
};

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

#endif
