#ifndef OBJECTS_H
#define OBJECTS_H

#include "Headers.h"
#include "AStarClass.h"

#define IP_SIZE 40

struct GameDetails {
	bool isSinglePlayerGame;
	bool isHost;
};

struct nodeHelper {
	char ip[IP_SIZE];
	unsigned int port;
	char ipWithPort[IP_SIZE];
};

enum charCellType {
	BG_GRASS, BG_SPAWN, BG_WAR, BG_ATTRIBUTE, BG_BLOCKED,

	STONE, TREE, STONE_WAR, TREE_WAR,

	TEMPLE_ANGELS, TEMPLE_DEMONS,

	H_DISABLER, H_SLOWER, H_BURSTER, H_STUNNER,

	I_SPEED_MOVE, I_HEALTH, I_DAMAGE, I_TEMPLE_HEALER,

	TREE_BACK, T_ANGELS_BACK, T_DEMONS_BACK,

	CLICK_PLAYER_SINGLE, CLICK_PLAYER_MULTI,

	CLICK_HELP, CLICK_BACK,

	CLICK_ANGELS, CLICK_DEMONS, CLICK_PICK_TEAM,

	CLICK_HOST_GAME, CLICK_JOIN_GAME, CLICK_NEXT,

	INPUT1, INPUT2, INPUT3, INPUT4, INPUT5
};

charCellType itemCharCell[] = { I_SPEED_MOVE, I_HEALTH, I_DAMAGE,
		I_TEMPLE_HEALER };

enum heroes {
	HERO_DISABLER, HERO_SLOWER, HERO_BURSTER, HERO_STUNNER
};

enum curse {
	CURSE_DISABLE, CURSE_SLOW, CURSE_BURST, CURSE_STUN, CURSE_NONE
};

//TODO : check if required
enum itemType {
	ITEM_SPEED_MOVE, ITEM_HEALTH, ITEM_DAMAGE, ITEM_TEMPLE_HEALER
};

enum powerMode {
	POWER_MODE_BASIC, POWER_MODE_MAGIC, POWER_MODE_STUN
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

};

struct TeamStruct {
	teamName name;
	int templeHealth;
};

enum clientStatus {
	CLIENT_ALIVE, CLIENT_DEAD, CLIENT_NOT_REACHABLE
};

struct Player {
	//int playerId;
	clientStatus status;

	nodeHelper* networkDetails;

	heroes heroType;

	charCellType charType;

	TeamStruct* team;

	powerMode currPowerMode;

	//attributes
	int heroHealth;
	int strength;
	int speedMove;

	//related to curse
	int speedMoveTemp; //used to save speed to restore back after curse is over
	curse curseType;

	//related to location & move
	AStarClass *astar;
	Coordinate_grid location;

	bool toAttackTemple;
	int whichEnemyPlayerToAttack;

	//for items
	Coordinate_grid targetCell;
	Coordinate_grid itemCell;

	//timers
	int isTimerItemGlobalRunning;
	int isTimerMagicSpellRunning;
	int isTimerCurseRunning;
	bool isHeroRebornTimer;

	//some extra info to help
	bool isFirstPlayerInTeam; //denotes whether I am the first player in my team to join..right now required for location
	int idFriend;
	int idEnemy[2];

	bool atleastOnceAstar;
	bool aStarThrough;
};

Coordinate_grid onClickTargetCell;

#endif
