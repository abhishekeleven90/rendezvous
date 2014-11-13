#ifndef HANDLE_GRID_CHAR_SWITCH_H
#define HANDLE_GRID_CHAR_SWITCH_H

#include "SoundAll.h"
#include "Globals.h"
#include "Objects.h"
#include "Constants.h"
#include "CustomVectorStruct.h"
#include "MapNGrid.h"
#include "Timer.h"
#include "OneMoreFile.h"
#include "Network.h"

void setItemCell(Coordinate_grid movingToCell, int whichPlayer) {
	players[whichPlayer].itemCell.row = movingToCell.row;
	players[whichPlayer].itemCell.col = movingToCell.col;
}

//if more items add or try to add to the complex handleGridCharSwitch
bool isItem(Coordinate_grid cell) {
	switch (gridCharPrimary[cell.row][cell.col]) {
	case I_DAMAGE:
	case I_HEALTH:
	case I_SPEED_MOVE:
	case I_TEMPLE_HEALER:
		return true;
		break;
	default:
		return false;
	}
	return false;
}

Coordinate_grid getRandomCoordinatesForItem(teamName name, bool isPrimary) {
	int randomRow;
	int randomCol;

	while (true) {
		randomRow = (rand() % (END_GRID_ROW - START_GRID_ROW + 1)) + 1; //1 extra since we want it to start from 1
		randomCol = (rand() % (END_INNER_GRID_COL - START_INNER_GRID_COL + 1))
				+ 1;

		if (name == TEAM_ANGELS && randomRow < randomCol) {
			continue;
		}

		if (name == TEAM_DEMONS && randomRow > randomCol) {
			continue;
		}

		if (getGridChar(randomRow, randomCol, true, isPrimary) == BG_GRASS) { //assuming that items can come only on 'grass'
			break;
		}
	}

	return Coordinate_grid(randomRow, randomCol);
}

void placeItemAtRandomPos(teamName name) {
	Coordinate_grid grid = getRandomCoordinatesForItem(name, true); //passed 'isPrimary' as true since changing the primaryGrid
	int r = grid.row;
	int c = grid.col;

	switch (name) {
	case TEAM_ANGELS:
		putChar2Grid(r, c, itemCharCell[g_item_index_angels++], true, true);
		break;

	case TEAM_DEMONS:
		putChar2Grid(r, c, itemCharCell[g_item_index_demons++], true, true);
		break;

	case TEAM_BOTH:
		cout << "---placeItemAtRandomPos: shall not reach here ever" << endl;
		break;
	}

	g_item_index_angels = g_item_index_angels % ARRAY_SIZE(itemCharCell);
	g_item_index_demons = g_item_index_demons % ARRAY_SIZE(itemCharCell);
}

void initItemAtRandomPos() {
	Coordinate_grid grid = getRandomCoordinatesForItem(TEAM_BOTH, false); //passed 'isPrimary' as false since changing the normal grid
	int r = grid.row;
	int c = grid.col;

	if (r > c) {
		putChar2Grid(r, c, itemCharCell[g_item_index_angels++], true, false);
		putChar2Grid(c, r, itemCharCell[g_item_index_demons++], true, false);
	} else {
		putChar2Grid(r, c, itemCharCell[g_item_index_demons++], true, false);
		putChar2Grid(c, r, itemCharCell[g_item_index_angels++], true, false);
	}

	g_item_index_angels = g_item_index_angels % ARRAY_SIZE(itemCharCell);
	g_item_index_demons = g_item_index_demons % ARRAY_SIZE(itemCharCell);
}

enum switchCallType {
	PRINT_GRID, PROCESS_MOVE_RIGHT_CLICK, PROCESS_MOVE_LEFT_CLICK, RENDER_GRID
};

void sendServerMoveThrough() {
	helperSendServerMove();
}

void processCase(switchCallType callType, Coordinate_grid grid, GLuint texId,
		string toPrint, void( rightClick)(void), void( leftClick)(void),
		bool isBackChar, int xBlocks = 1, int yBlocks = 1) {

	switch (callType) {
	case PRINT_GRID:
		cout << " " << toPrint;
		break;

	case PROCESS_MOVE_RIGHT_CLICK:
		rightClick();
		break;

	case PROCESS_MOVE_LEFT_CLICK:
		//cout << "left click at grass" << endl;
		leftClick();
		break;

	case RENDER_GRID:
		if (!isBackChar) {
			putBmpToCell(grid, texId, xBlocks, yBlocks);
		}
		break;
	}
}

void wrong() {
	playEventSound(PATH_SOUND_WRONG_CLICK);
}

itemType getItemTypeFromCharItem(Coordinate_grid cellForChar) {
	charCellType charItem = gridCharPrimary[cellForChar.row][cellForChar.col];
	itemType item;
	switch (charItem) {
	case I_DAMAGE:
		item = ITEM_DAMAGE;
		break;
	case I_HEALTH:
		item = ITEM_HEALTH;
		break;
	case I_SPEED_MOVE:
		item = ITEM_SPEED_MOVE;
		break;
	case I_TEMPLE_HEALER:
		item = ITEM_TEMPLE_HEALER;
		break;
	}
	return item;
}

void updateHeroAttributesTakingItem(int playerId) {
	itemType itemTaken = getItemTypeFromCharItem(players[playerId].itemCell);
	switch (itemTaken) {
	case ITEM_DAMAGE:
		cout << "item_damage taken by player: " << playerId << endl;
		players[playerId].strength += GAIN_ITEM_DAMAGE;
		break;

	case ITEM_HEALTH:
		cout << "item_health taken by player: " << playerId << endl;
		players[playerId].heroHealth += GAIN_ITEM_HEALTH;
		break;

	case ITEM_SPEED_MOVE:
		cout << "item_speed_move taken by player: " << playerId << endl;
		players[playerId].speedMove += GAIN_ITEM_SPEED_MOVE;

		if (players[playerId].speedMove >= SPEED_MAX) {
			//TODO: notification
			cout << "Max Speed Reached" << endl;
			players[playerId].speedMove = SPEED_MAX;
		}
		break;

	case ITEM_TEMPLE_HEALER:
		cout << "item_temple_healer taken by player: " << playerId << endl;
		switch (players[playerId].team->name) {
		case TEAM_ANGELS:
			angelsTeam.templeHealth += GAIN_ITEM_TEMPLE_HEALER;
			break;
		case TEAM_DEMONS:
			demonsTeam.templeHealth += GAIN_ITEM_TEMPLE_HEALER;
			break;
		}
		break;

	default:

		cout << "---updateHeroAttributesTakingItem: shall not come here---";
		cout << itemTaken << endl;
		break;
	}
}

void takeItem(int whichPlayer) {

	if (!players[whichPlayer].isTimerItemGlobalRunning) {
		timerItemGlobal(whichPlayer);
		updateHeroAttributesTakingItem(whichPlayer);
	} else {
		cout << "Item not taken by player: " << whichPlayer << endl;
	}

	//Irrespective of the GLOBAL_ITEM_TIMER, a new item is displayed at random pos
	putChar2Grid(players[whichPlayer].itemCell.row,
			players[whichPlayer].itemCell.col, BG_GRASS, false, true);

	if (players[whichPlayer].team->name == TEAM_ANGELS) {
		placeItemAtRandomPos(TEAM_ANGELS);
	}
	if (players[whichPlayer].team->name == TEAM_DEMONS) {
		placeItemAtRandomPos(TEAM_DEMONS);
	}

}

//--------------TEMPLE METHODS-------------------

//only to be called by server, moveHero()
void decreaseEnemyTempleHealth(int whichPlayer) { //this is ok, use it
	TeamStruct* enemyTeam;
	if (players[whichPlayer].team->name == TEAM_ANGELS)
		enemyTeam = &demonsTeam;
	else
		enemyTeam = &angelsTeam;

	enemyTeam->templeHealth -= players[whichPlayer].strength;

	if (enemyTeam->templeHealth <= 0) {
		enemyTeam->templeHealth = 0;
		isGameOver = true;
		winningTeam = players[whichPlayer].team->name;

		//------------------------------------Change for single player (start)----------
		if (gameDetails.isSinglePlayerGame) {
			processGameOver(numToChar(winningTeam));
		}
		//------------------------------------Change for single player (end)----------
	}
	setAttackTemple(whichPlayer, false);
	//TODO: Abhishek play temple attack sound, 	gif attack animation


}

void decreaseEnemyPlayerHealthHelper(int whichPlayer, bool burstDamage) {

	int enemyPlayer = players[whichPlayer].whichEnemyPlayerToAttack;

	if (!burstDamage)
		players[enemyPlayer].heroHealth -= players[whichPlayer].strength;
	else
		players[enemyPlayer].heroHealth -= CURSE_AMT_BURST_DAMAGE;

	if (players[enemyPlayer].heroHealth <= 0) {
		//reborn logic
		players[enemyPlayer].heroHealth = 0;
		giveBirthToPlayer(enemyPlayer, true);//reborn
		//reborn
		timerHeroBorn(enemyPlayer);
	}
	setAttackEnemyPlayer(whichPlayer, -1);
	//TODO: Abhishek play enemy attack sound, gif attack animation


}

//used only by master node
void curseThePlayer(int byWhichPlayer, int onWhichPlayer) {
	switch (players[byWhichPlayer].heroType) {
	case HERO_STUNNER:
		cout << byWhichPlayer << " stunned " << onWhichPlayer << endl;
		players[onWhichPlayer].curseType = CURSE_STUN;
		timerCurse(onWhichPlayer);
		break;

	case HERO_DISABLER:
		cout << byWhichPlayer << " disabled " << onWhichPlayer << endl;
		players[onWhichPlayer].curseType = CURSE_DISABLE;
		timerCurse(onWhichPlayer);
		break;

	case HERO_SLOWER:
		cout << byWhichPlayer << " slowed " << onWhichPlayer << endl;
		players[onWhichPlayer].curseType = CURSE_WEAK;
		timerCurse(onWhichPlayer);
		break;

	case HERO_BURSTER:
		cout << byWhichPlayer << " bursted " << onWhichPlayer << endl;
		players[onWhichPlayer].curseType = CURSE_BURST;
		timerCurse(onWhichPlayer);
		//works so not removing, but seems redundant code
		players[byWhichPlayer].whichEnemyPlayerToAttack = onWhichPlayer;
		decreaseEnemyPlayerHealthHelper(byWhichPlayer, true);
		break;

	default:
		cout << "---Inside curseThePlayer: shall not come here---" << endl;
		return;
	}
}

//only to be called by server, moveHero()
void decreaseEnemyPlayerHealth(int whichPlayer) { //this is ok, use it
	int enemyPlayer = players[whichPlayer].whichEnemyPlayerToAttack;

	//magic logic, should be near and left clicked, and magic mode on
	bool toApplyCurse = false;
	if (players[whichPlayer].currPowerMode == POWER_MODE_MAGIC) {
		if (!players[whichPlayer].isTimerMagicSpellRunning)
			toApplyCurse = true;
	}

	if (toApplyCurse) {
		timerMagicSpell(whichPlayer);
		setAttackEnemyPlayer(whichPlayer, -1);
		curseThePlayer(whichPlayer, enemyPlayer);
	} else {
		decreaseEnemyPlayerHealthHelper(whichPlayer, false);
	}
}

//to be called by client
void attackAngelsTemple() {
	//note: validation is done itself at the client side
	if (players[currPlayerId].curseType == CURSE_STUN)
		return;

	if (players[currPlayerId].team->name == TEAM_DEMONS) {
		helperSendAttackTemple();
	}
}

//to be called by client
void attackDemonsTemple() {
	//note: validation is done itself at the client side
	if (players[currPlayerId].curseType == CURSE_STUN)
		return;

	if (players[currPlayerId].team->name == TEAM_ANGELS) {
		helperSendAttackTemple();
	}
}

//to be called by client, currentPlayer, non-primary node
void attackEnemy() {
	if (players[currPlayerId].curseType == CURSE_STUN)
		return;

	//note: validation is done itself at the client side

	//------------------------------------Change for single player (start)----------
	charCellType toAttackHeroCellType;

	if (gameDetails.isSinglePlayerGame) {
		toAttackHeroCellType
				= gridCharPrimary[onClickTargetCell.row][onClickTargetCell.col];
	}

	else {
		toAttackHeroCellType
				= gridChar[onClickTargetCell.row][onClickTargetCell.col];
	}
	//------------------------------------Change for single player (end)----------

	int enemy1 = players[currPlayerId].idEnemy[0];
	int enemy2 = players[currPlayerId].idEnemy[1];
	if (players[enemy1].charType == toAttackHeroCellType) {
		helperSendAttackHero(enemy1);
	} else if (players[enemy2].charType == toAttackHeroCellType) {
		helperSendAttackHero(enemy2);
	}
}

//--------------TEMPLE METHODS-------------------

void handleGridCharSwitch(Coordinate_grid grid, switchCallType callType) {
	//players[currPlayerId].targetCell = grid;
	onClickTargetCell = grid;

	//------------------------------------Change for single player (start)----------
	charCellType charType;
	int row = grid.row;
	int col = grid.col;

	if (gameDetails.isSinglePlayerGame) {
		charType = gridCharPrimary[row][col];

		if (isAttributeSpace(Coordinate_grid(row, col - ATTRIBUTE_WIDTH))) {
			charType = BG_ATTRIBUTE;
		}

		else if (isOponentCellForTeam(
				Coordinate_grid(row, col - ATTRIBUTE_WIDTH), currPlayerId)) {
			charType = BG_BLOCKED;
		}

	}

	else {
		charType = gridChar[row][col];
	}
	//------------------------------------Change for single player (end)----------

	switch (charType) {
	case BG_GRASS:
		processCase(callType, grid, texId_bg_grass, "Gra",
				sendServerMoveThrough, wrong, false);
		break;

	case BG_SPAWN:
		processCase(callType, grid, texId_bg_spawn, "BSp",
				sendServerMoveThrough, wrong, false);
		break;

	case BG_WAR:
		processCase(callType, grid, texId_bg_war, "BWa", sendServerMoveThrough,
				wrong, false);
		break;

	case BG_ATTRIBUTE:
		processCase(callType, grid, texId_bg_attribute, "BAt", wrong, wrong,
				false);
		break;

	case BG_BLOCKED:
		processCase(callType, grid, texId_bg_blocked, "BBl", wrong, wrong,
				false);
		break;

	case STONE:
		processCase(callType, grid, texId_stone, "Sto", wrong, wrong, false);
		break;

	case TREE:
		processCase(callType, grid, texId_tree, "Tre", wrong, wrong, false);
		break;

	case STONE_WAR:
		processCase(callType, grid, texId_stone_war, "StW", wrong, wrong, false);
		break;

	case TREE_WAR:
		processCase(callType, grid, texId_tree_war, "TrW", wrong, wrong, false);
		break;

	case TEMPLE_ANGELS:
		processCase(callType, grid, texId_t_angels, "TAn", wrong,
				attackAngelsTemple, false, TEMPLE_BLOCKS, TEMPLE_BLOCKS);
		break;

	case TEMPLE_DEMONS:
		processCase(callType, grid, texId_t_demons, "TDe", wrong,
				attackDemonsTemple, false, TEMPLE_BLOCKS, TEMPLE_BLOCKS);
		break;

	case H_DISABLER:
		processCase(callType, grid, texId_h_disabler, "HDi", wrong,
				attackEnemy, false);
		break;

	case H_SLOWER:
		processCase(callType, grid, texId_h_slower, "HSl", wrong, attackEnemy,
				false);
		break;

	case H_BURSTER:
		//cout << "burster clicked!!" << endl;
		processCase(callType, grid, texId_h_burster, "HBu", wrong, attackEnemy,
				false);
		break;

	case H_STUNNER:
		processCase(callType, grid, texId_h_stunner, "HSt", wrong, attackEnemy,
				false);
		break;

	case I_SPEED_MOVE:
		processCase(callType, grid, texId_i_speedMov, "ISM",
				sendServerMoveThrough, wrong, false);
		break;

	case I_HEALTH:
		processCase(callType, grid, texId_i_health, "IHe",
				sendServerMoveThrough, wrong, false);
		break;

	case I_DAMAGE:
		//setItemCell(grid, currPlayerId);
		processCase(callType, grid, texId_i_damage, "IDa",
				sendServerMoveThrough, wrong, false);
		break;

	case I_TEMPLE_HEALER:
		//setItemCell(grid, currPlayerId);
		processCase(callType, grid, texId_i_tHealer, "ITH",
				sendServerMoveThrough, wrong, false);
		break;

	case TREE_BACK:
		processCase(callType, grid, temp_texId, "TBa", wrong, wrong, true);
		break;

	case T_ANGELS_BACK:
		processCase(callType, grid, temp_texId, "TAB", wrong,
				attackAngelsTemple, true);
		break;

	case T_DEMONS_BACK:
		processCase(callType, grid, temp_texId, "TDB", wrong,
				attackDemonsTemple, true);
		break;

	default:
		cout << "should not happen - something's wrong" << endl;
	}
}

#endif



------------------------------------------------------------------------------


#ifndef AI_H
#define AI_H

#define LATENCY_HUMAN 600000
#define HEALTH_CRITICAL 30
#define BLOCKS_ITEM 4
#define HEALTH_TEMPLE_RELATIVE_DIFF 100

enum aiMode {
	AI_NORMAL, AI_OFFENSIVE
};

class AI {
	int id;
	Coordinate_grid locStart;
	Coordinate_grid locItem;

	pthread_t threadId;
	bool isMovingToSpwan;
	bool isMovingToTakeItem;

	Coordinate_grid myTempleAttackLocs[8];

public:
	aiMode modeAi;

	AI(int id, Coordinate_grid startLoc, aiMode modeAi) {
		this->id = id;
		this->locStart = startLoc;
		this->modeAi = modeAi;

		this->isMovingToSpwan = false;
		this->isMovingToTakeItem = false;

		if (players[id].team->name == TEAM_ANGELS) {
			myTempleAttackLocs[0] = Coordinate_grid(3, 4);
			myTempleAttackLocs[1] = Coordinate_grid(3, 5);
			myTempleAttackLocs[2] = Coordinate_grid(4, 3);
			myTempleAttackLocs[3] = Coordinate_grid(5, 3);
			myTempleAttackLocs[4] = Coordinate_grid(4, 6);
			myTempleAttackLocs[5] = Coordinate_grid(5, 6);
			myTempleAttackLocs[6] = Coordinate_grid(6, 4);
			myTempleAttackLocs[7] = Coordinate_grid(6, 5);
		} else {
			myTempleAttackLocs[0] = Coordinate_grid(15, 16);
			myTempleAttackLocs[1] = Coordinate_grid(15, 17);
			myTempleAttackLocs[2] = Coordinate_grid(16, 15);
			myTempleAttackLocs[3] = Coordinate_grid(17, 15);
			myTempleAttackLocs[4] = Coordinate_grid(18, 16);
			myTempleAttackLocs[5] = Coordinate_grid(18, 17);
			myTempleAttackLocs[6] = Coordinate_grid(16, 18);
			myTempleAttackLocs[7] = Coordinate_grid(17, 18);
		}
	}

	int getId() {
		return this->id;
	}

	int getRowStart() {
		return this->locStart.row;
	}

	int getColStart() {
		return this->locStart.col + ATTRIBUTE_WIDTH;
	}

	int getRowItem() {
		return this->locItem.row;
	}

	int getColItem() {
		return this->locItem.col;
	}

	void attackEnemyTemple() {
		if (players[getId()].curseType == CURSE_STUN) {
			return;
		}

		if (getId() == 1 && players[getId()].team->name == TEAM_ANGELS) {
			processGeneral("t:16,18?1");
		}

		else if (getId() == 1 && players[getId()].team->name == TEAM_DEMONS) {
			processGeneral("t:4,6?1");
		}

		if (getId() == 2 && players[getId()].team->name == TEAM_ANGELS) {
			processGeneral("t:16,18?2");
		}

		else if (getId() == 2 && players[getId()].team->name == TEAM_DEMONS) {
			processGeneral("t:4,6?2");
		}

		if (getId() == 3 && players[getId()].team->name == TEAM_ANGELS) {
			processGeneral("t:17,19?3");
		}

		else if (getId() == 3 && players[getId()].team->name == TEAM_DEMONS) {
			processGeneral("t:5,7?3");
		}
	}

	void helperAttackEnemy(Coordinate_grid grid, int enemyId) {
		if (players[getId()].curseType == CURSE_STUN) {
			return;
		}

		requestMagicPowerAI(getId());

		string msg = "h:";
		msg = concat(msg, numToStr(grid.row));
		msg = concat(msg, ",");
		msg = concat(msg, numToStr(grid.col));
		msg = concat(msg, ",");
		msg = concat(msg, numToStr(enemyId));
		msg = concat(msg, "?");
		msg = concat(msg, numToStr(getId()));
		processGeneral(str2Char(msg));
	}

	void moveToSpawn() {
		this->isMovingToSpwan = true;
		this->isMovingToTakeItem = false; //clear if wanted to take item

		string msg = "m:";
		msg = concat(msg, numToStr(getRowStart()));
		msg = concat(msg, ",");
		msg = concat(msg, numToStr(getColStart()));
		msg = concat(msg, "?");
		msg = concat(msg, numToStr(getId()));
		processGeneral(str2Char(msg));
	}

	void moveToItem() {
		this->isMovingToTakeItem = true;

		string msg = "m:";
		msg = concat(msg, numToStr(getRowItem()));
		msg = concat(msg, ",");
		msg = concat(msg, numToStr(getColItem()));
		msg = concat(msg, "?");
		msg = concat(msg, numToStr(getId()));
		processGeneral(str2Char(msg));
	}

	bool isMyHealthBelowCrtical() {
		return players[getId()].heroHealth <= HEALTH_CRITICAL;
	}

	bool isReachedSpawn() {
		return isSameLocation(this->locStart, players[getId()].location);
	}

	bool isReachedItem() {
		if (!this->isMovingToTakeItem) {
			return true;
		}

		Coordinate_grid tempLoc = players[getId()].location;
		tempLoc.col += ATTRIBUTE_WIDTH;
		return isSameLocation(this->locItem, tempLoc);
	}

	void attackEnemyPlayer() {
		int enemyId;
		Coordinate_grid grid;

		while (1) {
			while (gameDetails.isBotsPaused)
				;

			if (this->modeAi == AI_OFFENSIVE) {
				return;
			}

			//refill the health
			//Abhishek: there can be a while here to let it move to spawn at any cost
			if (isMyHealthBelowCrtical()) {
				moveToSpawn();
				break;
			}

			enemyId = players[getId()].idEnemy[0];
			Coordinate_grid grid = isNearEnemyPlayerAreaAI(getId(), enemyId);

			if (grid.row == -1) {
				enemyId = players[getId()].idEnemy[1];
				grid = isNearEnemyPlayerAreaAI(getId(), enemyId);
			}

			if (grid.row != -1) {
				helperAttackEnemy(grid, enemyId);
				usleep(LATENCY_HUMAN);
				continue;
			}

			break;
		}
	}

	void helperIsSpwanReached() {
		if (isReachedSpawn()) {
			this->isMovingToSpwan = false;
		}
	}

	void helperIsItemReached() {

		if (isReachedItem()) {
			this->isMovingToTakeItem = false;
		}
	}

	bool isItemTimerOff() {
		return !players[getId()].isTimerItemGlobalRunning;
	}

	bool isItemNearBy() {
		if (this->isMovingToTakeItem) { //No item is nearby if I am already going to take item
			return false;
		}

		int rowMe = players[getId()].location.row;
		int colMe = players[getId()].location.col + ATTRIBUTE_WIDTH;

		Coordinate_grid grid = Coordinate_grid(rowMe - BLOCKS_ITEM,
				colMe - BLOCKS_ITEM);

		for (int i = 0; i < 2 * BLOCKS_ITEM + 1; i++) {
			for (int j = 0; j < 2 * BLOCKS_ITEM + 1; j++) {
				Coordinate_grid gridToCheck = grid;
				gridToCheck.row += i;
				gridToCheck.col += j;

				if (!isValidCell(gridToCheck)) {
					continue;
				}

				Coordinate_grid gridTemp = gridToCheck;
				gridTemp.col -= ATTRIBUTE_WIDTH;

				if (isOponentCellForTeam(gridTemp, getId())) {
					continue;
				}
				if (isItem(gridToCheck)) {
					this->locItem = gridToCheck;
					return true;
				}
			}
		}
		return false;
	}

	bool isTempleRelativelyCritical() {
		int idEnemy = players[getId()].idEnemy[0];
		int enemyTempleHealth = players[idEnemy].team->templeHealth;
		int myTempleHealth = players[getId()].team->templeHealth;

		if (myTempleHealth <= (enemyTempleHealth - HEALTH_TEMPLE_RELATIVE_DIFF)) {
			return true;
		}

		return false;
	}

	Coordinate_grid isEnemyAttackingMyTemple(int enemyPlayer) {

		Coordinate_grid enemyLoc = players[enemyPlayer].location;

		for (int i = 0; i <= 8; i++) {
			if (isSameLocation(enemyLoc, this->myTempleAttackLocs[i])) {
				return Coordinate_grid(enemyLoc.row,
						enemyLoc.col + ATTRIBUTE_WIDTH);
			}
		}
		return Coordinate_grid(-1, -1);
	}

	void protectMyTemple() {

		while (1) {
			while (gameDetails.isBotsPaused)
				;

			if (this->modeAi == AI_OFFENSIVE) {
				return;
			}

			if (!isTempleRelativelyCritical()) {
				return;
			}

			//refill the health
			if (isMyHealthBelowCrtical()) {
				moveToSpawn();
				break;
			}

			int enemyId = players[getId()].idEnemy[0];
			Coordinate_grid grid = isEnemyAttackingMyTemple(enemyId);

			if (grid.row == -1) {
				enemyId = players[getId()].idEnemy[1];
				grid = isEnemyAttackingMyTemple(enemyId);
			}

			if (grid.row != -1) {
				helperAttackEnemy(grid, enemyId);
				usleep(LATENCY_HUMAN);
				continue;
			}

			break;
		}
	}

	void running() {
		attackEnemyTemple();
		while (!gameDetails.isGameOver) {
			while (gameDetails.isBotsPaused)
				;

			attackEnemyPlayer();

			helperIsSpwanReached();

			if (isItemNearBy() && isItemTimerOff() && !isMovingToSpwan) {//Abhi: apply spawn check
				moveToItem();
			}

			helperIsItemReached();

			if (!this->isMovingToSpwan && !this->isMovingToTakeItem) { //Abhishek: also call before attackEnemyPlayer
				usleep(LATENCY_HUMAN);
				attackEnemyTemple();
			}

			if (!this->isMovingToSpwan && this->modeAi != AI_OFFENSIVE) {
				protectMyTemple();
			}

			usleep(LATENCY_HUMAN);

		}
	}

};

pthread_t aiThreadId1;
pthread_t aiThreadId2;
pthread_t aiThreadId3;
pthread_t rectifyItemsThread;

AI* ai1;
AI* ai2;
AI* ai3;

void* startMe1(void*) {
	ai1->running();
	return NULL;
}

void* startMe2(void*) {
	ai2->running();
	return NULL;
}

void* startMe3(void*) {
	ai3->running();
	return NULL;
}

void* rectifyItems(void*) {
	while (1) {
		for (int id = 0; id < NUM_OF_PLAYERS; id++) {
			if (players[id].status != STATUS_PRESENT) {
				continue;
			}

			int itemsCount = 0;

			for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
				for (int c = START_INNER_GRID_COL; c <= END_INNER_GRID_COL; c++) {
					if (!isOponentCellForTeam(Coordinate_grid(r, c), id)) {
						if (isItem(Coordinate_grid(r, c + ATTRIBUTE_WIDTH))) {
							itemsCount++;
						}
					}
				}
			}

			if (itemsCount < ITEMS_ON_MAP_COUNT) {
				cout << "----rectifying item for: " << id << endl;
				placeItemAtRandomPos(players[id].team->name);
			}

		}
		sleep(1);
	}
	return NULL;
}

void createAIThread1() {
	createThread(&aiThreadId1, startMe1);
}

void createAIThread2() {
	createThread(&aiThreadId2, startMe2);
}

void createAIThread3() {
	createThread(&aiThreadId3, startMe3);
}

void createRectifyItemsThread() {
	createThread(&rectifyItemsThread, rectifyItems);
}

#endif

------------------------------------------------------------------------


#ifndef A_STAR_CLASS
#define A_STAR_CLASS

#include <stdio.h>
#include <iostream>
#include <queue>
#include "Node.h"
#include "CustomVectorStruct.h"

#define MOVEMENT_COST 1

using namespace std;

class AStarClass {

	Node* gridAStar[END_GRID_ROW + 1][END_INNER_GRID_COL + 1];
	Node* source = NULL;
	Node* target = NULL;
	priority_queue<Node*, vector<Node*> , NodeCompare> openList;

public:

	void setTarget(int r, int c) {
		target = gridAStar[r][c];
	}

	void setSource(int r, int c) {
		source = gridAStar[r][c];
		source->gval = 0;
		source->parent = NULL;
		source->fval = (source->gval + heurisitic(source, target));
		openList = priority_queue<Node*, vector<Node*> , NodeCompare> ();
		openList.push(source);
		source->onOpenList = true;
	}

	Node* getSource() {
		return source;
	}

	Node* getTarget() {
		return target;
	}

	void firstInitAStar() {
		//mapDetails filled in ActionOnGrid.h/aStarMove
		for (int i = 1; i <= END_GRID_ROW; i++) {
			for (int j = 1; j <= END_INNER_GRID_COL; j++) {
				gridAStar[i][j] = newNode(i, j);
			}
		}
	}

	//AStar init
	void initAStar(Coordinate_grid source, Coordinate_grid target) {
		for (int i = 1; i <= END_GRID_ROW; i++) {
			for (int j = 1; j <= END_INNER_GRID_COL; j++) {
				gridAStar[i][j]->fval = INT_MAX;
				gridAStar[i][j]->gval = INT_MAX;
				gridAStar[i][j]->parent = NULL;
				gridAStar[i][j]->alreadyVisited = false;
				gridAStar[i][j]->onPath = false;
				gridAStar[i][j]->onOpenList = false;
			}
		}
		setTarget(target.row, target.col - ATTRIBUTE_WIDTH);
		setSource(source.row, source.col);
	}

	void blockSiteAStarGrid(int i, int j) {
		gridAStar[i][j]->blockstatus = true;
	}

	void openSiteAStarGrid(int i, int j) {
		gridAStar[i][j]->blockstatus = false;//open
	}

	bool isBlocked(int i, int j) {
		return gridAStar[i][j]->blockstatus;
	}

	bool isOnPath(int i, int j) {
		return gridAStar[i][j]->onPath;
	}

	bool isSource(int i, int j) {
		if (source == NULL)
			return false;
		if (source->row == i && source->col == j)
			return true;
		return false;
	}

	bool isTarget(int i, int j) {
		if (target == NULL)
			return false;
		if (target->row == i && target->col == j)
			return true;
		return false;
	}

	//does not put the source onPath
	void constructPath(bool through) {
		Node* curr;
		if (through)
			curr = target;
		else
			curr = target->parent;
		while (!(equals(source, curr))) {
			curr->onPath = true;
			curr = curr->parent;
		}
	}

	void processIndividualNeighbour(Node* curr, Node* neighbour) {
		if (neighbour == NULL)
			return;
		if (neighbour->alreadyVisited || neighbour->blockstatus)
			return;
		int tentative_g_score = curr->gval + MOVEMENT_COST;
		// check is on gval and if the node is not in open list

		if (!neighbour->onOpenList || tentative_g_score < neighbour->gval) {
			neighbour->parent = curr;
			neighbour->gval = tentative_g_score;
			neighbour->fval = tentative_g_score + heurisitic(neighbour, target);
		}
		if (!neighbour->onOpenList) {
			openList.push(neighbour);
			neighbour->onOpenList = true;
		}
	}

	Node* getNodeFromGrid(int row, int col) {
		if (row < 1 || row > END_GRID_ROW)
			return NULL;
		if (col < 1 || col > END_INNER_GRID_COL)
			return NULL;
		return gridAStar[row][col];
	}

	// grid end points(out-of-bounds) need to be seriously taken care of
	void processNeighbours(Node* curr) {
		Node* neighbours[4];
		neighbours[0] = getNodeFromGrid(curr->row + 1, curr->col);
		neighbours[1] = getNodeFromGrid(curr->row - 1, curr->col);
		neighbours[2] = getNodeFromGrid(curr->row, curr->col + 1);
		neighbours[3] = getNodeFromGrid(curr->row, curr->col - 1);

		processIndividualNeighbour(curr, neighbours[0]);
		processIndividualNeighbour(curr, neighbours[1]);
		processIndividualNeighbour(curr, neighbours[2]);
		processIndividualNeighbour(curr, neighbours[3]);
	}

	//path always exists!
	void AStar(bool through) {
		while (!openList.empty()) {
			Node* curr = openList.top();
			openList.pop();

			curr->onOpenList = false;

			if (equals(curr, target)) {
				constructPath(through);
				// when we come to above point
				// the parent of target is set by previous iteration
				break;// vv imp
			}

			curr->alreadyVisited = true;// keeping in closed list
			processNeighbours(curr);
		}
	}
};

#endif


------------------------------------------------------------------------------

#ifndef CONSTANTS_H
#define CONSTANTS_H

//-------------------------CONSTANTS------------------

#define ITEMS_ON_MAP_COUNT 3
#define CELL_LENGTH 0.2f

#define NUM_OF_PLAYERS 4
#define PLAYER_ID_PRIMARY 0

#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))
#define SPEED_ACTUAL(id) (SPEED_MAX-players[id].speedMove+1)

//Grid related
#define MAX_XCELL 1.0f
#define MAX_YCELL 2.0f

#define MIN_XCELL -2.4f
#define MIN_YCELL -2.0f

#define START_GRID_ROW 1
#define END_GRID_ROW 20

#define START_INNER_GRID_COL 1
#define END_INNER_GRID_COL 20

#define START_OUTER_GRID_COL START_INNER_GRID_COL //1
#define END_OUTER_GRID_COL END_INNER_GRID_COL+(2*ATTRIBUTE_WIDTH)

#define START_LEFT_ATTRIBUTE_COL START_OUTER_GRID_COL //1
#define END_LEFT_ATTRIBUTE_COL START_LEFT_ATTRIBUTE_COL+ATTRIBUTE_WIDTH-1

#define END_RIGHT_ATTRIBUTE_COL END_OUTER_GRID_COL
#define START_RIGHT_ATTRIBUTE_COL END_RIGHT_ATTRIBUTE_COL-ATTRIBUTE_WIDTH+1

//Attributes
#define HEALTH_FULL_HERO 200
#define HEALTH_FULL_TEMPLE 500

#define STRENGTH_H_STUNNER 4 //DAMAGE
#define STRENGTH_H_DISABLER 6
#define STRENGTH_H_SLOWER 5
#define STRENGTH_H_BURSTER 7

#define CURSE_AMT_BURST_DAMAGE 30
#define CURSE_AMT_SLOW_STRENGTH 4

#define SPEED_MOVE_H_STUNNER 1
#define SPEED_MOVE_H_DISABLER 2
#define SPEED_MOVE_H_SLOWER 1
#define SPEED_MOVE_H_BURSTER 1

#define GAIN_ITEM_DAMAGE 2
#define GAIN_ITEM_HEALTH 20
#define GAIN_ITEM_SPEED_MOVE 1
#define GAIN_ITEM_TEMPLE_HEALER 50

#define SPEED_MAX 10
#define SPEED_MIN 0

//Number of Blocks
#define SPAWN_BLOCKS 3
#define ATTRIBUTE_WIDTH 2
#define TEMPLE_BLOCKS 2 //both on x & y direction
#define DIAG_BLOCKS 2 //numOfDiagonals = 2*DIAGONAL_BLOCKS+1
#endif

----------------------------------------------------------------
#ifndef CUSTOM_VECTOR_STRUCT_H
#define CUSTOM_VECTOR_STRUCT_H

struct Coordinate_openGl {
public:

	Coordinate_openGl(float X, float Y, float Z) {
		x = X;
		y = Y;
		z = Z;
	}

	Coordinate_openGl(float X, float Y) {
		x = X;
		y = Y;
	}

	float x, y, z;
};

struct Coordinate_grid {
public:

	Coordinate_grid() {

	}

	Coordinate_grid(int r, int c) {
		row = r;
		col = c;
	}

	int row, col;
};

struct Coordinate_window {
public:

	Coordinate_window(float X, float Y) {
		x = X;
		y = Y;
	}

	int x, y;
};

struct MyTexture {
public:

	MyTexture() {

	}

	MyTexture(GLuint texId, double u, double v) {
		textureId = texId;
		u3 = u;
		v3 = v;
	}

	GLuint textureId;
	double u3;
	double v3;
};

#endif

-----------------------------------------------

#ifndef FILE_PATHS_H
#define FILE_PATHS_H

//locations
#define PATH_LOC_STONES_COMMON "data/object_loc/stones_common_loc.txt"
#define PATH_LOC_STONES_TEAM "data/object_loc/stones_team_loc.txt"
#define PATH_LOC_TREES_COMMON "data/object_loc/trees_common_loc.txt"
#define PATH_LOC_TREES_TEAM "data/object_loc/trees_team_loc.txt"

//sounds
#define PATH_SOUND_EVENT "data/sounds/event.wav"
#define PATH_SOUND_MARIO_THEME "data/sounds/mario_theme.ogg"
#define PATH_SOUND_WRONG_CLICK "data/sounds/wrong_click.ogg"

//images
#define PATH_IMG_BTN_START "data/images/btn_start.bmp"

#define PATH_IMG_BG_WINDOW_MAIN "data/images/bg_window_main.bmp"
#define PATH_IMG_BG_SPAWN "data/images/bg_spawn.bmp"
#define PATH_IMG_BG_WAR "data/images/bg_war.bmp"
#define PATH_IMG_BG_ATTRIBUTE "data/images/bg_attribute.bmp"
#define PATH_IMG_BG_BLOCKED "data/images/bg_blocked.bmp"

#define PATH_IMG_TREE "data/images/tree.bmp"
#define PATH_IMG_STONE "data/images/stone.bmp"
#define PATH_IMG_TREE_WAR "data/images/tree_war.bmp"
#define PATH_IMG_STONE_WAR "data/images/stone_war.bmp"

#define PATH_IMG_TEMPLE_ANGELS "data/images/temple_angels.bmp"
#define PATH_IMG_TEMPLE_DEMONS "data/images/temple_demons.bmp"

#define PATH_IMG_HERO_STUNNER "data/images/hero_stunner.bmp"
#define PATH_IMG_HERO_DISABLER "data/images/hero_disabler.bmp"
#define PATH_IMG_HERO_SLOWER "data/images/hero_slower.bmp"
#define PATH_IMG_HERO_BURSTER "data/images/hero_burster.bmp"

#define PATH_IMG_ITEM_MOVEMENT_SPEED "data/images/item_movement_speed.bmp"
#define PATH_IMG_ITEM_DAMAGE "data/images/item_damage.bmp"
#define PATH_IMG_ITEM_HEALTH "data/images/item_health.bmp"
#define PATH_IMG_ITEM_TEMPLE_HEALER "data/images/item_temple_healer.bmp"

#define PATH_IMG_BG "data/images/bg.png"
#define PATH_IMG_LOGO "data/images/logo.png"
#define PATH_IMG_BACK "data/images/back.png"

#define PATH_IMG_ICON_ABOUT "data/images/icon_about.png"
#define PATH_IMG_ICON_CONTROLS "data/images/icon_controls.png"
#define PATH_IMG_ICON_PLAYERS "data/images/icon_players.png"
#define PATH_IMG_ICON_RULES "data/images/icon_rules.png"

#define PATH_IMG_PICK_TEAM "data/images/pick_team.png"
#define PATH_IMG_PICK_HERO "data/images/pick_hero.png"

#define PATH_IMG_JOIN_GAME "data/images/join_game.png"
#define PATH_IMG_HOST_GAME "data/images/host_game.png"

#define PATH_IMG_WHITE "data/images/white.png"
#define PATH_IMG_INPUT "data/images/input.png"
#define PATH_IMG_BTN_NEXT "data/images/btn_next.png"
#define PATH_IMG_TRY_AGAIN "data/images/try_again.png"
#define PATH_IMG_BTN_RESET "data/images/btn_reset.png"

#define PATH_IMG_BG_WAITING "data/images/bg_waiting.png"

#define PATH_IMG_PAGE_ABOUT "data/images/page_about.png"
#define PATH_IMG_PAGE_CONTROLS "data/images/page_controls.png"
#define PATH_IMG_PAGE_JOIN "data/images/page_join.png"
#define PATH_IMG_PAGE_PLAYERS "data/images/page_players.png"
#define PATH_IMG_PAGE_RULES "data/images/page_rules.png"
#define PATH_IMG_PAGE_WIN_ANGELS "data/images/page_win_angels.png"
#define PATH_IMG_PAGE_WIN_DEMONS "data/images/page_win_demons.png"

#define PATH_IMG_PLAYER_SINGLE "data/images/player_single.png"
#define PATH_IMG_PLAYER_MULTI "data/images/player_multi.png"

#endif

---------------------------------------------------

#ifndef FILE_PATHS_ATT_H
#define FILE_PATHS_ATT_H

#define PATH_IMG_ATT_CURSE_BURST "data/images/att_curse_burst.png"
#define PATH_IMG_ATT_CURSE_DISABLE "data/images/att_curse_disable.png"
#define PATH_IMG_ATT_CURSE_SLOW "data/images/att_curse_slow.png"
#define PATH_IMG_ATT_CURSE_STUN "data/images/att_curse_stun.png"
#define PATH_IMG_ATT_CURSE_NONE "data/images/att_curse_none.png"

#define PATH_IMG_ATT_HERO_BURSTER "data/images/att_hero_burster.png"
#define PATH_IMG_ATT_HERO_DISABLER "data/images/att_hero_disabler.png"
#define PATH_IMG_ATT_HERO_SLOWER "data/images/att_hero_slower.png"
#define PATH_IMG_ATT_HERO_STUNNER "data/images/att_hero_stunner.png"

#define PATH_IMG_ATT_MODE_BASIC "data/images/att_mode_basic.png"
#define PATH_IMG_ATT_MODE_MAGIC "data/images/att_mode_magic.png"
#define PATH_IMG_ATT_MODE_STUN "data/images/att_mode_stun.png"

#define PATH_IMG_ATT_TEAM_ANGELS "data/images/att_team_angels.png"
#define PATH_IMG_ATT_TEAM_DEMONS "data/images/att_team_demons.png"

#define PATH_IMG_ATT_TEAM_ENEMY "data/images/att_team_enemy.png"
#define PATH_IMG_ATT_TEAM_MY "data/images/att_team_my.png"

#define PATH_IMG_ATT_TIMERS_ITEM_OFF "data/images/att_timers_itemOff.png"
#define PATH_IMG_ATT_TIMERS_ITEM_ON "data/images/att_timers_itemOn.png"
#define PATH_IMG_ATT_TIMERS_MAGIC_OFF "data/images/att_timers_magicOff.png"
#define PATH_IMG_ATT_TIMERS_MAGIC_ON "data/images/att_timers_magicOn.png"

#define PATH_IMG_ATT_WALL "data/images/att_wall.png"

#define PATH_IMG_ATT_HEALTH "data/images/att_health.png"
#define PATH_IMG_ATT_TEMPLE_HEALTH "data/images/att_health_temple.png"
#define PATH_IMG_ATT_STRENGTH "data/images/att_strength.png"
#define PATH_IMG_ATT_SPEED "data/images/att_speed.png"

#endif

------------------------------------------------------
#ifndef GENERAL_HELPER_H
#define GENERAL_HELPER_H

#include "Globals.h"
#include "OpenGlHelper.h"
#include "FilePaths.h"
#include "FilePaths_att.h"
#include "CustomVectorStruct.h"
#include "Constants.h"
#include "AStarClass.h"
#include "Objects.h"
#include "MapNGrid.h"
#include "ActionOnGrid.h"
#include "Validations.h"
#include "Network.h"

bool isSameLocation(Coordinate_grid one, Coordinate_grid two) {
	if (one.row == two.row && one.col == two.col)
		return true;
	else
		return false;
}

void ifSpawnLocationFullHealth(int whichPlayer) {
	int k = 1;
	for (int i = END_GRID_ROW - SPAWN_BLOCKS + 1; i <= END_GRID_ROW; i++) {
		for (int j = 1; j <= k; j++) {
			bool oneSide = isSameLocation(Coordinate_grid(i, j),
					players[whichPlayer].location);
			bool otherSide = isSameLocation(Coordinate_grid(j, i),
					players[whichPlayer].location);
			if (oneSide || otherSide) {
				if (players[whichPlayer].heroHealth < HEALTH_FULL_HERO) {
					players[whichPlayer].heroHealth = HEALTH_FULL_HERO;
					return;
				}
			}
		}
		k++;
	}
}

//to be called by server
bool isNearEnemyTempleArea(int whichPlayer) {
	Coordinate_grid pLoc = players[whichPlayer].location;
	charCellType neighbours[4];
	neighbours[0] = getGridChar(pLoc.row + 1, pLoc.col, true, true);
	neighbours[1] = getGridChar(pLoc.row, pLoc.col + 1, true, true);
	neighbours[2] = getGridChar(pLoc.row - 1, pLoc.col, true, true);
	neighbours[3] = getGridChar(pLoc.row, pLoc.col - 1, true, true);

	charCellType enemyTemple, enemyTempleBack;

	if (players[whichPlayer].team->name == TEAM_ANGELS) {
		enemyTemple = TEMPLE_DEMONS;
		enemyTempleBack = T_DEMONS_BACK;
	} else {
		enemyTemple = TEMPLE_ANGELS;
		enemyTempleBack = T_ANGELS_BACK;
	}

	for (int i = 0; i <= 3; i++) {
		if (neighbours[i] == enemyTemple || neighbours[i] == enemyTempleBack) {
			return true;
		}
	}
	return false;

}

//to be called by server
bool isNearEnemyPlayerArea(int whichPlayer) {
	int enemyPlayer = players[whichPlayer].whichEnemyPlayerToAttack;
	Coordinate_grid myLoc = players[whichPlayer].location;
	Coordinate_grid enemyLoc = players[enemyPlayer].location;

	Coordinate_grid myNeighbours[4];
	myNeighbours[0] = Coordinate_grid(myLoc.row + 1, myLoc.col);
	myNeighbours[1] = Coordinate_grid(myLoc.row, myLoc.col + 1);
	myNeighbours[2] = Coordinate_grid(myLoc.row - 1, myLoc.col);
	myNeighbours[3] = Coordinate_grid(myLoc.row, myLoc.col - 1);

	for (int i = 0; i <= 3; i++) {
		if (isSameLocation(enemyLoc, myNeighbours[i])) {
			return true;
		}
	}
	return false;
}

//------------------------------------Change for single player (start)----------
Coordinate_grid isNearEnemyPlayerAreaAI(int botAI, int enemyPlayer) {

	Coordinate_grid myLoc = players[botAI].location;
	Coordinate_grid enemyLoc = players[enemyPlayer].location;

	Coordinate_grid myNeighbours[4];
	myNeighbours[0] = Coordinate_grid(myLoc.row + 1, myLoc.col);
	myNeighbours[1] = Coordinate_grid(myLoc.row, myLoc.col + 1);
	myNeighbours[2] = Coordinate_grid(myLoc.row - 1, myLoc.col);
	myNeighbours[3] = Coordinate_grid(myLoc.row, myLoc.col - 1);

	for (int i = 0; i <= 3; i++) {
		if (isSameLocation(enemyLoc, myNeighbours[i])) {
			return Coordinate_grid(enemyLoc.row, enemyLoc.col + ATTRIBUTE_WIDTH);
		}
	}
	return Coordinate_grid(-1, -1);
}
//------------------------------------Change for single player (end)----------

void blockOpponentsArea() {
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_INNER_GRID_COL; c <= END_INNER_GRID_COL; c++) {
			if (isOponentCellForTeam(Coordinate_grid(r, c), currPlayerId)) {
				putChar2Grid(r, c, BG_BLOCKED, true, false);

			}
		}
	}
}

void loadTeamAttributes() {
	angelsTeam.templeHealth = HEALTH_FULL_TEMPLE;
	demonsTeam.templeHealth = HEALTH_FULL_TEMPLE;
}

void loadPlayerGeneralAttributes(int playerId) {
	players[playerId].isTimerItemGlobalRunning = false;
	players[playerId].isTimerMagicSpellRunning = false;
	players[playerId].isTimerCurseRunning = false;
	players[playerId].atleastOnceAstar = false;
	players[playerId].toAttackTemple = false;
	players[playerId].whichEnemyPlayerToAttack = -1;

	players[playerId].astar = new AStarClass();
	players[playerId].astar->firstInitAStar();

	players[playerId].currPowerMode = POWER_MODE_BASIC;
	players[playerId].curseType = CURSE_NONE;

	players[playerId].heroHealth = HEALTH_FULL_HERO;

	switch (players[playerId].heroType) {
	case HERO_STUNNER:
		players[playerId].strength = STRENGTH_H_STUNNER;
		players[playerId].speedMove = SPEED_MOVE_H_STUNNER;
		players[playerId].charType = H_STUNNER;
		break;

	case HERO_DISABLER:
		players[playerId].strength = STRENGTH_H_DISABLER;
		players[playerId].speedMove = SPEED_MOVE_H_DISABLER;
		players[playerId].charType = H_DISABLER;
		break;

	case HERO_SLOWER:
		players[playerId].strength = STRENGTH_H_SLOWER;
		players[playerId].speedMove = SPEED_MOVE_H_SLOWER;
		players[playerId].charType = H_SLOWER;
		break;

	case HERO_BURSTER:
		players[playerId].strength = STRENGTH_H_BURSTER;
		players[playerId].speedMove = SPEED_MOVE_H_BURSTER;
		players[playerId].charType = H_BURSTER;
		break;
	}

	//setting player starting locations
	if (players[playerId].team->name == TEAM_ANGELS) {
		if (players[playerId].isFirstPlayerInTeam) {
			players[playerId].location = Coordinate_grid(19, 1);
		}

		else {
			players[playerId].location = Coordinate_grid(20, 2);
		}
	}

	if (players[playerId].team->name == TEAM_DEMONS) {
		if (players[playerId].isFirstPlayerInTeam) {
			players[playerId].location = Coordinate_grid(1, 19);
		}

		else {
			players[playerId].location = Coordinate_grid(2, 20);
		}
	}

	int row = players[playerId].location.row;
	int col = players[playerId].location.col;
	putChar2Grid(row, col, players[playerId].charType, true, false);

	//setting myFriend & enemyIds
	teamName myTeam = players[playerId].team->name;
	int k = 0; //used to keep track of enemy index

	players[playerId].idFriend = -1; //Setting default ids
	players[playerId].idEnemy[0] = -1;
	players[playerId].idEnemy[1] = -1;

	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		if (i == playerId || players[i].status == STATUS_NOT_JOINED) {
			continue;
		}

		if (isMyFriend(myTeam, i)) {
			players[playerId].idFriend = i;
		}

		else if (isNotMyFriend(myTeam, i)) {
			players[playerId].idEnemy[k++] = i;
		}
	}
}

void printGrid() {
	cout << "Printing grid" << endl;
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			handleGridCharSwitch(Coordinate_grid(r, c), PRINT_GRID);
		}
		cout << endl;
	}
}

//call this from the render function periodically
//on the master node, rest will be communicated to others
void moveHero(int whichPlayer) {

	if (!players[whichPlayer].atleastOnceAstar)
		return;

	//Collision Detection patch
	aStarMove(whichPlayer, players[whichPlayer].aStarThrough);

	Node* nodeToMove =
			findLocToMove(players[whichPlayer].location, whichPlayer);
	if (nodeToMove == NULL) {
		if (players[whichPlayer].toAttackTemple) {
			if (isNearEnemyTempleArea(whichPlayer))
				decreaseEnemyTempleHealth(whichPlayer);
			else {
				players[whichPlayer].toAttackTemple = false;
				//Abhishek temple attack remove the attack as stooped in between?
				//or continue??? //both seem fine
				//first option has a problem that, the player may not find any area to move
				//due to some player blocking, then!!!
				//but can't save all clicks that way someone can cheat -->
			}
		} else if (players[whichPlayer].whichEnemyPlayerToAttack != -1) {
			if (isNearEnemyPlayerArea(whichPlayer))
				decreaseEnemyPlayerHealth(whichPlayer);
			else {
				players[whichPlayer].whichEnemyPlayerToAttack = -1;
				//Abhishek enemy attack remove the attack as stopped in between?
				//or continue??? //both seem fine
				//first option has a problem that, the player may not find any area to move
				//due to some player blocking, then!!!
				//but can't save all clicks that way someone can cheat -->
			}
		}
		return; //nothing to move
	}
	Coordinate_grid celltoMove = Coordinate_grid(nodeToMove->row,
			nodeToMove->col + ATTRIBUTE_WIDTH);

	if (isItem(celltoMove)) {
		setItemCell(celltoMove, whichPlayer);
		takeItem(whichPlayer);
	}

	putChar2Grid(
			players[whichPlayer].location.row,
			players[whichPlayer].location.col,
			initialGridChar[players[whichPlayer].location.row][players[whichPlayer].location.col
					+ ATTRIBUTE_WIDTH], true, true);

	players[whichPlayer].location.row = nodeToMove->row;
	players[whichPlayer].location.col = nodeToMove->col;

	ifSpawnLocationFullHealth(whichPlayer);

	putChar2Grid(players[whichPlayer].location.row,
			players[whichPlayer].location.col, players[whichPlayer].charType,
			true, true);
}

void putTeamTypeAndTemples() {
	putPngToLAttCell(Coordinate_grid(1, 1), texId_att_team_my, 2, 1);
	putPngToRAttCell(Coordinate_grid(1, 1), texId_att_team_enemy, 2, 1);

	switch (players[currPlayerId].team->name) {
	case TEAM_ANGELS:
		putPngToLAttCell(Coordinate_grid(2, 1), texId_att_team_angels, 2, 1);
		putPngToRAttCell(Coordinate_grid(2, 1), texId_att_team_demons, 2, 1);

		putBmpToLAttCell(Coordinate_grid(4, 1), texId_t_angels, 2, 1); //temples
		putBmpToRAttCell(Coordinate_grid(4, 1), texId_t_demons, 2, 1);
		break;

	case TEAM_DEMONS:
		putPngToLAttCell(Coordinate_grid(2, 1), texId_att_team_demons, 2, 1);
		putPngToRAttCell(Coordinate_grid(2, 1), texId_att_team_angels, 2, 1);

		putBmpToLAttCell(Coordinate_grid(4, 1), texId_t_demons, 2, 1); //temples
		putBmpToRAttCell(Coordinate_grid(4, 1), texId_t_angels, 2, 1);
		break;

	case TEAM_BOTH:
		cout << "---shall never occur---" << endl;
		break;
	}
}

void putMyAttributes() {
	switch (players[currPlayerId].currPowerMode) {
	case POWER_MODE_BASIC:
		putPngToLAttCell(Coordinate_grid(9, 1), texId_att_mBasic, 2, 1);
		break;
	case POWER_MODE_MAGIC:
		putPngToLAttCell(Coordinate_grid(9, 1), texId_att_mMagic, 2, 1);
		break;
	case POWER_MODE_STUN:
		putPngToLAttCell(Coordinate_grid(9, 1), texId_att_mStun, 2, 1);
		break;
	}

	switch (players[currPlayerId].curseType) {
	case CURSE_NONE:
		putPngToLAttCell(Coordinate_grid(10, 1), texId_att_cNone, 2, 1);
		break;
	case CURSE_STUN:
		putPngToLAttCell(Coordinate_grid(10, 1), texId_att_cStun, 2, 1);
		break;
	case CURSE_DISABLE:
		putPngToLAttCell(Coordinate_grid(10, 1), texId_att_cDisable, 2, 1);
		break;
	case CURSE_WEAK:
		putPngToLAttCell(Coordinate_grid(10, 1), texId_att_cSlow, 2, 1);
		break;
	case CURSE_BURST:
		putPngToLAttCell(Coordinate_grid(10, 1), texId_att_cBurst, 2, 1);
		break;
	}

	//Speed
	putPngToLAttCell(Coordinate_grid(12, 1), texId_att_speed, 1, 1);
	putTextToLAttCell(Coordinate_grid(12, 2),
			numToStr(players[currPlayerId].speedMove));

	//Strength
	putPngToLAttCell(Coordinate_grid(13, 1), texId_att_strength, 1, 1);
	putTextToLAttCell(Coordinate_grid(13, 2),
			numToStr(players[currPlayerId].strength));
}

void putHealth() {
	putPngToLAttCell(Coordinate_grid(5, 1), texId_att_temple_health, 1, 1);
	putPngToRAttCell(Coordinate_grid(5, 1), texId_att_temple_health, 1, 1);

	switch (players[currPlayerId].team->name) {
	case TEAM_ANGELS:
		putTextToLAttCell(Coordinate_grid(5, 2),
				numToStr(angelsTeam.templeHealth));
		putTextToRAttCell(Coordinate_grid(5, 2),
				numToStr(demonsTeam.templeHealth));
		break;
	case TEAM_DEMONS:
		putTextToLAttCell(Coordinate_grid(5, 2),
				numToStr(demonsTeam.templeHealth));
		putTextToRAttCell(Coordinate_grid(5, 2),
				numToStr(angelsTeam.templeHealth));
		break;
	}

	int idFriend = players[currPlayerId].idFriend;
	int idEnemy0 = players[currPlayerId].idEnemy[0];
	int idEnemy1 = players[currPlayerId].idEnemy[1];

	putPngToLAttCell(Coordinate_grid(11, 1), texId_att_health, 1, 1);
	putTextToLAttCell(Coordinate_grid(11, 2),
			numToStr(players[currPlayerId].heroHealth));

	if (idFriend != -1) {
		putPngToLAttCell(Coordinate_grid(17, 1), texId_att_health, 1, 1);
		putTextToLAttCell(Coordinate_grid(17, 2),
				numToStr(players[idFriend].heroHealth));
	}

	if (idEnemy0 != -1) {
		putPngToRAttCell(Coordinate_grid(9, 1), texId_att_health, 1, 1);
		putTextToRAttCell(Coordinate_grid(9, 2),
				numToStr(players[idEnemy0].heroHealth));
	}

	if (idEnemy1 != -1) {
		putPngToRAttCell(Coordinate_grid(13, 1), texId_att_health, 1, 1);
		putTextToRAttCell(Coordinate_grid(13, 2),
				numToStr(players[idEnemy1].heroHealth));
	}
}

void putHeroes() {
	int idFriend = players[currPlayerId].idFriend;
	int idEnemy0 = players[currPlayerId].idEnemy[0];
	int idEnemy1 = players[currPlayerId].idEnemy[1];

	switch (players[currPlayerId].heroType) {
	case HERO_STUNNER:
		putBmpToLAttCell(Coordinate_grid(7, 1), texId_h_stunner, 1, 1);
		putPngToLAttCell(Coordinate_grid(8, 1), texId_att_h_stunner, 2, 1);
		break;

	case HERO_DISABLER:
		putBmpToLAttCell(Coordinate_grid(7, 1), texId_h_disabler, 1, 1);
		putPngToLAttCell(Coordinate_grid(8, 1), texId_att_h_disabler, 2, 1);
		break;

	case HERO_SLOWER:
		putBmpToLAttCell(Coordinate_grid(7, 1), texId_h_slower, 1, 1);
		putPngToLAttCell(Coordinate_grid(8, 1), texId_att_h_slower, 2, 1);
		break;

	case HERO_BURSTER:
		putBmpToLAttCell(Coordinate_grid(7, 1), texId_h_burster, 1, 1);
		putPngToLAttCell(Coordinate_grid(8, 1), texId_att_h_burster, 2, 1);
		break;
	}

	if (idFriend != -1) {
		switch (players[idFriend].heroType) {
		case HERO_STUNNER:
			putBmpToLAttCell(Coordinate_grid(15, 1), texId_h_stunner, 1, 1);
			putPngToLAttCell(Coordinate_grid(16, 1), texId_att_h_stunner, 2, 1);
			break;

		case HERO_DISABLER:
			putBmpToLAttCell(Coordinate_grid(15, 1), texId_h_disabler, 1, 1);
			putPngToLAttCell(Coordinate_grid(16, 1), texId_att_h_disabler, 2, 1);
			break;

		case HERO_SLOWER:
			putBmpToLAttCell(Coordinate_grid(15, 1), texId_h_slower, 1, 1);
			putPngToLAttCell(Coordinate_grid(16, 1), texId_att_h_slower, 2, 1);
			break;

		case HERO_BURSTER:
			putBmpToLAttCell(Coordinate_grid(15, 1), texId_h_burster, 1, 1);
			putPngToLAttCell(Coordinate_grid(16, 1), texId_att_h_burster, 2, 1);
			break;
		}
	}

	if (idEnemy0 != -1) {
		switch (players[idEnemy0].heroType) {
		case HERO_STUNNER:
			putBmpToRAttCell(Coordinate_grid(7, 1), texId_h_stunner, 1, 1);
			putPngToRAttCell(Coordinate_grid(8, 1), texId_att_h_stunner, 2, 1);
			break;

		case HERO_DISABLER:
			putBmpToRAttCell(Coordinate_grid(7, 1), texId_h_disabler, 1, 1);
			putPngToRAttCell(Coordinate_grid(8, 1), texId_att_h_disabler, 2, 1);
			break;

		case HERO_SLOWER:
			putBmpToRAttCell(Coordinate_grid(7, 1), texId_h_slower, 1, 1);
			putPngToRAttCell(Coordinate_grid(8, 1), texId_att_h_slower, 2, 1);
			break;

		case HERO_BURSTER:
			putBmpToRAttCell(Coordinate_grid(7, 1), texId_h_burster, 1, 1);
			putPngToRAttCell(Coordinate_grid(8, 1), texId_att_h_burster, 2, 1);
			break;
		}
	}

	if (idEnemy1 != -1) {
		switch (players[idEnemy1].heroType) {
		case HERO_STUNNER:
			putBmpToRAttCell(Coordinate_grid(11, 1), texId_h_stunner, 1, 1);
			putPngToRAttCell(Coordinate_grid(12, 1), texId_att_h_stunner, 2, 1);
			break;

		case HERO_DISABLER:
			putBmpToRAttCell(Coordinate_grid(11, 1), texId_h_disabler, 1, 1);
			putPngToRAttCell(Coordinate_grid(12, 1), texId_att_h_disabler, 2, 1);
			break;

		case HERO_SLOWER:
			putBmpToRAttCell(Coordinate_grid(11, 1), texId_h_slower, 1, 1);
			putPngToRAttCell(Coordinate_grid(12, 1), texId_att_h_slower, 2, 1);
			break;

		case HERO_BURSTER:
			putBmpToRAttCell(Coordinate_grid(11, 1), texId_h_burster, 1, 1);
			putPngToRAttCell(Coordinate_grid(12, 1), texId_att_h_burster, 2, 1);
			break;
		}
	}
}

void putTimers() {
	if (players[currPlayerId].isTimerItemGlobalRunning) {
		putPngToLAttCell(Coordinate_grid(19, 1), texId_att_time_itemOn, 2, 1);
	} else {
		putPngToLAttCell(Coordinate_grid(19, 1), texId_att_time_itemOff, 2, 1);
	}

	if (players[currPlayerId].isTimerMagicSpellRunning) {
		putPngToLAttCell(Coordinate_grid(20, 1), texId_att_time_magicOn, 2, 1);
	} else {
		putPngToLAttCell(Coordinate_grid(20, 1), texId_att_time_magicOff, 2, 1);
	}
}

void putWall() {
	putPngToRAttCell(Coordinate_grid(17, 1), texId_att_wall, 2, 1);
}

void loadAttributeSpace() {
	putTeamTypeAndTemples();

	putHeroes();
	putMyAttributes();

	putHealth();

	putTimers();
	//putWall();
}

void renderGrid() {
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			handleGridCharSwitch(Coordinate_grid(r, c), RENDER_GRID);
		}
	}
}

void requestBasicPower() {
	cout << "requesting basic_power by player " << currPlayerId << endl;
	if (players[currPlayerId].currPowerMode != POWER_MODE_BASIC) {
		helperSendPowerMode(0);
	}
}

//------------------------------------Change for single player (start)----------
void requestBasicPowerAI(int botId) {
	cout << "requesting basic_power by player " << botId << endl;
	if (players[botId].currPowerMode != POWER_MODE_BASIC) {
		helperSendPowerModeAI(0, botId);
	}
}

void requestMagicPowerAI(int botId) {
	if (players[botId].curseType != CURSE_DISABLE
			&& players[botId].currPowerMode != POWER_MODE_MAGIC
			&& !players[botId].isTimerMagicSpellRunning) { //filter at client
		cout << "requesting power_magic by player " << botId << endl;
		helperSendPowerModeAI(1, botId);
	} /*else {
	 cout << "Player can not use Magic power, you are cursed!!!" << endl;
	 }*/
}
//------------------------------------Change for single player (end)----------

void requestMagicPower() {
	if (players[currPlayerId].curseType != CURSE_DISABLE
			&& players[currPlayerId].currPowerMode != POWER_MODE_MAGIC
			&& !players[currPlayerId].isTimerMagicSpellRunning) { //filter at client
		cout << "requesting power_magic by player " << currPlayerId << endl;
		helperSendPowerMode(1);
	} /*else {
	 cout << "Player can not use Magic power, you are cursed!!!" << endl;
	 }*/
}

#endif

-------------------------------------------


#ifndef GLOBALS_H
#define GLOBALS_H

#include "Objects.h"
#include "Constants.h"
#include "CustomVectorStruct.h"

GameDetails gameDetails;

float windowWidth = 850, windowHeight = 700;

float cellWidth = windowWidth / (END_OUTER_GRID_COL - START_OUTER_GRID_COL + 1);
float cellHeight = windowHeight / (END_GRID_ROW - START_GRID_ROW + 1);

charCellType
		gridCharPrimary[END_GRID_ROW - START_GRID_ROW + 2][END_OUTER_GRID_COL
				- START_OUTER_GRID_COL + 2]; //+1 extra since starting from 1

charCellType gridChar[END_GRID_ROW - START_GRID_ROW + 2][END_OUTER_GRID_COL
		- START_OUTER_GRID_COL + 2]; //+1 extra since starting from 1

charCellType
		initialGridChar[END_GRID_ROW - START_GRID_ROW + 2][END_OUTER_GRID_COL
				- START_OUTER_GRID_COL + 2]; //+1 extra since starting from 1

int g_item_index_angels = 0; //holds the global index of next item to be placed
int g_item_index_demons = 0;

int currPlayerId;
TeamStruct angelsTeam, demonsTeam;

Player players[4];

//TextureIds
GLuint btn_start_texId;

GLuint texId_bg_grass;
GLuint texId_bg_spawn;
GLuint texId_bg_war;
GLuint texId_bg_blocked;

GLuint texId_bg_attribute;

GLuint texId_tree;
GLuint texId_stone;
GLuint texId_tree_war;
GLuint texId_stone_war;

GLuint texId_t_angels; //temples
GLuint texId_t_demons;

GLuint texId_h_stunner;
GLuint texId_h_disabler;
GLuint texId_h_slower;
GLuint texId_h_burster;

GLuint texId_i_speedMov;
GLuint texId_i_speedAttack;
GLuint texId_i_damage;
GLuint texId_i_health;
GLuint texId_i_tHealer;

GLuint temp_texId;

MyTexture texId_att_cBurst;
MyTexture texId_att_cDisable;
MyTexture texId_att_cSlow;
MyTexture texId_att_cStun;
MyTexture texId_att_cNone;

MyTexture texId_att_h_burster;
MyTexture texId_att_h_disabler;
MyTexture texId_att_h_slower;
MyTexture texId_att_h_stunner;

MyTexture texId_att_mBasic;
MyTexture texId_att_mMagic;
MyTexture texId_att_mStun;

MyTexture texId_att_team_angels;
MyTexture texId_att_team_demons;

MyTexture texId_att_team_enemy;
MyTexture texId_att_team_my;

MyTexture texId_att_timers;
MyTexture texId_att_time_itemOff;
MyTexture texId_att_time_itemOn;
MyTexture texId_att_time_magicOff;
MyTexture texId_att_time_magicOn;

MyTexture texId_att_health;
MyTexture texId_att_temple_health;
MyTexture texId_att_strength;
MyTexture texId_att_speed;

MyTexture texId_att_wall;

MyTexture texId_bg;
MyTexture texId_logo;
MyTexture texId_btn_back;

MyTexture texId_icon_about;
MyTexture texId_icon_controls;
MyTexture texId_icon_players;
MyTexture texId_icon_rules;

MyTexture texId_join_game;
MyTexture texId_host_game;
MyTexture texId_white;
MyTexture texId_input;
MyTexture texId_btn_next;
MyTexture texId_try_again;
MyTexture texId_btn_reset;

MyTexture texId_pick_team;
MyTexture texId_pick_hero;

MyTexture texId_bg_waiting;

MyTexture texId_page_about;
MyTexture texId_page_controls;
MyTexture texId_page_join;
MyTexture texId_page_players;
MyTexture texId_page_rules;
MyTexture texId_page_win_angels;
MyTexture texId_page_win_demons;

MyTexture texId_player_single;
MyTexture texId_player_multi;

//window IDs
int windowId_current;

Coordinate_grid downGrid_click; //used for clickHandler

//locks
pthread_mutex_t mutexQueuePrimary;
pthread_mutex_t mutexTargetCell;

bool isGameOver = false;
teamName winningTeam;

#endif

-----------------------------------------------------------

#ifndef HEADERS_H
#define HEADERS_H

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <GL/glut.h>
#include <GL/gl.h>

#include <fstream>
#include <sstream>
#include <string.h>

#include <list>
#include <math.h>

#include <pthread.h>

using namespace std;

#endif

----------------------------------------------------


#include <assert.h>
#include <fstream>

#include "ImageLoader.h"

using namespace std;

Image::Image(char* ps, int w, int h) :
	pixels(ps), width(w), height(h) {

}

Image::~Image() {
	delete[] pixels;
}

namespace {
//Converts a four-character array to an integer, using little-endian form
int toInt(const char* bytes) {
	return (int) (((unsigned char) bytes[3] << 24) | ((unsigned char) bytes[2]
			<< 16) | ((unsigned char) bytes[1] << 8) | (unsigned char) bytes[0]);
}

//Converts a two-character array to a short, using little-endian form
short toShort(const char* bytes) {
	return (short) (((unsigned char) bytes[1] << 8) | (unsigned char) bytes[0]);
}

//Reads the next four bytes as an integer, using little-endian form
int readInt(ifstream &input) {
	char buffer[4];
	input.read(buffer, 4);
	return toInt(buffer);
}

//Reads the next two bytes as a short, using little-endian form
short readShort(ifstream &input) {
	char buffer[2];
	input.read(buffer, 2);
	return toShort(buffer);
}

//Just like auto_ptr, but for arrays
template<class T>
class auto_array {
private:
	T* array;
	mutable bool isReleased;
public:
	explicit auto_array(T* array_ = NULL) :
		array(array_), isReleased(false) {
	}

	auto_array(const auto_array<T> &aarray) {
		array = aarray.array;
		isReleased = aarray.isReleased;
		aarray.isReleased = true;
	}

	~auto_array() {
		if (!isReleased && array != NULL) {
			delete[] array;
		}
	}

	T* get() const {
		return array;
	}

	T &operator*() const {
		return *array;
	}

	void operator=(const auto_array<T> &aarray) {
		if (!isReleased && array != NULL) {
			delete[] array;
		}
		array = aarray.array;
		isReleased = aarray.isReleased;
		aarray.isReleased = true;
	}

	T* operator->() const {
		return array;
	}

	T* release() {
		isReleased = true;
		return array;
	}

	void reset(T* array_ = NULL) {
		if (!isReleased && array != NULL) {
			delete[] array;
		}
		array = array_;
	}

	T* operator+(int i) {
		return array + i;
	}

	T &operator[](int i) {
		return array[i];
	}
};
}

Image* loadBMP(const char* filename) {
	ifstream input;
	input.open(filename, ifstream::binary);
	assert(!input.fail() || !"Could not find file");
	char buffer[2];
	input.read(buffer, 2);
	assert((buffer[0] == 'B' && buffer[1] == 'M') || !"Not a bitmap file");
	input.ignore(8);
	int dataOffset = readInt(input);

	//Read the header
	int headerSize = readInt(input);
	int width;
	int height;
	switch (headerSize) {
	case 40:
		//V3
		width = readInt(input);
		height = readInt(input);
		input.ignore(2);
		assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
		assert(readShort(input) == 0 || !"Image is compressed");
		break;
	case 12:
		//OS/2 V1
		width = readShort(input);
		height = readShort(input);
		input.ignore(2);
		assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
		break;
	case 64:
		//OS/2 V2
		assert(!"Can't load OS/2 V2 bitmaps");
		break;
	case 108:
		//Windows V4
		assert(!"Can't load Windows V4 bitmaps");
		break;
	case 124:
		//Windows V5
		assert(!"Can't load Windows V5 bitmaps");
		break;
	default:
		assert(!"Unknown bitmap format");
	}

	//Read the data
	int bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
	int size = bytesPerRow * height;
	auto_array<char> pixels(new char[size]);
	input.seekg(dataOffset, ios_base::beg);
	input.read(pixels.get(), size);

	//Get the data into the right format
	auto_array<char> pixels2(new char[width * height * 3]);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			for (int c = 0; c < 3; c++) {
				pixels2[3 * (width * y + x) + c] = pixels[bytesPerRow * y + 3
						* x + (2 - c)];
			}
		}
	}

	input.close();
	return new Image(pixels2.release(), width, height);
}


------------------------------------------------------------


#include "Headers.h"
#include "MapNGrid.h"
#include "GeneralHelper.h"
#include "OpenGlHelper.h"
#include "Constants.h"
#include "Objects.h"
#include "SoundAll.h"
#include "Timer.h"
#include "Network.h"

#include "Windows.h"

int main(int argc, char** argv) {
	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight); //Set the window size
	glutInitWindowPosition(10, 10);

	create_window_first();
	return 0; //This line is never reached
}

---------------------------------------------------------------


#ifndef MAP_N_GRID_H
#define MAP_N_GRID_H

#include "Constants.h"
#include "Objects.h"
#include "FilePaths.h"
#include "FilePaths_att.h"
#include "Globals.h"
#include "Validations.h"
#include "Node.h"

#include "text3d.h"

void putAttributeSpace();
void putGrass();
void putSpawnLocation();
void initMap();
GLfloat getXFromCell(int col);
GLfloat getYFromCell(int row);
void putChar2Grid(int row, int col, charCellType charType, bool isInner,
		bool isPrimary);
void putMultipleCharToGrid(int row, int col, charCellType charType,
		charCellType backChar, int xBlocks, int yBlocks, bool isInner);
void copyInit();
void setHeroLocation(int which, Coordinate_grid loc);

Node* findLocToMove(Coordinate_grid curr);
bool isBlockedSite(int r, int c);

void putAttributeSpace() {
	//left attribute space
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_LEFT_ATTRIBUTE_COL; c <= END_LEFT_ATTRIBUTE_COL; c++) {
			putChar2Grid(r, c, BG_ATTRIBUTE, false, false);
		}
	}
	//right attribute space
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_RIGHT_ATTRIBUTE_COL; c <= END_RIGHT_ATTRIBUTE_COL; c++) {
			putChar2Grid(r, c, BG_ATTRIBUTE, false, false);
		}
	}
}

void putGrass() {
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_INNER_GRID_COL; c <= END_INNER_GRID_COL; c++) {
			putChar2Grid(r, c, BG_GRASS, true, false);
		}
	}
}

void putSpawnLocation() {
	int k = 1;
	for (int i = END_GRID_ROW - SPAWN_BLOCKS + 1; i <= END_GRID_ROW; i++) {
		for (int j = 1; j <= k; j++) {
			putChar2Grid(i, j, BG_SPAWN, true, false);
			putChar2Grid(j, i, BG_SPAWN, true, false);
		}
		k++;
	}
}

void putWarGround() {
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_INNER_GRID_COL; c <= END_INNER_GRID_COL; c++) {
			if (r == c) {
				for (int k = DIAG_BLOCKS; k >= 0; k--) {
					putChar2Grid(r + k, c, BG_WAR, true, false);
					putChar2Grid(r, c + k, BG_WAR, true, false);
				}
			}
		}
	}
}

void putTemple() {
	putMultipleCharToGrid(5, 4, TEMPLE_ANGELS, T_ANGELS_BACK, TEMPLE_BLOCKS,
			TEMPLE_BLOCKS, true);
	putMultipleCharToGrid(17, 16, TEMPLE_DEMONS, T_DEMONS_BACK, TEMPLE_BLOCKS,
			TEMPLE_BLOCKS, true);
}

void putToGridFromFile(string filePath, charCellType charCellType,
		bool isTeamArea, bool isInner) {
	ifstream infile(filePath.c_str());
	string line;
	while (getline(infile, line)) {
		istringstream iss(line);
		int a, b;
		if (!(iss >> a >> b)) {
			break;
		} // error

		putChar2Grid(a, b, charCellType, isInner, false);
		if (isTeamArea) {
			putChar2Grid(b, a, charCellType, isInner, false);
		}
	}
}

void putStonesInTeamArea() {
	putToGridFromFile(PATH_LOC_STONES_TEAM, STONE, true, true);
}

void putTreesInTeamArea() {
	putToGridFromFile(PATH_LOC_TREES_TEAM, TREE, true, true);
}

void putStonesInCommonArea() {
	putToGridFromFile(PATH_LOC_STONES_COMMON, STONE_WAR, false, true);
}

void putTreesInCommonArea() {
	putToGridFromFile(PATH_LOC_TREES_COMMON, TREE_WAR, false, true);
}

void putStonesNTrees() {
	putStonesInTeamArea();
	putTreesInTeamArea();
	putStonesInCommonArea();
	putTreesInCommonArea();
}

void initMap() {
	//Maintain the sequence
	putGrass();
	putAttributeSpace();
	putSpawnLocation();
	putWarGround();
	putTemple();

	putStonesNTrees();

	copyInit(); //Disclaimer: to be called before item put & hero put!!!
}

void copyPrimaryGrid() {
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			gridCharPrimary[r][c] = gridChar[r][c];
		}
	}
}

//copies the state of the initial map to another array
//not modifiable now, as discussed
void copyInit() {
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			initialGridChar[r][c] = gridChar[r][c];
		}
	}
}

Node* findLocToMove(Coordinate_grid curr, int which) {
	AStarClass* astarForPlayer = players[which].astar;
	Node* neighbours[4];
	neighbours[0] = astarForPlayer->getNodeFromGrid(curr.row + 1, curr.col);
	neighbours[1] = astarForPlayer->getNodeFromGrid(curr.row - 1, curr.col);
	neighbours[2] = astarForPlayer->getNodeFromGrid(curr.row, curr.col + 1);
	neighbours[3] = astarForPlayer->getNodeFromGrid(curr.row, curr.col - 1);

	Node* toReturn = NULL;

	if (neighbours[0] != NULL && neighbours[0]->onPath) {
		toReturn = neighbours[0];
	} else if (neighbours[1] != NULL && neighbours[1]->onPath) {
		toReturn = neighbours[1];
	} else if (neighbours[2] != NULL && neighbours[2]->onPath) {
		toReturn = neighbours[2];
	} else if (neighbours[3] != NULL && neighbours[3]->onPath) {
		toReturn = neighbours[3];
	}

	if (toReturn != NULL) {
		(astarForPlayer->getNodeFromGrid(curr.row, curr.col))->onPath = false;//So that it doesn't loop back
	}

	return toReturn;
}

Coordinate_openGl getOpenGlCoordinatesFromGrid(Coordinate_grid grid) {
	GLfloat x = MIN_XCELL + (grid.col - 1) * CELL_LENGTH;
	GLfloat y = MAX_YCELL - (grid.row * CELL_LENGTH);
	return Coordinate_openGl(x, y);
}

Coordinate_grid getGridCoordinatesFromOpenGl(Coordinate_openGl openGl) {
	int row = (MAX_YCELL - openGl.y) / CELL_LENGTH + 1;
	int col = ((openGl.x - MIN_XCELL) / CELL_LENGTH) + 1;
	return Coordinate_grid(row, col);
}

charCellType getGridChar(int randomRow, int randomCol, bool isInner,
		bool isPrimary) {
	if (isInner) {
		randomCol += ATTRIBUTE_WIDTH;
	}

	if (isPrimary) {
		return gridCharPrimary[randomRow][randomCol];
	}

	else {
		return gridChar[randomRow][randomCol];
	}
}

charCellType getGridChar(Coordinate_grid grid) {
	return getGridChar(grid.row, grid.col, false, false);
}

float t3dComputeScale(const char* str) {
	float width = t3dDrawWidth(str);
	return CELL_LENGTH / width;
}

void putTextToCell(Coordinate_grid grid, string text, float r=1.0f, float g=0.0f, float b=0.0f) {
	Coordinate_openGl openGl = getOpenGlCoordinatesFromGrid(grid);
	float t3dScale = t3dComputeScale("qqqqq");
	glPushMatrix();
	glTranslatef(openGl.x + 0.04, openGl.y + 0.04, -5.0f);
	//glTranslatef(0, 0, -4.0f);
	glScalef(t3dScale, t3dScale, t3dScale);
	glColor3f(r, g, b);
	//glTranslatef(0, 0, 1.5f / _scale);
	t3dDraw3D(text, -1, 1, 0.2f);
	glPopMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
}

void putTextToLAttCell(Coordinate_grid grid, string text) {
	//grid.col should be between 1 & ATTRIBUTE_WIDTH
	putTextToCell(grid, text);
}

void putTextToRAttCell(Coordinate_grid grid, string text) {
	//grid.col should be between 1 & ATTRIBUTE_WIDTH
	grid.col += ATTRIBUTE_WIDTH + END_INNER_GRID_COL - START_INNER_GRID_COL + 1;
	putTextToCell(grid, text);
}

void putBmpToCell(Coordinate_grid grid, GLuint _textureId, int xBlocks,
		int yBlocks) {
	Coordinate_openGl openGl = getOpenGlCoordinatesFromGrid(grid);

	GLfloat xSize = xBlocks * CELL_LENGTH;
	GLfloat ySize = yBlocks * CELL_LENGTH;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(openGl.x, openGl.y, -5.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(openGl.x, openGl.y + ySize, -5.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(openGl.x + xSize, openGl.y + ySize, -5.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(openGl.x + xSize, openGl.y, -5.0f);

	glEnd();
}

void putBmpToLAttCell(Coordinate_grid grid, GLuint textureId, int xBlocks,
		int yBlocks) {
	//grid.col should be between 1 & ATTRIBUTE_WIDTH
	putBmpToCell(grid, textureId, xBlocks, yBlocks);
}

void putBmpToRAttCell(Coordinate_grid grid, GLuint textureId, int xBlocks,
		int yBlocks) {
	//grid.col should be between 1 & ATTRIBUTE_WIDTH
	grid.col += ATTRIBUTE_WIDTH + END_INNER_GRID_COL - START_INNER_GRID_COL + 1;
	putBmpToCell(grid, textureId, xBlocks, yBlocks);
}

void putPngToCell(Coordinate_grid grid, MyTexture myTexture, int xBlocks,
		int yBlocks) {
	Coordinate_openGl openGl = getOpenGlCoordinatesFromGrid(grid);

	GLfloat xSize = xBlocks * CELL_LENGTH;
	GLfloat ySize = yBlocks * CELL_LENGTH;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, myTexture.textureId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3f(openGl.x, openGl.y, -5.0f);
	glTexCoord2d(myTexture.u3, 0);
	glVertex3f(openGl.x, openGl.y + ySize, -5.0f);
	glTexCoord2d(myTexture.u3, myTexture.v3);
	glVertex3f(openGl.x + xSize, openGl.y + ySize, -5.0f);
	glTexCoord2d(0, myTexture.v3);
	glVertex3f(openGl.x + xSize, openGl.y, -5.0f);

	glEnd();
}

void putPngToLAttCell(Coordinate_grid grid, MyTexture myTexture, int xBlocks,
		int yBlocks) {
	//grid.col should be between 1 & ATTRIBUTE_WIDTH
	putPngToCell(grid, myTexture, xBlocks, yBlocks);
}

void putPngToRAttCell(Coordinate_grid grid, MyTexture myTexture, int xBlocks,
		int yBlocks) {
	//grid.col should be between 1 & ATTRIBUTE_WIDTH
	grid.col += ATTRIBUTE_WIDTH + END_INNER_GRID_COL - START_INNER_GRID_COL + 1;
	putPngToCell(grid, myTexture, xBlocks, yBlocks);
}

void putChars(int xBlocks, int yBlocks, int row, int col, charCellType charType) {
	for (int i = 0; i < xBlocks; i++) {
		for (int j = 0; j < yBlocks; j++) {
			gridChar[row - j][col + i] = charType;
		}
	}

}

void putPngWithChar(int row, int col, MyTexture texId, charCellType charType,
		int xBlocks, int yBlocks) { //this is not used for rendering

	putPngToCell(Coordinate_grid(row, col), texId, xBlocks, yBlocks);
	putChars(xBlocks, yBlocks, row, col, charType);
}

void putBmpWithChar(int row, int col, GLuint texId, charCellType charType,
		int xBlocks, int yBlocks) { //this is not used for rendering

	putBmpToCell(Coordinate_grid(row, col), texId, xBlocks, yBlocks);
	putChars(xBlocks, yBlocks, row, col, charType);
}

void putMultipleCharToGrid(int row, int col, charCellType charType,
		charCellType backChar, int xBlocks, int yBlocks, bool isInner) {

	//Adding 'back' characters - required in case of covering multiple cells
	for (int i = 0; i < xBlocks; i++) {
		for (int j = 0; j < yBlocks; j++) {
			putChar2Grid(row - j, col + i, backChar, isInner, false);
		}
	}

	//The first position(bottom-left) should be actual character
	putChar2Grid(row, col, charType, isInner, false);
}

//In case some new charis added to grid, remember to add the some in renderGrid
void putChar2Grid(int row, int col, charCellType charType, bool isInner,
		bool isPrimary) {
	if (!isValidRowNColIndex(Coordinate_grid(row, col), isInner)) {
		return;
	}

	if (isInner) {
		row = row;
		col = col + ATTRIBUTE_WIDTH;
	}

	if (isPrimary) {
		if (charType != BG_BLOCKED) {
			gridCharPrimary[row][col] = charType;
		}
	}

	else {
		gridChar[row][col] = charType;
	}

}

//gray area for second player area handled by Harinder's code
bool isBlockedSite(int r, int c, int whichPlayer) {
	if (isOponentCellForTeam(Coordinate_grid(r, c), whichPlayer))
		return true;
	charCellType type = getGridChar(r, c, true, true);
	switch (type) {
	case BG_GRASS:
	case BG_SPAWN:
	case BG_WAR:
	case I_SPEED_MOVE:
	case I_HEALTH:
	case I_DAMAGE:
	case I_TEMPLE_HEALER:
		return false;//open
	default:
		break;
	}
	return true; //close
}

#endif


------------------------------------------------------------


//****************Function Declarations*******************
void intToChar(int intToChng, char* charToRet);
char* str2Char(string str);
string numToStr(int num);
char* numToChar(int num);
string concat(string s1, string s2);
void tab(int count);
bool startsWith(const char *a, const char *b);
char* substring(const char *string, int position, int length);
int countOccurence(char* string, char splitter);
void split(char* string, char splitter, char splittedArr[][1024]);
void split(char* string, char splitter, char splittedArr[][16384]);
int indexOf(char* string, char of);

//****************Function Definitions*******************
void intToChar(int intToChng, char* charToRet) {
	size_t size = sizeof(charToRet);
	snprintf(charToRet, size, "%d", intToChng);
}

char* str2Char(string str) {
	char *cstr = new char[str.length() + 1];
	strcpy(cstr, str.c_str());
	return cstr;
}

string numToStr(int num) {
	stringstream ss;
	ss << num;
	return ss.str();
}

char* numToChar(int num) {
	string str = numToStr(num);
	return str2Char(str);
}

string concat(string s1, string s2) {
	stringstream ss;
	ss << s1 << s2;
	return ss.str();
}
void tab(int count) {
	for (int i = 0; i < count; i++) {
		cout << "\t";
	}
}

bool startsWith(const char *a, const char *b) {
	if (strncmp(a, b, strlen(b)) == 0) {
		return 0;
	}
	return 1;
}

char* substring(const char *string, int position, int length) {
	char *pointer;
	int c;

	pointer = (char*) malloc(length + 1);

	if (pointer == NULL) {
		printf("Inside substring : Unable to allocate memory.\n");
		exit( EXIT_FAILURE);
	}

	for (c = 0; c < position - 1; c++) {
		string++;
	}

	for (c = 0; c < length; c++) {
		*(pointer + c) = *string;
		string++;
	}

	*(pointer + c) = '\0';
	return pointer;
}

int countOccurence(char* string, char splitter) {
	int len = strlen(string);
	int count = 0;
	for (int i = 0; i < len; i++) {
		if (string[i] == splitter) {
			count++;
		}
	}
	return count;
}

void split(char* string, char splitter, char splittedArr[][1024]) {
	int len = strlen(string);
	char tmp[len];
	memset(tmp, 0, strlen(tmp));
	int j = 0; //pointer for tmp string
	int k = 0; //pointer for strings in splittedArr
	for (int i = 0; i < len; i++) {
		if (string[i] != splitter) {
			tmp[j] = string[i];
			j++;
		} else {
			tmp[j] = '\0';
			strcpy(splittedArr[k], tmp);
			j = 0;
			memset(tmp, 0, strlen(tmp));
			k++;
		}
	}
	if (string[len - 1] != splitter) { //Adding this check to get the last array
		tmp[j] = '\0';
		strcpy(splittedArr[k], tmp);
	}
}

void split(char* string, char splitter, char splittedArr[][16384]) {
	int len = strlen(string);
	char tmp[len];
	memset(tmp, 0, strlen(tmp));
	int j = 0; //pointer for tmp string
	int k = 0; //pointer for strings in splittedArr
	for (int i = 0; i < len; i++) {
		if (string[i] != splitter) {
			tmp[j] = string[i];
			j++;
		} else {
			tmp[j] = '\0';
			strcpy(splittedArr[k], tmp);
			j = 0;
			memset(tmp, 0, strlen(tmp));
			k++;
		}
	}

	if (string[len - 1] != splitter) { //Adding this check to get the last array
		tmp[j] = '\0';
		strcpy(splittedArr[k], tmp);
	}
}

int indexOf(char* string, char of) {
	int len = strlen(string);
	for (int i = 0; i < len; i++) {
		if (string[i] == of) {
			return i;
		}
	}
	return len - 1;
}

string remLastCharFromStr(string str) {
	char* strChar = str2Char(str);
	strChar[strlen(strChar) - 1] = '\0';

	return strChar;
}

---------------------------------------------------------


#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <map>
#include <math.h>
#include <list>
#include "MyString.h"
#include "OneMoreFile.h"
#include "Globals.h"

//----------Constants---------
#define M 400
#define QUEUE_LIMIT 5
#define RETRY_COUNT 5
#define RETRY_COUNT_BROADCAST 30
#define DATA_SIZE_KILO 1024

#define MSG_BROADCAST "b:"
#define MSG_MOVE "m:"

#define MSG_SERVER_ACK "a:"
#define MSG_SERVER_REJECT "r:"
#define SERVER_REQ_IGNORED "i:"
#define MSG_SERVER_WELCOME_BACK "w:"

#define MSG_POWER_BASIC "B:"
#define MSG_POWER_MAGIC "M:"

#define MSG_ATTACK_TEMPLE "t:"
#define MSG_ATTACK_HERO "h:"

#define MSG_CONNECT "c:"
#define MSG_VALIDATE_TEAM "y:"
#define MSG_VALIDATE_HERO "z:"
#define MSG_REQ_PLAYER_DETAILS "l:"

#define MSG_GAME_OVER "o:"
#define MSG_JOINING "g:"

#define SERVER_BUSY 'x'

//----------Globals---------
char DATA_RCVD[M][DATA_SIZE_KILO];
char GLOBAL_ARR[2][DATA_SIZE_KILO];

char server_send_data[DATA_SIZE_KILO], server_recv_data[DATA_SIZE_KILO];
char client_send_data[DATA_SIZE_KILO], client_recv_data[DATA_SIZE_KILO];
char broad_send_data[DATA_SIZE_KILO], broad_client_recv_data[DATA_SIZE_KILO];

unsigned int server_port = 0;
unsigned int remote_port = 0; // port with which to connect to server
unsigned int broadRemote_port = 0;

char ip2Join[IP_SIZE]; //used by client to join the server
char broadIp2Join[IP_SIZE];

char primaryNodeIp[IP_SIZE];
int primaryNodePort;

pthread_t serverThreadId;
pthread_t clientThreadId;
pthread_t broadcastThreadId;
pthread_t sendDataServerThreadId;
pthread_t updateServerThreadId;

int serverSock;

int isCreated = false;
int isJoined = false;

nodeHelper* selfNode = new nodeHelper;

list<string> queuePrimary;

enum broadcastType {
	BROADCAST_ALL, BROADCAST_GAME_OVER, BROADCAST_JOINING
};

//****************Function Declarations*******************
//-----Helper Functions----
bool isAllClientsAlive();
void takeUpdateAction(string msg);

void sendDataAndWaitForResult();
void sendDataDontWaitForResult();
void sendDataToServer();
void* threadSendDataToServer(void* arg);

void enqueMy(list<string> *l, string msg);
string dequeMy(list<string> *l);
void printQueue(list<string> *l);
void emptyQueue(list<string> *queue);

bool isServerOk();

connectStatus helperSendConnect();
bool helperValidateTeam(teamName team);
bool helperValidateHero(heroes hero);
void helperRequestPlayersDetails();
void helperSendServerMove(Coordinate_grid targetCell);

void createServerThread();
void createClientBroadcastThread();
void createSendServerDataThread();

void populateClientSendDataForGameOver();
void populateClientSendDataForJoining();
void populateClientSendDataForBroadcast();

nodeHelper* convertToNodeHelper(char *ipWithPort);

void setRemoteNode(char* ip, unsigned int port);
int createThread(pthread_t* threadId, void* threadFn(void*));
void joinIpWithPort(char* ip, unsigned int port, char* ipWithPort);
void getMyIp(char* ip);
int getMyPort(int mySock);
void fillNodeEntries(struct sockaddr_in server_addr);

int getIpOfPlayer(playerStatus status);
bool isTeamFull(teamName team);
bool isHeroTaken(heroes hero);

void processGameOver(char* data);
void processJoining(char* data);
void processBroadcast(char *data);
void processConnect(char *data);
void processValidateTeam(char *data, int id);
void processValidateHero(char *data, int id);
void processGeneral(char *completeData);

//-----TCP Functions-------
bool connectToServer(int & sock);
bool connectToServerBroadcast(int & sock, int playerId);
void* threadClientBroadcast(void* arg);
void* server(void* arg);

//****************Function Definitions*******************
//-----Helper Functions----
bool isAllClientsAlive() {
	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		if (players[i].status == STATUS_NOT_REACHABLE) {
			return false;
		}
	}
	return true;
}

void takeUpdateAction(const char* msg) {
	char* type = substring(msg, 0, 2);
	char* data = substring(msg, 3, strlen(msg));

	char dataValArr[2][DATA_SIZE_KILO];
	split(data, '?', dataValArr);

	char* reqData = dataValArr[0];
	int requestingPlayerId = atoi(dataValArr[1]);

	//since master maintains if player's reborn timer, handle at master
	//player reborn timer is not being sent to the client nodes.
	if (players[requestingPlayerId].isHeroRebornTimer) {
		cout << "At master node: " << "ignoring message as just reborn hero : "
				<< requestingPlayerId << endl;
		return;
	}

	if (strcmp(type, MSG_MOVE) == 0) {
		char coordinates[2][DATA_SIZE_KILO];
		split(reqData, ',', coordinates);
		players[requestingPlayerId].targetCell.row = atoi(coordinates[0]);
		players[requestingPlayerId].targetCell.col = atoi(coordinates[1]);

		players[requestingPlayerId].atleastOnceAstar = true;

		aStarMove(requestingPlayerId, true);
	}

	else if (strcmp(type, MSG_POWER_BASIC) == 0) {
		selectBasicPower(requestingPlayerId);
	}

	else if (strcmp(type, MSG_POWER_MAGIC) == 0) {
		selectMagicPower(requestingPlayerId);
	}

	else if (strcmp(type, MSG_ATTACK_TEMPLE) == 0) {
		char coordinates[2][DATA_SIZE_KILO];
		split(reqData, ',', coordinates);
		players[requestingPlayerId].targetCell.row = atoi(coordinates[0]);
		players[requestingPlayerId].targetCell.col = atoi(coordinates[1]);
		players[requestingPlayerId].atleastOnceAstar = true;
		attackEnemyTempleGeneric(requestingPlayerId);
	}

	else if (strcmp(type, MSG_ATTACK_HERO) == 0) {
		char coordinates[3][DATA_SIZE_KILO];
		split(reqData, ',', coordinates);
		players[requestingPlayerId].targetCell.row = atoi(coordinates[0]);
		players[requestingPlayerId].targetCell.col = atoi(coordinates[1]);
		int enemyPlayerId = atoi(coordinates[2]);
		players[requestingPlayerId].atleastOnceAstar = true;
		attackHeroEnemyGeneric(requestingPlayerId, enemyPlayerId);
	}

}

void sendDataAndWaitForResult() {
	memset(client_recv_data, 0, sizeof client_recv_data);
	client_recv_data[0] = '\0';
	createSendServerDataThread();
	while (client_recv_data[0] == '\0')
		; //wait until data is received
}

void sendDataDontWaitForResult() {
	memset(client_recv_data, 0, sizeof client_recv_data);
	client_recv_data[0] = '\0';
	createSendServerDataThread();
}

void sendDataToServer() {
	int sock, bytes_recieved;

	//Appending myId in the request
	strcat(client_send_data, "?");
	strcat(client_send_data, numToStr(currPlayerId).c_str());

	if (!connectToServer(sock)) {
		client_recv_data[0] = SERVER_BUSY;
		return;
	}

	//cout << "Client socket ID:" << sock << endl;
	send(sock, client_send_data, strlen(client_send_data), 0);

	bytes_recieved = recv(sock, client_recv_data, DATA_SIZE_KILO, 0);
	client_recv_data[bytes_recieved] = '\0';

	close(sock);
}

void* threadSendDataToServer(void* arg) {
	sendDataToServer();
	return NULL;
}

void* threadUpdateServer(void* arg) {
	while (1) {
		if (!queuePrimary.empty()) {
			pthread_mutex_lock(&mutexQueuePrimary);
			string msg = dequeMy(&queuePrimary);
			pthread_mutex_unlock(&mutexQueuePrimary);
			takeUpdateAction(msg.c_str());
		}
	}
	return NULL;
}

void connectServerBroadcast(int playerId) {
	memset(broad_client_recv_data, 0, sizeof broad_client_recv_data);
	int sock, bytes_recieved;
	broad_client_recv_data[0] = '\0';
	if (!connectToServerBroadcast(sock, playerId)) {
		broad_client_recv_data[0] = SERVER_BUSY;
		return;
	}
	//cout << "Client socket ID:" << sock << endl;
	send(sock, broad_send_data, strlen(broad_send_data), 0);
	bytes_recieved = recv(sock, broad_client_recv_data, DATA_SIZE_KILO, 0);
	broad_client_recv_data[bytes_recieved] = '\0';
	//cout << "Data successfully received" << client_recv_data << endl;
	close(sock);
}

void supportBroadCast(broadcastType type) {
	for (int i = NUM_OF_PLAYERS - 1; i >= 0; i--) { //loop is backwards, since we want to send info to server at last (Don't ask why)

		switch (type) { //Intentionally populating sendData again & again
		case BROADCAST_ALL:
			populateClientSendDataForBroadcast();
			break;
		case BROADCAST_GAME_OVER:
			populateClientSendDataForGameOver();
			break;
		case BROADCAST_JOINING:
			populateClientSendDataForJoining();
			break;
		}

		if (players[i].status == STATUS_PRESENT) {
			strcpy(broadIp2Join, players[i].networkDetails->ip);
			broadRemote_port = players[i].networkDetails->port;
			if (type == BROADCAST_JOINING) {
				cout << "i:" << broadIp2Join << ":" << broadRemote_port << endl;
			}
			connectServerBroadcast(i);
		}
	}
}

void* threadClientBroadcast(void* arg) {
	cout << "Broadcast Client Thread started" << endl;

	while (!isGameOver) {
		supportBroadCast(BROADCAST_ALL);
	}

	cout << "Master node: Game over!!!" << endl;
	cout << "Winning Team: " << winningTeam << endl;
	cout << "Stopping broadcast!!" << endl;
	supportBroadCast(BROADCAST_GAME_OVER);

	return NULL;
}

void enqueMy(list<string> *l, string msg) {
	(*l).push_back(msg);
}

string dequeMy(list<string> *l) {
	string msg = (*l).front();
	(*l).pop_front();
	return msg;
}

void printQueue(list<string> *l) {
	for (list<string>::iterator it = (*l).begin(); it != (*l).end(); it++) {
		cout << (*it) << ", ";
	}
	cout << endl;
}

void emptyQueue(list<string> *queue) {
	while (!(*queue).empty()) {
		cout << dequeMy(queue);
	}
}

connectStatus helperSendConnect() { //returns true
	cout << "trying to connect to host" << endl;

	strcpy(client_send_data, MSG_CONNECT);
	strcat(client_send_data, selfNode->ipWithPort);
	setRemoteNode(gameDetails.hostDetails->ip, gameDetails.hostDetails->port);

	//call either of 'sendDataDontWaitForResult' or 'sendDataAndWaitForResult'
	sendDataAndWaitForResult();

	char* type = substring(client_recv_data, 0, 2);
	char* data = substring(client_recv_data, 3, strlen(client_recv_data));

	if (type[0] == 'x' || strcmp(type, SERVER_REQ_IGNORED) == 0) {
		gameDetails.isIssueConnectingToServer = true;
		return CONNECTED_NOT;
	}

	if (strcmp(type, MSG_SERVER_ACK) == 0) {
		gameDetails.isIssueConnectingToServer = false;
		currPlayerId = atoi(data);
		return CONNECTED_NEW;
	}

	if (strcmp(type, MSG_SERVER_WELCOME_BACK) == 0) {
		gameDetails.isIssueConnectingToServer = false;
		currPlayerId = atoi(data);
		return CONNECTED_ALREADY;
	}

	return CONNECTED_NOT;
}

bool isServerOk() {
	char *type = substring(client_recv_data, 0, 2);
	if (strcmp(type, MSG_SERVER_ACK) == 0) {
		return true;
	}

	if (strcmp(type, MSG_SERVER_REJECT) == 0) {
		return false;
	}

	return false;
}

bool helperValidateTeam(teamName team) {
	strcpy(client_send_data, MSG_VALIDATE_TEAM);
	strcat(client_send_data, numToChar(team));
	setRemoteNode(gameDetails.hostDetails->ip, gameDetails.hostDetails->port);
	sendDataAndWaitForResult();
	return isServerOk();
}

bool helperValidateHero(heroes hero) {
	strcpy(client_send_data, MSG_VALIDATE_HERO);
	strcat(client_send_data, numToChar(hero));
	setRemoteNode(gameDetails.hostDetails->ip, gameDetails.hostDetails->port);
	sendDataAndWaitForResult();
	return isServerOk();
}

void helperRequestPlayersDetails() {
	strcpy(client_send_data, MSG_REQ_PLAYER_DETAILS);
	setRemoteNode(gameDetails.hostDetails->ip, gameDetails.hostDetails->port);
	sendDataAndWaitForResult();

	char* data = substring(client_recv_data, 3, strlen(client_recv_data));

	split(data, ',', DATA_RCVD);
	int k = 0, i = 0;
	for (i = 0; i < NUM_OF_PLAYERS; i++) {
		players[i].status = STATUS_PRESENT;
		players[i].heroType = static_cast<heroes> (atoi(DATA_RCVD[k++]));

		teamName team = static_cast<teamName> (atoi(DATA_RCVD[k++]));
		if (team == TEAM_ANGELS) {
			players[i].team = &angelsTeam;
		} else if (team == TEAM_DEMONS) {
			players[i].team = &demonsTeam;
		}

		players[i].isFirstPlayerInTeam = atoi(DATA_RCVD[k++]);

		if (DATA_RCVD[k][0] == 'x') {
			break; //breaking since all subsequent players must not be joined
		}
	}

	i++;
	while (i < NUM_OF_PLAYERS) {
		players[i++].status = STATUS_NOT_JOINED;
	}
}

void helperSendServerMove() {
	//Coordinate_grid targetCell = players[currPlayerId].targetCell;
	Coordinate_grid targetCell = onClickTargetCell;

	//Setting client_send_data
	strcpy(client_send_data, MSG_MOVE);

	char rowChar[4];
	intToChar(targetCell.row, rowChar);
	char colChar[4];
	intToChar(targetCell.col, colChar);

	strcat(client_send_data, rowChar);
	strcat(client_send_data, ",");
	strcat(client_send_data, colChar);

	//------------------------------------Change for single player (start)----------
	if (gameDetails.isSinglePlayerGame) {
		strcat(client_send_data, "?");
		strcat(client_send_data, numToStr(currPlayerId).c_str());

		processGeneral(client_send_data);
		return;
	}
	//------------------------------------Change for single player (end)----------

	//setting the remoteNode ip & port
	setRemoteNode(gameDetails.hostDetails->ip, gameDetails.hostDetails->port);

	//call either of 'sendDataDontWaitForResult' or 'sendDataAndWaitForResult'
	sendDataDontWaitForResult();
}

//------------------------------------Change for single player (start)----------
void helperSendPowerModeAI(int type, int botId) {

	if (type == 0)
		strcpy(client_send_data, MSG_POWER_BASIC);
	else if (type == 1)
		strcpy(client_send_data, MSG_POWER_MAGIC);
	else
		return;//something is wrong

	strcat(client_send_data, "?");
	strcat(client_send_data, numToStr(botId).c_str());

	processGeneral(client_send_data);
}
//------------------------------------Change for single player (end)----------

void helperSendPowerMode(int type) {

	if (type == 0)
		strcpy(client_send_data, MSG_POWER_BASIC);
	else if (type == 1)
		strcpy(client_send_data, MSG_POWER_MAGIC);
	else
		return;//something is wrong

	//setting the remoteNode ip & port
	setRemoteNode(gameDetails.hostDetails->ip, gameDetails.hostDetails->port);

	//call either of 'sendDataDontWaitForResult' or 'sendDataAndWaitForResult'
	sendDataDontWaitForResult();
}

void helperSendAttackTemple() {
	Coordinate_grid targetCell = onClickTargetCell;

	strcpy(client_send_data, MSG_ATTACK_TEMPLE);

	char rowChar[4];
	intToChar(targetCell.row, rowChar);
	char colChar[4];
	intToChar(targetCell.col, colChar);

	strcat(client_send_data, rowChar);
	strcat(client_send_data, ",");
	strcat(client_send_data, colChar);

	//------------------------------------Change for single player (start)----------
	if (gameDetails.isSinglePlayerGame) {
		strcat(client_send_data, "?");
		strcat(client_send_data, numToStr(currPlayerId).c_str());

		processGeneral(client_send_data);
		return;
	}
	//------------------------------------Change for single player (end)----------

	//setting the remoteNode ip & port
	setRemoteNode(gameDetails.hostDetails->ip, gameDetails.hostDetails->port);

	//call either of 'sendDataDontWaitForResult' or 'sendDataAndWaitForResult'
	sendDataDontWaitForResult();
}

void helperSendAttackHero(int enemyPlayer) {
	Coordinate_grid targetCell = onClickTargetCell;

	strcpy(client_send_data, MSG_ATTACK_HERO);

	char rowChar[4];
	intToChar(targetCell.row, rowChar);
	char colChar[4];
	intToChar(targetCell.col, colChar);
	char enemyIdChar[4];
	intToChar(enemyPlayer, enemyIdChar);

	strcat(client_send_data, rowChar);
	strcat(client_send_data, ",");
	strcat(client_send_data, colChar);
	strcat(client_send_data, ",");
	strcat(client_send_data, enemyIdChar);

	//------------------------------------Change for single player (start)----------
	if (gameDetails.isSinglePlayerGame) {
		strcat(client_send_data, "?");
		strcat(client_send_data, numToStr(currPlayerId).c_str());

		processGeneral(client_send_data);
		return;
	}
	//------------------------------------Change for single player (end)----------

	//setting the remoteNode ip & port
	setRemoteNode(gameDetails.hostDetails->ip, gameDetails.hostDetails->port);

	//call either of 'sendDataDontWaitForResult' or 'sendDataAndWaitForResult'
	sendDataDontWaitForResult();
}

//for non-primary nodes
void processGameOver(char* data) {
	cout << "At non-primary node: gameover received" << endl;
	cout << "At non-primary node: winningTeam " << data << endl;
	gameDetails.winningTeam = static_cast<teamName> (atoi(data));
	gameDetails.isGameOver = true;
}

void processJoining(char* data) {
	cout << "received joining msg" << endl;
	gameDetails.isStartJoiningTimer = true;
	strcpy(server_send_data, MSG_SERVER_ACK);
}

//used by non-primary nodes
void processBroadcast(char *data) {
	if (!gameDetails.isDoneWithJoining) {
		return; //I don't want to process broadcast if I am not joined
	}

	for (int i = 0; i < M; i++) {
		memset(DATA_RCVD[i], 0, sizeof DATA_RCVD[i]);
	}

	for (int i = 0; i < 2; i++) {
		memset(GLOBAL_ARR[i], 0, sizeof GLOBAL_ARR[i]);
	}

	split(data, '+', GLOBAL_ARR);

	//copying GridReceived
	split(GLOBAL_ARR[0], '|', DATA_RCVD);

	int k = 0;
	for (int i = START_GRID_ROW; i <= END_GRID_ROW; i++) {
		for (int j = START_INNER_GRID_COL; j <= END_INNER_GRID_COL; j++) {
			if (!isOponentCellForTeam(Coordinate_grid(i, j), currPlayerId)) {
				putChar2Grid(i, j,
						static_cast<charCellType> (atoi(DATA_RCVD[k++])), true,
						false);
			} else {
				k++;
			}
		}
	}

	for (int i = 0; i < M; i++) {
		memset(DATA_RCVD[i], 0, sizeof DATA_RCVD[i]);
	}

	if (currPlayerId != PLAYER_ID_PRIMARY) { //copying the players information only if I am not the primary Node
		//copying player attributes
		split(GLOBAL_ARR[1], ',', DATA_RCVD);
		k = 0;
		for (int i = 0; i < NUM_OF_PLAYERS; i++) {
			players[i].isFirstPlayerInTeam = atoi(DATA_RCVD[k++]);
			angelsTeam.templeHealth = atoi(DATA_RCVD[k++]);
			demonsTeam.templeHealth = atoi(DATA_RCVD[k++]);
			players[i].currPowerMode = static_cast<powerMode> (atoi(
					DATA_RCVD[k++]));
			players[i].heroHealth = atoi(DATA_RCVD[k++]);
			players[i].strength = atoi(DATA_RCVD[k++]);
			players[i].speedMove = atoi(DATA_RCVD[k++]);
			players[i].curseType = static_cast<curse> (atoi(DATA_RCVD[k++]));
			players[i].isTimerItemGlobalRunning = atoi(DATA_RCVD[k++]);
			players[i].isTimerMagicSpellRunning = atoi(DATA_RCVD[k++]);
			players[i].isTimerCurseRunning = atoi(DATA_RCVD[k++]);

			if (DATA_RCVD[k][0] == 'x') {
				break;
			}
		}
	}

	strcpy(server_send_data, MSG_SERVER_ACK);
}

int getIpOfPlayer(playerStatus status) {

	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		if (players[i].status == status) {
			return i;
		}
	}

	return -1;
}

void processConnect(char *data) {

	cout << "received connect message: " << data << endl;

	if (!gameDetails.isTimerNotHostWaiting) { //host waiting
		int notJoinedIp = getIpOfPlayer(STATUS_NOT_JOINED);

		if (notJoinedIp == -1) { //all players have joined
			strcpy(server_send_data, SERVER_REQ_IGNORED);
		}

		else { //some player has not joined, joining now
			players[notJoinedIp].status = STATUS_JOINED;
			players[notJoinedIp].networkDetails = convertToNodeHelper(data);
			strcpy(server_send_data, MSG_SERVER_ACK);
			strcat(server_send_data, numToChar(notJoinedIp));
		}

	}

	else { //host is not waiting (most probably while in the game)
		int leftIp = getIpOfPlayer(STATUS_NOT_REACHABLE);
		if (leftIp == -1) { //all players are in the game or dead
			strcpy(server_send_data, SERVER_REQ_IGNORED);
		}

		else { //some player has left
			players[leftIp].networkDetails = convertToNodeHelper(data);
			strcpy(server_send_data, MSG_SERVER_WELCOME_BACK);
			strcat(server_send_data, numToChar(leftIp));
		}
	}

}

int getMembersInTeam(teamName team) {
	int memberCount = 0;
	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		if (players[i].status == STATUS_PRESENT && team
				== players[i].team->name) {
			memberCount++;
		}
	}

	return memberCount;
}

void processValidateTeam(char *data, int id) {
	cout << "received validateTeam msg: '" << data << "' from: " << id << endl;

	teamName team = static_cast<teamName> (atoi(data));

	int memberCount = getMembersInTeam(team);

	if (memberCount == 2) {
		strcpy(server_send_data, MSG_SERVER_REJECT);
		return;
	}

	if (team == TEAM_ANGELS) {
		players[id].team = &angelsTeam;
	} else {
		players[id].team = &demonsTeam;
	}

	players[id].status = STATUS_PRESENT;

	if (memberCount == 0) {
		players[id].isFirstPlayerInTeam = true;
	}

	strcpy(server_send_data, MSG_SERVER_ACK);
}

bool isHeroTaken(heroes hero) {
	cout << players[0].heroType;
	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		if (players[i].status == STATUS_PRESENT && hero == players[i].heroType) {
			cout << "hero: " << hero << " taken by- " << i << endl;
			return true;
		}
	}

	return false;
}

void processValidateHero(char *data, int id) {
	cout << "received validateHero msg: " << data << "' from: " << id << endl;

	heroes hero = static_cast<heroes> (atoi(data));
	if (!isHeroTaken(hero)) {
		players[id].heroType = hero;
		strcpy(server_send_data, MSG_SERVER_ACK);
	}

	else {
		strcpy(server_send_data, MSG_SERVER_REJECT);
	}
}

void processReqPlayerDetails(char *data) {
	cout << "received reqPlayerDetails msg: " << data << endl;

	strcpy(server_send_data, MSG_SERVER_ACK);

	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		if (players[i].status == STATUS_PRESENT) {
			strcat(server_send_data, numToChar(players[i].heroType));
			strcat(server_send_data, ",");
			strcat(server_send_data, numToChar(players[i].team->name));
			strcat(server_send_data, ",");
			strcat(server_send_data, numToChar(players[i].isFirstPlayerInTeam));
			strcat(server_send_data, ",");
		} else {
			strcat(server_send_data, "x");
			break; //breaking since if a client is found not joined, all subsequent shall also not be joined
		}
	}
}

//used by master/primary node and no one else
void processGeneral(char *completeData) {
	pthread_mutex_lock(&mutexQueuePrimary);
	enqueMy(&queuePrimary, completeData);
	pthread_mutex_unlock(&mutexQueuePrimary);
}

void createServerThread() {
	createThread(&serverThreadId, server);
}

void createClientBroadcastThread() {
	createThread(&broadcastThreadId, threadClientBroadcast);
}

void createSendServerDataThread() {
	createThread(&sendDataServerThreadId, threadSendDataToServer);
}

void createUpdateServerThread() {
	cout << "createUpdateServerThread" << endl;
	createThread(&updateServerThreadId, threadUpdateServer);
}

void populateClientSendDataForGameOver() {
	cout << "At master node: called me!! game over!!" << endl;
	strcpy(broad_send_data, MSG_GAME_OVER);
	strcat(broad_send_data, numToStr(winningTeam).c_str());
}

void populateClientSendDataForJoining() {
	cout << "Broadcast Joining" << endl;
	strcpy(broad_send_data, MSG_JOINING);
}

void populateClientSendDataForBroadcast() {
	strcpy(broad_send_data, MSG_BROADCAST);

	//populating 'broad_client_send_data' with grid
	for (int i = START_GRID_ROW; i <= END_GRID_ROW; i++) {
		for (int j = START_INNER_GRID_COL; j <= END_INNER_GRID_COL; j++) {
			strcat(broad_send_data,
					numToStr(getGridChar(i, j, true, true)).c_str());
			strcat(broad_send_data, "|");
		}
	}

	strcat(broad_send_data, "+");

	//populating 'broad_client_send_data' with attributes
	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		if (players[i].status != STATUS_NOT_JOINED) {

			Player player = players[i];

			strcat(broad_send_data,
					numToStr(player.isFirstPlayerInTeam).c_str());
			strcat(broad_send_data, ",");

			strcat(broad_send_data, numToStr(angelsTeam.templeHealth).c_str());
			strcat(broad_send_data, ",");

			strcat(broad_send_data, numToStr(demonsTeam.templeHealth).c_str());
			strcat(broad_send_data, ",");

			strcat(broad_send_data, numToStr(player.currPowerMode).c_str());
			strcat(broad_send_data, ",");

			//attributes
			strcat(broad_send_data, numToStr(player.heroHealth).c_str());
			strcat(broad_send_data, ",");

			strcat(broad_send_data, numToStr(player.strength).c_str());
			strcat(broad_send_data, ",");

			strcat(broad_send_data, numToStr(player.speedMove).c_str());
			strcat(broad_send_data, ",");

			//related to curse
			strcat(broad_send_data, numToStr(player.curseType).c_str());
			strcat(broad_send_data, ",");

			//timers
			strcat(broad_send_data,
					numToStr(player.isTimerItemGlobalRunning).c_str());

			strcat(broad_send_data, ",");

			strcat(broad_send_data,
					numToStr(player.isTimerMagicSpellRunning).c_str());
			strcat(broad_send_data, ",");

			strcat(broad_send_data,
					numToStr(player.isTimerCurseRunning).c_str());
			strcat(broad_send_data, ",");
		}

		else {
			strcat(broad_send_data, "x");
			break;
		}
	}
}

nodeHelper* convertToNodeHelper(char *ipWithPort) {
	nodeHelper* toReturn = new nodeHelper;

	strcpy(toReturn->ipWithPort, ipWithPort);
	char* ipAddr = substring(ipWithPort, 0, indexOf(ipWithPort, ':'));
	char* portString = substring(ipWithPort, indexOf(ipWithPort, ':') + 2,
			strlen(ipWithPort));

	unsigned int portNum = atoi(portString);

	strcpy(toReturn->ip, ipAddr);
	toReturn->port = portNum;

	return toReturn;
}

void setRemoteNode(char* ip, unsigned int port) {
	memset(client_recv_data, 0, sizeof client_recv_data);
	strcpy(ip2Join, ip);
	remote_port = port;
}

int createThread(pthread_t* threadId, void* threadFn(void*)) {
	if (pthread_create(threadId, NULL, threadFn, NULL)) {

		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	return 0;
}

void joinIpWithPort(char* ip, unsigned int port, char* ipWithPort) {
	char portChar[10];
	intToChar(port, portChar);
	strcpy(ipWithPort, ip);
	strcat(ipWithPort, ":");
	strcat(ipWithPort, portChar);
}

void getMyIp(char* ip) {
	struct ifaddrs * ifAddrStruct = NULL;
	struct ifaddrs * ifa = NULL;
	void * tmpAddrPtr = NULL;

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr) {
			continue;
		}
		if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
			// is a valid IP4 Address
			tmpAddrPtr = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			//printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
			if (strcmp(ifa->ifa_name, "eth0") == 0 || strcmp(ifa->ifa_name,
					"wlan0") == 0 || strcmp(ifa->ifa_name, "p3p1") == 0) {
				strcpy(ip, addressBuffer);
			}
		} else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
			// is a valid IP6 Address
			tmpAddrPtr = &((struct sockaddr_in6 *) ifa->ifa_addr)->sin6_addr;
			char addressBuffer[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
			//printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
		}
	}

	if (ifAddrStruct != NULL) {
		freeifaddrs(ifAddrStruct);
	}
}

int getMyPort(int mySock) {
	struct sockaddr_in sin;
	socklen_t addrlen = sizeof(sin);
	if (getsockname(mySock, (struct sockaddr *) &sin, &addrlen) == 0
			&& sin.sin_family == AF_INET && addrlen == sizeof(sin)) {
		int local_port = ntohs(sin.sin_port);
		return local_port;
	} else {
		; // handle error
	}
	return 0;
}

void fillNodeEntries(struct sockaddr_in server_addr) {
	char ip[IP_SIZE];
	memset(ip, 0, sizeof ip);
	getMyIp(ip);

	if (strlen(ip) == 0) {
		strcpy(ip, "127.0.0.1");
	}

	strcpy(selfNode->ip, ip);

	selfNode->port = getMyPort(serverSock);

	char ipWithPort[IP_SIZE];
	joinIpWithPort(selfNode->ip, selfNode->port, ipWithPort);
	strcpy(selfNode->ipWithPort, ipWithPort);

}

//-----TCP Functions-------
bool connectToServer(int & sock) {
	struct hostent *host;
	struct sockaddr_in server_addr;

	host = gethostbyname(ip2Join);

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr = *((struct in_addr *) host->h_addr);
	server_addr.sin_port = htons(remote_port);
	bzero(&(server_addr.sin_zero), 8);

	int retriedCount = 0;
	while (connect(sock, (struct sockaddr *) &server_addr,
			sizeof(struct sockaddr)) == -1) {

		//trying again assuming the server is busy
		retriedCount++;
		cout << "Server busy --- retrying(" << retriedCount << "/"
				<< RETRY_COUNT << ")" << endl;
		sleep(1);
		if (retriedCount == RETRY_COUNT) {
			cout
					<< "Server is not up or not responding, terminating client...please try again"
					<< endl;
			close(sock);
			return false;
		}
	}

	return true;
}

void helperHelperBroadcast(int & sock, struct sockaddr_in & server_addr) {
	struct hostent *host;

	host = gethostbyname(broadIp2Join);
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr = *((struct in_addr *) host->h_addr);
	server_addr.sin_port = htons(broadRemote_port);
	bzero(&(server_addr.sin_zero), 8);
}

bool connectToServerBroadcast(int & sock, int playerId) {

	struct sockaddr_in server_addr;

	helperHelperBroadcast(sock, server_addr);

	int retriedCount = 0;
	while (connect(sock, (struct sockaddr *) &server_addr,
			sizeof(struct sockaddr)) == -1) {

		players[playerId].status = STATUS_NOT_REACHABLE;

		strcpy(broadIp2Join, players[playerId].networkDetails->ip);
		broadRemote_port = players[playerId].networkDetails->port;
		helperHelperBroadcast(sock, server_addr);

		//trying again assuming the server is busy
		retriedCount++;
		cout << "BroadCast-playerId: " << playerId << " with ip- "
				<< broadIp2Join << ":" << broadRemote_port
				<< " busy --- retrying(" << retriedCount << "/"
				<< RETRY_COUNT_BROADCAST << ")" << endl;
		sleep(1);
		if (retriedCount == RETRY_COUNT_BROADCAST) {

			//client is DEAD!!!
			cout << "playerId: " << playerId
					<< " is no more alive, let's continue" << endl;
			players[playerId].status = STATUS_LEFT;

			close(sock);
			return false;
		}
	}

	if (players[playerId].status == STATUS_NOT_REACHABLE) {
		players[playerId].status = STATUS_PRESENT;
		cout << "playerId: " << playerId << " back now" << endl;
		populateClientSendDataForJoining();
	}

	//cout << "Client successfully connected to server" << endl;
	return true;
}

void* server(void* arg) {
	int sock, connected, trueint = 1;

	struct sockaddr_in server_addr, client_addr;
	unsigned int sin_size;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}

	serverSock = sock;

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &trueint, sizeof(int)) == -1) {
		perror("Setsockopt");
		exit(1);
	}

	server_addr.sin_family = AF_INET;

	if (server_port != 0) { //Let the server choose the port itself if not supplied externally
		server_addr.sin_port = htons(server_port);
	}
	server_addr.sin_addr.s_addr = INADDR_ANY;

	bzero(&(server_addr.sin_zero), 8);

	if (bind(sock, (struct sockaddr *) ((&server_addr)),
			sizeof(struct sockaddr)) == -1) {
		perror("Unable to bind");
		exit(1);
	}
	if (listen(sock, QUEUE_LIMIT) == -1) {
		perror("Listen");
		exit(1);
	}

	fillNodeEntries(server_addr);

	cout << "Starting to listen on: " << selfNode->ipWithPort << endl;
	cout << ">>>: ";
	fflush(stdout);
	while (1) {
		int bytes_received;
		sin_size = sizeof(struct sockaddr_in);
		connected
				= accept(sock, (struct sockaddr*) ((&client_addr)), &sin_size);

		bytes_received = recv(connected, server_recv_data, DATA_SIZE_KILO, 0);
		server_recv_data[bytes_received] = '\0';

		char* type = substring(server_recv_data, 0, 2);
		char* data = substring(server_recv_data, 3, strlen(server_recv_data));

		char dataValArr[2][DATA_SIZE_KILO];
		split(data, '?', dataValArr);

		char* reqData = dataValArr[0];
		int requestingPlayerId = atoi(dataValArr[1]);

		if (strcmp(type, MSG_CONNECT) == 0) {
			processConnect(reqData);
		}

		else if (strcmp(type, MSG_REQ_PLAYER_DETAILS) == 0) {
			processReqPlayerDetails(reqData);
		}

		else if (strcmp(type, MSG_JOINING) == 0) {
			processJoining(reqData);
		}

		else if (!isAllClientsAlive()) {
			strcpy(server_send_data, SERVER_REQ_IGNORED);
		}

		else if (strcmp(type, MSG_BROADCAST) == 0) {
			processBroadcast(reqData);
		}

		else if (strcmp(type, MSG_VALIDATE_TEAM) == 0) {
			processValidateTeam(reqData, requestingPlayerId);
		}

		else if (strcmp(type, MSG_VALIDATE_HERO) == 0) {
			processValidateHero(reqData, requestingPlayerId);
		}

		//for non-primary nodes
		else if (strcmp(type, MSG_GAME_OVER) == 0) {
			processGameOver(reqData);
		}

		//for primary node
		else {
			processGeneral(server_recv_data);
		}

		send(connected, server_send_data, strlen(server_send_data), 0);
		fflush(stdout);//may be fatal, adding for UI

		close(connected);
	}
	cout << "right now, doesn't reach here" << endl;
	close(sock);
}

#endif


----------------------------------------------
#ifndef NODE_H
#define NODE_H

#define INT_MAX 32000

using namespace std;

struct Node {
	int row;
	int col;
	int fval;
	int gval;

	bool blockstatus;// true if blocked
	bool onPath; // for the travel path on screen, a hack
	bool alreadyVisited;// for the closed list
	bool onOpenList;//for the openList

	Node* parent;// for the recursive path

};

class NodeCompare {
public:
	bool operator()(Node* t1, Node* t2) {
		if (t2->fval <= t1->fval)
			return true;
		return false;
	}
};

//Node methods
Node* newNode(int row, int col);
int heurisitic(Node* a, Node* b);
int abs(int num);
bool equals(Node* a, Node*b);

//row and col need to start from 1 and end at 20 as discussed
Node* newNode(int row, int col) {
	//whenever creating a new node always fill its row and col
	Node* node = new Node;
	node->row = row;
	node->col = col;
	node->blockstatus = false;

	node->alreadyVisited = false;
	node->onPath = false;
	node->gval = INT_MAX;
	return node;
}

int heurisitic(Node* a, Node* b) {
	return abs(a->row - b->row) + abs(a->col - b->col);
}

int abs(int num) {
	if (num < 0)
		return -num;
	else
		return num;
}

bool equals(Node* a, Node*b) {
	return a->row == b->row && a->col == b->col;
}

#endif

------------------------------------------------------------------

#ifndef OBJECTS_H
#define OBJECTS_H

#include "Headers.h"
#include "AStarClass.h"

#define IP_SIZE 40

struct nodeHelper {
	char ip[IP_SIZE];
	unsigned int port;
	char ipWithPort[IP_SIZE];
};

//Angels - left, Demons - right
enum teamName {
	TEAM_ANGELS, TEAM_DEMONS, TEAM_BOTH
};

enum heroes {
	HERO_DISABLER, HERO_SLOWER, HERO_BURSTER, HERO_STUNNER, HERO_NOT_PRESENT
};

struct GameDetails {
	bool isSinglePlayerGame;
	bool isHost;
	nodeHelper* hostDetails;
	bool isIssueConnectingToServer;
	bool isTimerNotHostWaiting;

	bool isStartJoiningTimer;
	bool isDoneWithJoining;

	bool isGameOver;
	teamName winningTeam;

	bool isBotsPaused;
};

enum charCellType {
	BG_GRASS, BG_SPAWN, BG_WAR, BG_ATTRIBUTE, BG_BLOCKED,

	STONE, TREE, STONE_WAR, TREE_WAR,

	TEMPLE_ANGELS, TEMPLE_DEMONS,

	H_DISABLER, H_SLOWER, H_BURSTER, H_STUNNER,

	I_SPEED_MOVE, I_HEALTH, I_DAMAGE, I_TEMPLE_HEALER,

	TREE_BACK, T_ANGELS_BACK, T_DEMONS_BACK,

	CLICK_PLAYER_SINGLE, CLICK_PLAYER_MULTI,

	CLICK_ABOUT, CLICK_CONTROLS, CLICK_PLAYERS, CLICK_RULES, CLICK_BACK,

	CLICK_ANGELS, CLICK_DEMONS, CLICK_PICK_TEAM,

	CLICK_HOST_GAME, CLICK_JOIN_GAME, CLICK_NEXT, CLICK_RESET,

	INPUT
};

charCellType itemCharCell[] = { I_SPEED_MOVE, I_HEALTH, I_DAMAGE,
		I_TEMPLE_HEALER };

enum curse {
	CURSE_DISABLE, CURSE_WEAK, CURSE_BURST, CURSE_STUN, CURSE_NONE
};

enum itemType {
	ITEM_SPEED_MOVE, ITEM_HEALTH, ITEM_DAMAGE, ITEM_TEMPLE_HEALER
};

enum powerMode {
	POWER_MODE_BASIC, POWER_MODE_MAGIC, POWER_MODE_STUN
};

struct TeamStruct {
	teamName name;
	int templeHealth;
};

enum playerStatus {
	STATUS_PRESENT, STATUS_LEFT,

	STATUS_NOT_REACHABLE,

	STATUS_NOT_JOINED, STATUS_JOINED
};

enum connectStatus {
	CONNECTED_NEW, CONNECTED_ALREADY, CONNECTED_NOT
};

struct Player {
	//int playerId;
	playerStatus status;

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

--------------------------------------------------------------


#ifndef ONE_MORE_FILE_H
#define ONE_MORE_FILE_H

bool isMyFriend(teamName team, int playerId) {
	return team == players[playerId].team->name;
}

bool isNotMyFriend(teamName team, int playerId) {
	return team != players[playerId].team->name;
}

//pass true to second argument if taken rebirth
//called by server only
void giveBirthToPlayer(int playerId, bool reborn) {
	if (players[playerId].status != STATUS_LEFT) {
		players[playerId].status = STATUS_PRESENT;
	}

	players[playerId].isTimerItemGlobalRunning = false;
	players[playerId].isTimerMagicSpellRunning = false;
	players[playerId].isTimerCurseRunning = false;

	players[playerId].isHeroRebornTimer = false;
	//timer reborn logic handled by Timer.h


	players[playerId].atleastOnceAstar = false;
	players[playerId].toAttackTemple = false;
	players[playerId].whichEnemyPlayerToAttack = -1;

	if (!reborn)//first time
		players[playerId].astar = new AStarClass();
	players[playerId].astar->firstInitAStar();

	players[playerId].currPowerMode = POWER_MODE_BASIC;
	players[playerId].curseType = CURSE_NONE;
	players[playerId].heroHealth = HEALTH_FULL_HERO;

	//if reborn heroType stays same, and when newly born comes from networking
	switch (players[playerId].heroType) {
	case HERO_STUNNER:
		players[playerId].strength = STRENGTH_H_STUNNER;
		players[playerId].speedMove = SPEED_MOVE_H_STUNNER;
		players[playerId].charType = H_STUNNER;
		break;

	case HERO_DISABLER:
		players[playerId].strength = STRENGTH_H_DISABLER;
		players[playerId].speedMove = SPEED_MOVE_H_DISABLER;
		players[playerId].charType = H_DISABLER;
		break;

	case HERO_SLOWER:
		players[playerId].strength = STRENGTH_H_SLOWER;
		players[playerId].speedMove = SPEED_MOVE_H_SLOWER;
		players[playerId].charType = H_SLOWER;
		break;

	case HERO_BURSTER:
		players[playerId].strength = STRENGTH_H_BURSTER;
		players[playerId].speedMove = SPEED_MOVE_H_BURSTER;
		players[playerId].charType = H_BURSTER;
		break;
	}

	if (reborn) {
		int row = players[playerId].location.row;
		int col = players[playerId].location.col;
		putChar2Grid(row, col, initialGridChar[row][col + ATTRIBUTE_WIDTH],
				true, true);//putting grass or war area
	}

	//setting player starting locations
	if (players[playerId].team->name == TEAM_ANGELS) {
		if (players[playerId].isFirstPlayerInTeam) {
			players[playerId].location = Coordinate_grid(19, 1);
			//players[playerId].targetCell=Coordinate_grid(19, 1);
		}

		else {
			players[playerId].location = Coordinate_grid(20, 2);
			//players[playerId].targetCell=Coordinate_grid(20, 2);
		}
	}

	if (players[playerId].team->name == TEAM_DEMONS) {
		if (players[playerId].isFirstPlayerInTeam) {
			players[playerId].location = Coordinate_grid(1, 19);
			//players[playerId].targetCell=Coordinate_grid(1, 19);
		}

		else {
			players[playerId].location = Coordinate_grid(2, 20);
			//players[playerId].targetCell=Coordinate_grid(2, 20);
		}
	}

	int row = players[playerId].location.row;
	int col = players[playerId].location.col;
	putChar2Grid(row, col, players[playerId].charType, true, reborn);//it works with only false :(


	//if reborn, no need to set the enemy and friend ids again
	if (!reborn) {
		//setting myFriend & enemyIds
		teamName myTeam = players[currPlayerId].team->name;
		int k = 0; //used to keep track of enemy index

		for (int i = 0; i < NUM_OF_PLAYERS; i++) {
			if (i == currPlayerId) {
				continue;
			}

			if (isMyFriend(myTeam, i)) {
				players[currPlayerId].idFriend = i;
			}

			else if (isNotMyFriend(myTeam, i)) {
				players[currPlayerId].idEnemy[k++] = i;
			}
		}
	}

}

void aStarMove(int whichPlayer, bool through) {
	Coordinate_grid targetCell = players[whichPlayer].targetCell;

	//have to handle block status here itelf! phew :(
	for (int i = START_GRID_ROW; i <= END_GRID_ROW; i++) {
		for (int j = START_INNER_GRID_COL; j <= END_INNER_GRID_COL; j++) {
			if (isBlockedSite(i, j, whichPlayer)) {
				players[whichPlayer].astar->blockSiteAStarGrid(i, j);
			} else {
				players[whichPlayer].astar->openSiteAStarGrid(i, j);
			}
		}
	}
	//for the not through, the target could be blocked actually, change it locally!

	if (!through) {
		players[whichPlayer].astar->openSiteAStarGrid(targetCell.row,
				targetCell.col - ATTRIBUTE_WIDTH);
	}
	players[whichPlayer].astar->initAStar(players[whichPlayer].location,
			targetCell);

	players[whichPlayer].aStarThrough = through;
	players[whichPlayer].atleastOnceAstar = true;

	players[whichPlayer].astar->AStar(through);
}

//used by primary node
void selectBasicPower(int whichPlayer) {
	cout << "selected power_basic by player" << whichPlayer << endl;
	players[whichPlayer].currPowerMode = POWER_MODE_BASIC;
}

//used by primary node
void selectMagicPower(int whichPlayer) {
	if (players[whichPlayer].curseType != CURSE_DISABLE) {
		cout << "selected power_magic by player " << whichPlayer << endl;
		players[whichPlayer].currPowerMode = POWER_MODE_MAGIC;
	} else {
		cout << "Player can not use Magic power, you are cursed!!!" << endl;
	}
}

//----------TEMPLE ATTACK METHODS-------------------

void setAttackTemple(int whichPlayer, bool value) {
	players[whichPlayer].toAttackTemple = value;
}

void setAttackEnemyPlayer(int whichPlayer, int value) {
	players[whichPlayer].whichEnemyPlayerToAttack = value;
}

//to be called by server
void attackEnemyTempleGeneric(int whichPlayer) {
	setAttackTemple(whichPlayer, false);
	//above is imp, if path changed in between
	//then the previous value would be true
	//and if arrives by way of moving, and not attacking near temple
	//then will attack even when attack was cancelled
	aStarMove(whichPlayer, false);
	setAttackTemple(whichPlayer, true);
}

//to be called by server
//use which enemy you want to attack
//enemyPlayer to be >=0 <=3, obviously
void attackHeroEnemyGeneric(int whichPlayer, int enemyPlayer) {
	setAttackEnemyPlayer(whichPlayer, -1);
	//above is imp, if path changed in between
	//then the previous value would be true
	//and if arrives by way of moving, and not attacking near temple
	//then will attack even when attack was cancelled
	aStarMove(whichPlayer, false);
	setAttackEnemyPlayer(whichPlayer, enemyPlayer);
}

#endif

-----------------------------------------------------------

#ifndef OPEN_GL_HELPER_H
#define OPEN_GL_HELPER_H

#include "ImageLoader.h"
#include "Constants.h"
#include "CustomVectorStruct.h"
#include "SoundAll.h"
#include "ActionOnGrid.h"
#include "Objects.h"
#include "Headers.h"
#include "lodepng.h"

//Makes the image into a texture, and returns the id of the texture
GLuint loadBmpTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D, //Always GL_TEXTURE_2D
			0, //0 for now
			GL_RGB, //Format OpenGL uses for image
			image->width, image->height, //Width and height
			0, //The border of the image
			GL_RGB, //GL_RGB, because pixels are stored in RGB format
			GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
			//as unsigned numbers
			image->pixels); //The actual pixel data
	return textureId; //Returns the id of the texture
}

//converts window coordinates to openGL coordinates
//function not used now, but imp function : let it be here
Coordinate_openGl getOGLPos(float mX, float mY) {
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (float) mX;
	winY = (float) viewport[3] - (float) mY;
	glReadPixels(mX, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX,
			&posY, &posZ);

	return Coordinate_openGl(posX, posY, posZ);
}

//Called when the window is resized
void handleResize(int weight, int height) {
	windowWidth = weight;
	windowHeight = height;
	//Tell OpenGL how to convert from coordinates to pixel values
	glViewport(0, 0, windowWidth, windowHeight);
	glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective
	glPushMatrix();
	//Set the camera perspective
	glLoadIdentity(); //Reset the camera
	gluPerspective(45.0, //The camera angle
			(double) weight / (double) height, //The width-to-height ratio
			1.0, //The near z clipping coordinate
			200.0); //The far z clipping coordinate
}

GLuint getTextureFromBmp(string path) {
	GLuint _textureId; //The id of the texture
	Image *bgImage = loadBMP(path.c_str());
	_textureId = loadBmpTexture(bgImage);
	delete bgImage;
	return _textureId;
}

MyTexture getTextureFromPng(string path) {

	MyTexture myTexture;
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, path);

	// If there's an error, display it.
	if (error != 0) {
		std::cout << "error " << error << ": " << lodepng_error_text(error)
				<< std::endl;
		return myTexture;
	}

	// Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
	size_t u2 = 1;
	while (u2 < width)
		u2 *= 2;
	size_t v2 = 1;
	while (v2 < height)
		v2 *= 2;
	// Ratio for power of two version compared to actual version, to render the non power of two image with proper size.
	double u3 = (double) width / u2;
	double v3 = (double) height / v2;

	// Make power of two version of the image.
	std::vector<unsigned char> image2(u2 * v2 * 4);
	for (size_t y = 0; y < height; y++)
		for (size_t x = 0; x < width; x++)
			for (size_t c = 0; c < 4; c++) {
				image2[4 * u2 * y + 4 * x + c] = image[4 * width * y + 4 * x
						+ c];
			}

	// Enable the texture for OpenGL.


	glGenTextures(1, &myTexture.textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, myTexture.textureId); //Tell OpenGL which texture to edit
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST = no smoothing
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			&image2[0]);

	myTexture.u3 = u3;
	myTexture.v3 = v3;

	return myTexture;
}

/*void setOrthographicProjection() {
 glMatrixMode(GL_PROJECTION);
 glPushMatrix();
 glLoadIdentity();
 gluOrtho2D(0, windowWidth, 0, windowHeight);
 glScalef(1, -1, 1);
 glTranslatef(0, -windowHeight, 0);
 glMatrixMode(GL_MODELVIEW);
 }

 void resetPerspectiveProjection() {
 glMatrixMode(GL_PROJECTION);
 glPopMatrix();
 glMatrixMode(GL_MODELVIEW);
 }

 Coordinate_grid getGridCoordinatesFromWindow(Coordinate_window window) {
 int col = ceil(window.x / cellWidth);
 int row = ceil(window.y / cellHeight);
 return Coordinate_grid(row, col);
 }

 Coordinate_window getWindowCoordinatesFromGrid(Coordinate_grid grid) {
 float x = (grid.col - 1) * cellWidth;
 float y = (grid.row) * cellHeight;
 return Coordinate_window(x, y);
 }

 void renderBitmapString(Coordinate_grid grid, const char *string) {
 Coordinate_window window = getWindowCoordinatesFromGrid(grid);
 setOrthographicProjection();
 glPushMatrix();
 glLoadIdentity();

 const char *c;
 glRasterPos2f(window.x, window.y);
 for (c = string; *c != '\0'; c++) {
 glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
 }

 glPopMatrix();
 resetPerspectiveProjection();
 }*/

#endif

-----------------------------------------------------------------

#ifndef TIMER_H
#define TIMER_H

#define TIMER_ITEM_GLOBAL 10000
#define TIMER_MAGIC_SPELL 36000
#define TIMER_CURSE 12000
#define TIMER_HERO_REBORN 10000

#define TIMER_HOST_WAIT 30000
#define TIMER_PAGE_CREATING_GAME 5000

#define REFRESH_RATE 100

void timerRefresh(int value) {
	glutPostRedisplay();
	glutTimerFunc(REFRESH_RATE, timerRefresh, 0);
}

void timerItemGlobal(int whichPlayer) {
	if (!players[whichPlayer].isTimerItemGlobalRunning) { //timer not running
		cout << "starting timerItemGlobal for player: " << whichPlayer << endl;
		players[whichPlayer].isTimerItemGlobalRunning = true;
		glutTimerFunc(TIMER_ITEM_GLOBAL, timerItemGlobal, whichPlayer);
	}

	else { //timer not running
		cout << "stopping timerItemGlobal for player: " << whichPlayer << endl;
		players[whichPlayer].isTimerItemGlobalRunning = false;
	}
}

void timerHeroBorn(int whichPlayer) {
	if (!players[whichPlayer].isHeroRebornTimer) {
		cout << "starting timerHeroBorn for player: " << whichPlayer << endl;
		players[whichPlayer].isHeroRebornTimer = true;
		glutTimerFunc(TIMER_HERO_REBORN, timerHeroBorn, whichPlayer);
	} else {
		cout << "stopping timerHeroBorn for player: " << whichPlayer << endl;
		players[whichPlayer].isHeroRebornTimer = false;
	}
}

void timerMagicSpell(int whichPlayer) {
	if (!players[whichPlayer].isTimerMagicSpellRunning) {
		cout << "starting timerMagicSpell for player: " << whichPlayer << endl;
		players[whichPlayer].isTimerMagicSpellRunning = true;
		players[whichPlayer].currPowerMode = POWER_MODE_BASIC;
		glutTimerFunc(TIMER_MAGIC_SPELL, timerMagicSpell, whichPlayer);
	} else {
		cout << "stopping timerMagicSpell for player: " << whichPlayer << endl;
		players[whichPlayer].isTimerMagicSpellRunning = false;
	}
}

void timerCurse(int whichPlayer) {

	if (!players[whichPlayer].isTimerCurseRunning) {
		cout << "starting timerCurse for player " << whichPlayer << endl;
		switch (players[whichPlayer].curseType) {
		case CURSE_STUN:
			players[whichPlayer].speedMoveTemp = players[whichPlayer].speedMove;
			players[whichPlayer].speedMove = SPEED_MIN;
			players[whichPlayer].currPowerMode = POWER_MODE_STUN;
			break;
		case CURSE_DISABLE:
			players[whichPlayer].currPowerMode = POWER_MODE_BASIC;
			//Nothing to be done over here
			break;
		case CURSE_WEAK: //decrease strength
			players[whichPlayer].strength -= CURSE_AMT_SLOW_STRENGTH;
			break;
		case CURSE_BURST:
			//Nothing to be done over here
			break;
		case CURSE_NONE:
			cout << "timerCurse should not reach here" << endl;
			break;
		}

		players[whichPlayer].isTimerCurseRunning = true;
		glutTimerFunc(TIMER_CURSE, timerCurse, whichPlayer);
	}

	else {
		cout << "stopping timerCurse for player " << whichPlayer << endl;
		players[whichPlayer].isTimerCurseRunning = false;

		switch (players[whichPlayer].curseType) {
		case CURSE_STUN:
			players[whichPlayer].currPowerMode = POWER_MODE_BASIC;
			players[whichPlayer].speedMove
					+= players[whichPlayer].speedMoveTemp;
			if (players[whichPlayer].speedMove > SPEED_MAX) {
				players[whichPlayer].speedMove = SPEED_MAX;
			}
			break;
		case CURSE_DISABLE:
			//Nothing to be done over here
			break;
		case CURSE_WEAK:
			players[whichPlayer].strength += CURSE_AMT_SLOW_STRENGTH;
			break;
		case CURSE_BURST:
			//Nothing to be done over here
			break;
		case CURSE_NONE:
			cout << "timerCurse should not reach here" << endl;
			break;

		}

		players[whichPlayer].curseType = CURSE_NONE;
	}
}

bool isstartedHostWait = false;
void timerHostWait(int value) {
	if (!isstartedHostWait) {
		cout << "started timerHostWait" << endl;
		isstartedHostWait = true;
		glutTimerFunc(TIMER_HOST_WAIT, timerHostWait, 0);
	} else {
		cout << "stopping timerHostWait" << endl;
		gameDetails.isTimerNotHostWaiting = true;
	}
}

bool isstartedPageCreating = false;
void timerPageCreatingGame(int value) {
	if (!isstartedPageCreating) {
		cout << "starting timerPageCreatingGame" << endl;
		isstartedPageCreating = true;
		glutTimerFunc(TIMER_PAGE_CREATING_GAME, timerPageCreatingGame, 0);
	}

	else {
		cout << "stopping timerPageCreatingGame" << endl;
		gameDetails.isDoneWithJoining = true;
	}
}

#endif

-------------------------------------------------------------------------------------------

#ifndef VALIDATIONS_H
#define VALIDATIONS_H

#include "CustomVectorStruct.h"

bool isValidRowNColIndex(Coordinate_grid grid, int isInner) {
	if (grid.row < START_GRID_ROW || grid.row > END_GRID_ROW) {
		return false;
	}

	if (isInner) {
		if (grid.col < START_INNER_GRID_COL || grid.col > END_INNER_GRID_COL) {
			return false;
		}
	}

	else {
		if (grid.col < START_OUTER_GRID_COL || grid.col > END_OUTER_GRID_COL) {
			return false;
		}
	}

	return true;
}

bool isValidCell(Coordinate_grid grid) { //Checking it's not in black screen
	return isValidRowNColIndex(grid, false);
}

bool isOponentCellForTeam(Coordinate_grid grid, int whichPlayer) { //returns true if the passed cell lies in my opponent area
	int myTeam = players[whichPlayer].team->name;

	if (myTeam == TEAM_ANGELS && grid.col >= grid.row + DIAG_BLOCKS + 1) {
		return true;
	}

	if (myTeam == TEAM_DEMONS && grid.row >= grid.col + DIAG_BLOCKS + 1) {
		return true;
	}

	return false;
}

//------------------------------------Change for single player (start)----------
bool isAttributeSpace(Coordinate_grid grid) {
	return !isValidRowNColIndex(grid, true);

}
//------------------------------------Change for single player (end)----------

#endif


-----------------------------------------------------------------------------------


#ifndef WINDOWS_H
#define WINDOWS_H

#include "text3d.h"
#include "Network.h"
#include "AI.h"

void moveToWindow(void f()) {
	glutDestroyWindow(windowId_current);
	t3dCleanup();
	f();
}

void create_window_final();
void create_window_main();
void create_window_joiningGame();
void create_window_waiting();
void create_window_selectHero();
void create_window_selectTeam();
void create_window_multiplayer();
void create_window_rules();
void create_window_players();
void create_window_controls();
void create_window_about();
void create_window_first();

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_FIRST--------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_first() {
	texId_bg = getTextureFromPng(PATH_IMG_BG);
	texId_logo = getTextureFromPng(PATH_IMG_LOGO);

	texId_icon_about = getTextureFromPng(PATH_IMG_ICON_ABOUT);
	texId_icon_controls = getTextureFromPng(PATH_IMG_ICON_CONTROLS);
	texId_icon_players = getTextureFromPng(PATH_IMG_ICON_PLAYERS);
	texId_icon_rules = getTextureFromPng(PATH_IMG_ICON_RULES);

	texId_player_single = getTextureFromPng(PATH_IMG_PLAYER_SINGLE);
	texId_player_multi = getTextureFromPng(PATH_IMG_PLAYER_MULTI);
}

void putImages_first() {
	putPngToCell(Coordinate_grid(5, 6), texId_logo, 14, 4);

	putPngWithChar(18, 6, texId_icon_about, CLICK_ABOUT, 2, 2);
	putPngWithChar(18, 10, texId_icon_controls, CLICK_CONTROLS, 2, 2);
	putPngWithChar(18, 14, texId_icon_players, CLICK_PLAYERS, 2, 2);
	putPngWithChar(18, 18, texId_icon_rules, CLICK_RULES, 2, 2);

	putPngWithChar(12, 7, texId_player_single, CLICK_PLAYER_SINGLE, 4, 1);
	putPngWithChar(12, 15, texId_player_multi, CLICK_PLAYER_MULTI, 4, 1);

	putPngToCell(Coordinate_grid(20, 1), texId_bg, 24, 20);
}

//Initializes 3D rendering
void initRendering_first() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	angelsTeam.name = TEAM_ANGELS;
	demonsTeam.name = TEAM_DEMONS;

	//Loading all the textures from images
	loadTextures_first();
	t3dInit();
}

//Draws the 3D scene
void drawScene_first() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective


	putImages_first();
	//renderGrid();
	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_first(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void processLeftClick_first() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_ABOUT:
		moveToWindow(create_window_about);
		break;
	case CLICK_CONTROLS:
		moveToWindow(create_window_controls);
		break;
	case CLICK_PLAYERS:
		moveToWindow(create_window_players);
		break;
	case CLICK_RULES:
		moveToWindow(create_window_rules);
		break;
	case CLICK_PLAYER_SINGLE:
		gameDetails.isSinglePlayerGame = true;
		moveToWindow(create_window_selectTeam);
		break;
	case CLICK_PLAYER_MULTI:
		gameDetails.isSinglePlayerGame = false;
		moveToWindow(create_window_multiplayer);
		break;
	}
}

void myMouseClickHandler_first(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
		processLeftClick_first();
	}
}

void create_window_first() {
	windowId_current = glutCreateWindow("Welcome!!!");
	initRendering_first(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_first);
	glutKeyboardFunc(handleKeypress_first);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_first);

	timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_ABOUT---------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_about() {
	texId_page_about = getTextureFromPng(PATH_IMG_PAGE_ABOUT);

	texId_btn_back = getTextureFromPng(PATH_IMG_BACK);
}

void putImages_about() {
	putPngWithChar(19, 12, texId_btn_back, CLICK_BACK, 2, 2);

	putPngToCell(Coordinate_grid(20, 1), texId_page_about, 24, 20);
}

//Initializes 3D rendering
void initRendering_about() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_about();
	t3dInit();
}

//Draws the 3D scene
void drawScene_about() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putImages_about();
	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_about(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void processLeftClick_about() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_BACK:
		moveToWindow(create_window_first);
		break;
	}
}

void myMouseClickHandler_about(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
		processLeftClick_about();
	}
}

void create_window_about() {
	windowId_current = glutCreateWindow("About Game");
	initRendering_about(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_about);
	glutKeyboardFunc(handleKeypress_about);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_about);

	//timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_CONTROLS---------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_controls() {
	texId_page_controls = getTextureFromPng(PATH_IMG_PAGE_CONTROLS);

	texId_btn_back = getTextureFromPng(PATH_IMG_BACK);
}

void putImages_controls() {
	putPngWithChar(19, 12, texId_btn_back, CLICK_BACK, 2, 2);

	putPngToCell(Coordinate_grid(20, 1), texId_page_controls, 24, 20);
}

//Initializes 3D rendering
void initRendering_controls() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_controls();
	t3dInit();
}

//Draws the 3D scene
void drawScene_controls() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putImages_controls();
	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_controls(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void processLeftClick_controls() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_BACK:
		moveToWindow(create_window_first);
		break;
	}
}

void myMouseClickHandler_controls(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
		processLeftClick_controls();
	}
}

void create_window_controls() {
	windowId_current = glutCreateWindow("Controls");
	initRendering_controls(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_controls);
	glutKeyboardFunc(handleKeypress_controls);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_controls);

	//timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_PLAYERS---------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_players() {
	texId_page_players = getTextureFromPng(PATH_IMG_PAGE_PLAYERS);

	texId_btn_back = getTextureFromPng(PATH_IMG_BACK);
}

void putImages_players() {
	putPngWithChar(19, 12, texId_btn_back, CLICK_BACK, 2, 2);

	putPngToCell(Coordinate_grid(20, 1), texId_page_players, 24, 20);
}

//Initializes 3D rendering
void initRendering_players() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_players();
	t3dInit();
}

//Draws the 3D scene
void drawScene_players() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putImages_players();
	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_players(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void processLeftClick_players() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_BACK:
		moveToWindow(create_window_first);
		break;
	}
}

void myMouseClickHandler_players(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
		processLeftClick_players();
	}
}

void create_window_players() {
	windowId_current = glutCreateWindow("Players");
	initRendering_players(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_players);
	glutKeyboardFunc(handleKeypress_players);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_players);

	//timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_RULES---------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_rules() {
	texId_page_rules = getTextureFromPng(PATH_IMG_PAGE_RULES);

	texId_btn_back = getTextureFromPng(PATH_IMG_BACK);
}

void putImages_rules() {
	putPngWithChar(19, 12, texId_btn_back, CLICK_BACK, 2, 2);

	putPngToCell(Coordinate_grid(20, 1), texId_page_rules, 24, 20);
}

//Initializes 3D rendering
void initRendering_rules() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_rules();
	t3dInit();
}

//Draws the 3D scene
void drawScene_rules() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putImages_rules();
	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_rules(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void processLeftClick_rules() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_BACK:
		moveToWindow(create_window_first);
		break;
	}
}

void myMouseClickHandler_rules(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
		processLeftClick_rules();
	}
}

void create_window_rules() {
	windowId_current = glutCreateWindow("Rules");
	initRendering_rules(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_rules);
	glutKeyboardFunc(handleKeypress_rules);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_rules);

	timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_MULTIPLAYER--------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
string hostIp;
bool isShowInput;

void loadTextures_multiplayer() {
	texId_bg = getTextureFromPng(PATH_IMG_BG);

	texId_join_game = getTextureFromPng(PATH_IMG_JOIN_GAME);
	texId_host_game = getTextureFromPng(PATH_IMG_HOST_GAME);

	texId_white = getTextureFromPng(PATH_IMG_WHITE);
	texId_btn_next = getTextureFromPng(PATH_IMG_BTN_NEXT);
	texId_try_again = getTextureFromPng(PATH_IMG_TRY_AGAIN);
	texId_btn_reset = getTextureFromPng(PATH_IMG_BTN_RESET);
}

void putImages_multiplayer() {
	putPngWithChar(7, 6, texId_host_game, CLICK_HOST_GAME, 4, 2);
	putPngWithChar(7, 16, texId_join_game, CLICK_JOIN_GAME, 4, 2);

	if (isShowInput) {
		putTextToCell(Coordinate_grid(9, 16), hostIp);

		//Important - keep the "coordinates & blocks" same here & in below(else) part
		putPngWithChar(9, 16, texId_input, INPUT, 4, 1);
		putPngWithChar(9, 20, texId_btn_reset, CLICK_RESET, 1, 1);

		putPngWithChar(9, 22, texId_btn_next, CLICK_NEXT, 1, 1);

		if (gameDetails.isIssueConnectingToServer) {
			putPngToCell(Coordinate_grid(11, 16), texId_try_again, 4, 1);
		}
	} else {
		hostIp = "";
		//below is imp to avoid clicks if the button is not visible
		putChars(4, 1, 9, 16, BG_GRASS); //input button
		putChars(1, 1, 9, 20, BG_GRASS); //input button
		putChars(1, 1, 9, 22, BG_GRASS); //next button
		putChars(4, 1, 11, 16, BG_GRASS); //tryAgain button
	}

	putPngToCell(Coordinate_grid(20, 1), texId_bg, 24, 20);
}

//Initializes 3D rendering
void initRendering_multiplayer() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gameDetails.isIssueConnectingToServer = false;
	isShowInput = false;

	loadTextures_multiplayer();
	createServerThread();
	gameDetails.isTimerNotHostWaiting = true; //making this true over here because we don't want any client to connect

	t3dInit();
}

//Draws the 3D scene
void drawScene_multiplayer() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putImages_multiplayer();
	glutSwapBuffers(); //Send the 3D scene to the screen
}

void processClickNext() {
	if (hostIp == "") {
		return;
	}

	gameDetails.hostDetails = convertToNodeHelper(str2Char(hostIp));

	connectStatus connect = helperSendConnect();
	switch (connect) {
	case CONNECTED_NOT:
		break;
	case CONNECTED_ALREADY:
		moveToWindow(create_window_joiningGame);
		break;
	case CONNECTED_NEW:
		moveToWindow(create_window_selectTeam);
		break;
	}
}

//Called when a key is pressed
void handleKeypress_multiplayer(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	case 57: //'9'
		hostIp = concat(hostIp, "9");
		break;
	case 56://'8'
		hostIp = concat(hostIp, "8");
		break;
	case 55://'7'
		hostIp = concat(hostIp, "7");
		break;
	case 54://'6'
		hostIp = concat(hostIp, "6");
		break;
	case 53://'5'
		hostIp = concat(hostIp, "5");
		break;
	case 52://'4'
		hostIp = concat(hostIp, "4");
		break;
	case 51://'3'
		hostIp = concat(hostIp, "3");
		break;
	case 50://'2'
		hostIp = concat(hostIp, "2");
		break;
	case 49://'1'
		hostIp = concat(hostIp, "1");
		break;
	case 48://'0'
		hostIp = concat(hostIp, "0");
		break;
	case 46://'.'
		hostIp = concat(hostIp, ".");
		break;
	case 58://':'
		hostIp = concat(hostIp, ":");
		break;
	case 8://'backspace'
		hostIp = remLastCharFromStr(hostIp);
		break;
	case 13://'enter'
		processClickNext();
		break;
	}
}

void processClickHost() {
	isShowInput = false;
	gameDetails.isHost = true;

	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		players[i].status = STATUS_NOT_JOINED;
	}

	gameDetails.isIssueConnectingToServer = false;
	moveToWindow(create_window_selectTeam);
}

void processLeftClick_multiplayer() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_JOIN_GAME:
		gameDetails.isHost = false;
		isShowInput = true;
		break;

	case CLICK_HOST_GAME:
		processClickHost();
		break;

	case CLICK_NEXT:
		processClickNext();
		break;

	case CLICK_RESET:
		hostIp = "";
		break;
	}
}

void myMouseClickHandler_multiplayer(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
		processLeftClick_multiplayer();
	}
}

void create_window_multiplayer() {
	windowId_current = glutCreateWindow("Server Details!!!");
	initRendering_multiplayer(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_multiplayer);
	glutKeyboardFunc(handleKeypress_multiplayer);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_multiplayer);

	timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_SELECT_TEAM--------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
bool isTeamVisible[2];

void loadTextures_selectTeam() {
	texId_bg = getTextureFromPng(PATH_IMG_BG);

	texId_pick_team = getTextureFromPng(PATH_IMG_PICK_TEAM);

	texId_att_team_angels = getTextureFromPng(PATH_IMG_ATT_TEAM_ANGELS);
	texId_att_team_demons = getTextureFromPng(PATH_IMG_ATT_TEAM_DEMONS);

	texId_t_angels = getTextureFromBmp(PATH_IMG_TEMPLE_ANGELS);
	texId_t_demons = getTextureFromBmp(PATH_IMG_TEMPLE_DEMONS);
}

void putImages_selectTeam() {
	putPngWithChar(5, 8, texId_pick_team, CLICK_PICK_TEAM, 9, 2);

	if (isTeamVisible[TEAM_ANGELS]) {
		putBmpWithChar(11, 6, texId_t_angels, CLICK_ANGELS, 4, 4);
		putPngWithChar(13, 6, texId_att_team_angels, CLICK_ANGELS, 4, 2);
	} else {
		putChars(4, 4, 11, 6, BG_GRASS);
		putChars(4, 2, 13, 6, BG_GRASS);
	}

	if (isTeamVisible[TEAM_DEMONS]) {
		putBmpWithChar(11, 16, texId_t_demons, CLICK_DEMONS, 4, 4);
		putPngWithChar(13, 16, texId_att_team_demons, CLICK_DEMONS, 4, 2);
	} else {
		putChars(4, 4, 11, 16, BG_GRASS);
		putChars(4, 2, 13, 16, BG_GRASS);
	}

	putPngToCell(Coordinate_grid(20, 1), texId_bg, 24, 20);
}

//Initializes 3D rendering
void initRendering_selectTeam() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_selectTeam();

	isTeamVisible[TEAM_ANGELS] = true;
	isTeamVisible[TEAM_DEMONS] = true;

	t3dInit();
}

//Draws the 3D scene
void drawScene_selectTeam() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putImages_selectTeam();
	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_selectTeam(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void teamSelected_next(TeamStruct* team) {
	//------------------------------------Change for single player (start)----------
	if (gameDetails.isSinglePlayerGame) {
		gameDetails.isHost = true;

		players[PLAYER_ID_PRIMARY].team = team;
		cout << "set primary team as: " << team->name << endl;
		moveToWindow(create_window_selectHero);
	}
	//------------------------------------Change for single player (end)----------

	if (gameDetails.isHost) {
		players[PLAYER_ID_PRIMARY].team = team; //setting teamName in case of host only
		moveToWindow(create_window_selectHero);
	}

	else {
		if (helperValidateTeam(team->name)) {
			moveToWindow(create_window_selectHero);
		}

		else {
			isTeamVisible[team->name] = false;
		}
	}
}

void processLeftClick_selectTeam() {
	switch (getGridChar(downGrid_click)) {
	case CLICK_ANGELS:
		teamSelected_next(&angelsTeam);
		break;
	case CLICK_DEMONS:
		teamSelected_next(&demonsTeam);
		break;
	}
}

void myMouseClickHandler_selectTeam(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
		processLeftClick_selectTeam();
	}
}

void create_window_selectTeam() {
	windowId_current = glutCreateWindow("Select a team!!!");
	initRendering_selectTeam(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_selectTeam);
	glutKeyboardFunc(handleKeypress_selectTeam);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_selectTeam);

	timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_SELECT_HERO--------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
bool isHeroVisible[4];

void loadTextures_selectHero() {
	texId_bg = getTextureFromPng(PATH_IMG_BG);
	texId_pick_hero = getTextureFromPng(PATH_IMG_PICK_HERO);

	texId_h_stunner = getTextureFromBmp(PATH_IMG_HERO_STUNNER);
	texId_h_disabler = getTextureFromBmp(PATH_IMG_HERO_DISABLER);
	texId_h_slower = getTextureFromBmp(PATH_IMG_HERO_SLOWER);
	texId_h_burster = getTextureFromBmp(PATH_IMG_HERO_BURSTER);

	texId_att_h_burster = getTextureFromPng(PATH_IMG_ATT_HERO_BURSTER);
	texId_att_h_disabler = getTextureFromPng(PATH_IMG_ATT_HERO_DISABLER);
	texId_att_h_slower = getTextureFromPng(PATH_IMG_ATT_HERO_SLOWER);
	texId_att_h_stunner = getTextureFromPng(PATH_IMG_ATT_HERO_STUNNER);

	texId_att_team_angels = getTextureFromPng(PATH_IMG_ATT_TEAM_ANGELS);
	texId_att_team_demons = getTextureFromPng(PATH_IMG_ATT_TEAM_DEMONS);

	texId_t_angels = getTextureFromBmp(PATH_IMG_TEMPLE_ANGELS);
	texId_t_demons = getTextureFromBmp(PATH_IMG_TEMPLE_DEMONS);
}

void PutImages_selectHero() {
	if (isHeroVisible[HERO_SLOWER]) {
		putBmpWithChar(9, 6, texId_h_slower, H_SLOWER, 4, 4);
		putPngWithChar(10, 6, texId_att_h_slower, H_SLOWER, 4, 1);
	} else {
		putChars(4, 4, 9, 6, BG_GRASS);
		putChars(4, 1, 10, 6, BG_GRASS);
	}

	if (isHeroVisible[HERO_BURSTER]) {
		putBmpWithChar(9, 16, texId_h_burster, H_BURSTER, 4, 4);
		putPngWithChar(10, 16, texId_att_h_burster, H_BURSTER, 4, 1);
	} else {
		putChars(4, 4, 9, 16, BG_GRASS);
		putChars(4, 1, 10, 16, BG_GRASS);
	}

	if (isHeroVisible[HERO_STUNNER]) {
		putBmpWithChar(17, 6, texId_h_stunner, H_STUNNER, 4, 4);
		putPngWithChar(18, 6, texId_att_h_stunner, H_STUNNER, 4, 1);
	} else {
		putChars(4, 4, 17, 6, BG_GRASS);
		putChars(4, 1, 18, 6, BG_GRASS);
	}

	if (isHeroVisible[HERO_DISABLER]) {
		putBmpWithChar(17, 16, texId_h_disabler, H_DISABLER, 4, 4);
		putPngWithChar(18, 16, texId_att_h_disabler, H_DISABLER, 4, 1);
	} else {
		putChars(4, 4, 17, 16, BG_GRASS);
		putChars(4, 1, 18, 16, BG_GRASS);
	}

	putPngToCell(Coordinate_grid(3, 10), texId_pick_hero, 6, 1);
	putPngToCell(Coordinate_grid(20, 1), texId_bg, 24, 20);
}

//Initializes 3D rendering
void initRendering_selectHero() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	loadTextures_selectHero();

	isHeroVisible[HERO_STUNNER] = true;
	isHeroVisible[HERO_SLOWER] = true;
	isHeroVisible[HERO_BURSTER] = true;
	isHeroVisible[HERO_DISABLER] = true;

	t3dInit();
}

//Draws the 3D scene
void drawScene_selectHero() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective


	PutImages_selectHero();
	glutSwapBuffers();
}

//Called when a key is pressed
void handleKeypress_selectHero(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void heroSelected_next(heroes hero) {

	//------------------------------------Change for single player (start)----------
	if (gameDetails.isSinglePlayerGame) {
		players[PLAYER_ID_PRIMARY].heroType = hero;
		cout << "set primary hero as: " << hero << endl;
		moveToWindow(create_window_joiningGame);
	}
	//------------------------------------Change for single player (end)----------


	if (gameDetails.isHost) {
		players[PLAYER_ID_PRIMARY].heroType = hero; //setting hero in case of host only
		moveToWindow(create_window_waiting);
	}

	else {
		if (helperValidateHero(hero)) {
			moveToWindow(create_window_joiningGame);
		}

		else {
			isHeroVisible[hero] = false;
		}
	}
}

void processLeftClick_selectHero() {
	switch (getGridChar(downGrid_click)) {
	case H_SLOWER:
		heroSelected_next(HERO_SLOWER);
		break;
	case H_STUNNER:
		heroSelected_next(HERO_STUNNER);
		break;
	case H_DISABLER:
		heroSelected_next(HERO_DISABLER);
		break;
	case H_BURSTER:
		heroSelected_next(HERO_BURSTER);
		break;
	}
}

void myMouseClickHandler_selectHero(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
		processLeftClick_selectHero();
	}
}

void create_window_selectHero() {
	windowId_current = glutCreateWindow("Select a Hero!!!");
	initRendering_selectHero(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_selectHero);
	glutKeyboardFunc(handleKeypress_selectHero);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_selectHero);

	timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_WAITING------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/

void loadTextures_waiting() {
	texId_bg_waiting = getTextureFromPng(PATH_IMG_BG_WAITING);
}

void putImages_waiting() {
	putPngToCell(Coordinate_grid(20, 1), texId_bg_waiting, 24, 20);
}

//Initializes 3D rendering
void initRendering_waiting() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gameDetails.isTimerNotHostWaiting = false;

	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		//No player has joined yet
		players[i].status = STATUS_NOT_JOINED;
	}

	//setting some details here since else not set anywhere ;)
	currPlayerId = PLAYER_ID_PRIMARY;
	players[currPlayerId].networkDetails = selfNode;
	gameDetails.hostDetails = selfNode;
	players[currPlayerId].status = STATUS_PRESENT;
	players[currPlayerId].isFirstPlayerInTeam = true;

	//Initializing the herotypes to default
	for (int i = 1; i < NUM_OF_PLAYERS; i++) {
		players[i].heroType = HERO_NOT_PRESENT;
	}

	timerHostWait(0);
	loadTextures_waiting();

	t3dInit();
}

//Draws the 3D scene
void drawScene_waiting() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putTextToCell(Coordinate_grid(17, 7),
			concat("Host Ip-> ", selfNode->ipWithPort));
	putImages_waiting();

	if (gameDetails.isTimerNotHostWaiting) {
		supportBroadCast(BROADCAST_JOINING);
		moveToWindow(create_window_joiningGame);
	}

	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_waiting(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void processLeftClick_waiting() {
}

void myMouseClickHandler_waiting(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
		processLeftClick_waiting();
	}
}

void create_window_waiting() {
	windowId_current = glutCreateWindow("Waiting for others to join...");
	initRendering_waiting(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_waiting);
	glutKeyboardFunc(handleKeypress_waiting);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_waiting);

	timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * -----------------------------------------------------------WINDOW_JOINING_GAME---------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_joiningGame() {
	texId_page_join = getTextureFromPng(PATH_IMG_PAGE_JOIN);
}

void putImages_joiningGame() {
	putPngToCell(Coordinate_grid(20, 1), texId_page_join, 24, 20);
}

void setAttributes() {
	initMap();

	//Placing items in the map
	for (int i = 0; i < ITEMS_ON_MAP_COUNT; i++) {
		initItemAtRandomPos();
	}

	loadTeamAttributes();
	for (int i = 0; i < NUM_OF_PLAYERS; i++) {
		if (players[i].status == STATUS_PRESENT) {
			loadPlayerGeneralAttributes(i);
		}
	}
	copyPrimaryGrid();

	blockOpponentsArea();
}

void processJoinForSinglePlayer() {
	currPlayerId = PLAYER_ID_PRIMARY;
	players[currPlayerId].networkDetails = selfNode;
	gameDetails.hostDetails = selfNode;

	//--selecting teams
	players[1].team = players[0].team; //my friend

	if (players[0].team->name == TEAM_ANGELS) {
		players[2].team = &demonsTeam;
		players[3].team = &demonsTeam;
	} else {
		players[2].team = &angelsTeam;
		players[3].team = &angelsTeam;
	}

	//--selecting heroes
	int k = 1;
	for (int i = 0; i < 4; i++) {
		if (players[0].heroType != i) {
			players[k++].heroType = static_cast<heroes> (i);
		}
	}

	//---setting is firstPlayerInTeam
	players[0].isFirstPlayerInTeam = true;
	players[1].isFirstPlayerInTeam = false;
	players[2].isFirstPlayerInTeam = true;
	players[3].isFirstPlayerInTeam = false;

	//---all are present
	players[0].status = STATUS_PRESENT;
	players[1].status = STATUS_PRESENT;
	players[2].status = STATUS_PRESENT;
	players[3].status = STATUS_PRESENT;

	setAttributes();
}

//Initializes 3D rendering
void initRendering_joiningGame() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	loadTextures_joiningGame();
	//------------------------------------Change for single player (start)----------
	if (gameDetails.isSinglePlayerGame) {
		processJoinForSinglePlayer();
	}
	//------------------------------------Change for single player (end)----------
	gameDetails.isStartJoiningTimer = false;
	gameDetails.isDoneWithJoining = false;
	t3dInit();
}

bool isCalledJoiningFunctions = false;
bool isFineToCallJoiningFunctions() {
	if (isCalledJoiningFunctions) {
		return false;
	}
	if (gameDetails.isHost) {
		isCalledJoiningFunctions = true;
		return true;
	}
	if (gameDetails.isStartJoiningTimer) {
		isCalledJoiningFunctions = true;
		return true;
	}
	return false;
}

//Draws the 3D scene
void drawScene_joiningGame() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putImages_joiningGame();

	//------------------------------------Change for single player (start)----------
	if (gameDetails.isSinglePlayerGame) {
		moveToWindow(create_window_main);
	}
	//------------------------------------Change for single player (end)----------
	if (isFineToCallJoiningFunctions()) {
		timerPageCreatingGame(0);
		helperRequestPlayersDetails();
		setAttributes();
		if (gameDetails.isHost) {
			createClientBroadcastThread();
		}
	}

	if (gameDetails.isDoneWithJoining) {
		moveToWindow(create_window_main);
	}

	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_joiningGame(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void processLeftClick_joiningGame() {

}

void myMouseClickHandler_joiningGame(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
		processLeftClick_joiningGame();
	}
}

void create_window_joiningGame() {
	windowId_current = glutCreateWindow("Joining Game!!!");
	initRendering_joiningGame(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_joiningGame);
	glutKeyboardFunc(handleKeypress_joiningGame);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_joiningGame);

	timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_MAIN---------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_main() {
	texId_bg_grass = getTextureFromBmp(PATH_IMG_BG_WINDOW_MAIN);
	texId_bg_spawn = getTextureFromBmp(PATH_IMG_BG_SPAWN);
	texId_bg_war = getTextureFromBmp(PATH_IMG_BG_WAR);
	texId_bg_attribute = getTextureFromBmp(PATH_IMG_BG_ATTRIBUTE);
	texId_bg_blocked = getTextureFromBmp(PATH_IMG_BG_BLOCKED);

	texId_tree = getTextureFromBmp(PATH_IMG_TREE);
	texId_stone = getTextureFromBmp(PATH_IMG_STONE);
	texId_tree_war = getTextureFromBmp(PATH_IMG_TREE_WAR);
	texId_stone_war = getTextureFromBmp(PATH_IMG_STONE_WAR);

	texId_t_angels = getTextureFromBmp(PATH_IMG_TEMPLE_ANGELS);
	texId_t_demons = getTextureFromBmp(PATH_IMG_TEMPLE_DEMONS);

	texId_h_stunner = getTextureFromBmp(PATH_IMG_HERO_STUNNER);
	texId_h_disabler = getTextureFromBmp(PATH_IMG_HERO_DISABLER);
	texId_h_slower = getTextureFromBmp(PATH_IMG_HERO_SLOWER);
	texId_h_burster = getTextureFromBmp(PATH_IMG_HERO_BURSTER);

	texId_i_speedMov = getTextureFromBmp(PATH_IMG_ITEM_MOVEMENT_SPEED);
	texId_i_damage = getTextureFromBmp(PATH_IMG_ITEM_DAMAGE);
	texId_i_health = getTextureFromBmp(PATH_IMG_ITEM_HEALTH);
	texId_i_tHealer = getTextureFromBmp(PATH_IMG_ITEM_TEMPLE_HEALER);

	texId_att_cBurst = getTextureFromPng(PATH_IMG_ATT_CURSE_BURST);
	texId_att_cDisable = getTextureFromPng(PATH_IMG_ATT_CURSE_DISABLE);
	texId_att_cSlow = getTextureFromPng(PATH_IMG_ATT_CURSE_SLOW);
	texId_att_cStun = getTextureFromPng(PATH_IMG_ATT_CURSE_STUN);
	texId_att_cNone = getTextureFromPng(PATH_IMG_ATT_CURSE_NONE);

	texId_att_h_burster = getTextureFromPng(PATH_IMG_ATT_HERO_BURSTER);
	texId_att_h_disabler = getTextureFromPng(PATH_IMG_ATT_HERO_DISABLER);
	texId_att_h_slower = getTextureFromPng(PATH_IMG_ATT_HERO_SLOWER);
	texId_att_h_stunner = getTextureFromPng(PATH_IMG_ATT_HERO_STUNNER);

	texId_att_mBasic = getTextureFromPng(PATH_IMG_ATT_MODE_BASIC);
	texId_att_mMagic = getTextureFromPng(PATH_IMG_ATT_MODE_MAGIC);
	texId_att_mStun = getTextureFromPng(PATH_IMG_ATT_MODE_STUN);

	texId_att_team_angels = getTextureFromPng(PATH_IMG_ATT_TEAM_ANGELS);
	texId_att_team_demons = getTextureFromPng(PATH_IMG_ATT_TEAM_DEMONS);

	texId_att_team_enemy = getTextureFromPng(PATH_IMG_ATT_TEAM_ENEMY);
	texId_att_team_my = getTextureFromPng(PATH_IMG_ATT_TEAM_MY);

	texId_att_time_itemOff = getTextureFromPng(PATH_IMG_ATT_TIMERS_ITEM_OFF);
	texId_att_time_itemOn = getTextureFromPng(PATH_IMG_ATT_TIMERS_ITEM_ON);
	texId_att_time_magicOff = getTextureFromPng(PATH_IMG_ATT_TIMERS_MAGIC_OFF);
	texId_att_time_magicOn = getTextureFromPng(PATH_IMG_ATT_TIMERS_MAGIC_ON);

	texId_att_health = getTextureFromPng(PATH_IMG_ATT_HEALTH);
	texId_att_temple_health = getTextureFromPng(PATH_IMG_ATT_TEMPLE_HEALTH);
	texId_att_strength = getTextureFromPng(PATH_IMG_ATT_STRENGTH);
	texId_att_speed = getTextureFromPng(PATH_IMG_ATT_SPEED);

	texId_att_wall = getTextureFromPng(PATH_IMG_ATT_WALL);
}

//Initializes 3D rendering
void initRendering_main() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_main();

	t3dInit();

	gameDetails.isGameOver = false;
	if (gameDetails.isHost) {
		isGameOver = false;
		//createClientBroadcastThread(); //Created in joining page
		createUpdateServerThread();
		createRectifyItemsThread();
	}

	//------------------------------------Change for single player (start)----------
	if (gameDetails.isSinglePlayerGame) {
		//Creating AI threads
		ai1 = new AI(1, players[1].location, AI_NORMAL);
		ai2 = new AI(2, players[2].location, AI_NORMAL);
		ai3 = new AI(3, players[3].location, AI_OFFENSIVE);

		gameDetails.isBotsPaused = false;

		createAIThread1();
		createAIThread2();
		createAIThread3();
	}
	//------------------------------------Change for single player (end)----------
}

int i = 0;
void renderGridMainWindow() {
	if (gameDetails.isHost) {
		if (i % SPEED_ACTUAL(0) == 0) {
			moveHero(0);
		}
		if (i % SPEED_ACTUAL(1) == 0) {
			moveHero(1);
		}
		if (i % SPEED_ACTUAL(2) == 0) {
			moveHero(2);
		}
		if (i % SPEED_ACTUAL(3) == 0) {
			moveHero(3);
		}
		i++;
	}

	loadAttributeSpace();
	renderGrid();
}

//Draws the 3D scene
void drawScene_main() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective


	//printGrid();
	renderGridMainWindow();

	if (gameDetails.isGameOver) {
		moveToWindow(create_window_final);
	}

	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_main(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;

	case 32: //key - 'space' : on/off bgMusic
		toggleBgMusic();
		break;

		//------------------------------------Change for single player (start)----------
	case 112: //key - 'p' : pause game
		if (gameDetails.isSinglePlayerGame) {
			if (gameDetails.isBotsPaused) {
				cout << "Resuming bots" << endl;
				gameDetails.isBotsPaused = false;
			} else {
				cout << "Pausing bots" << endl;
				gameDetails.isBotsPaused = true;
			}
		}
		break;

	case 113: //key - 'q'
		if (gameDetails.isSinglePlayerGame) {
			if (ai1->modeAi == AI_NORMAL) {
				cout << "Changing AI mode to Offensive" << endl;
				ai1->modeAi = AI_OFFENSIVE;
			}

			else if (ai1->modeAi == AI_OFFENSIVE) {
				cout << "Changing AI mode to Normal" << endl;
				ai1->modeAi = AI_NORMAL;
			}
		}
		break;

		//------------------------------------Change for single player (end)----------

	case 49: //key - '1' : select basicPower
		//------------------------------------Change for single player (start)----------
		if (gameDetails.isSinglePlayerGame) {
			requestBasicPowerAI(currPlayerId);
		}
		//------------------------------------Change for single player (end)----------

		else {
			requestBasicPower();
		}
		break;

	case 50: //key - '2' : select magicPower
		//------------------------------------Change for single player (start)----------
		if (gameDetails.isSinglePlayerGame) {
			requestMagicPowerAI(currPlayerId);
		}
		//------------------------------------Change for single player (end)----------

		else {
			requestMagicPower();
		}

		break;

	case 52: //key - '4' //for testing purpose
		cout << "Printing message queue" << endl;
		printQueue(&queuePrimary);
		break;
	}
}

void processRightClick_main() {
	handleGridCharSwitch(downGrid_click, PROCESS_MOVE_RIGHT_CLICK);
}

void processLeftClick_main() {
	handleGridCharSwitch(downGrid_click, PROCESS_MOVE_LEFT_CLICK);
}

void myMouseClickHandler_main(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_RIGHT_BUTTON) { //reaches here only if GLUT_UP
		processRightClick_main();
	} else {
		processLeftClick_main();
	}
}

void create_window_main() {
	windowId_current = glutCreateWindow("Game is on!!!");
	initRendering_main(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_main);
	glutKeyboardFunc(handleKeypress_main);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_main);

	timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec
	playBgSound();
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

#endif

/* --------------------------------------------------------------------------------------------------------------------------------
 * ------------------------------------------------------------------WINDOW_FINAL--------------------------------------------------
 * --------------------------------------------------------------------------------------------------------------------------------*/
void loadTextures_final() {
	texId_page_win_angels = getTextureFromPng(PATH_IMG_PAGE_WIN_ANGELS);
	texId_page_win_demons = getTextureFromPng(PATH_IMG_PAGE_WIN_DEMONS);
}

void putImages_final() {
	if (gameDetails.winningTeam == TEAM_ANGELS) {
		putPngToCell(Coordinate_grid(20, 1), texId_page_win_angels, 24, 20);
	} else if (gameDetails.winningTeam == TEAM_DEMONS) {
		putPngToCell(Coordinate_grid(20, 1), texId_page_win_demons, 24, 20);
	}
}

//Initializes 3D rendering
void initRendering_final() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable((GL_BLEND));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Loading all the textures from images
	loadTextures_final();

	t3dInit();
}

//Draws the 3D scene
void drawScene_final() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //Reset the drawing perspective

	putImages_final();
	glutSwapBuffers(); //Send the 3D scene to the screen
}

//Called when a key is pressed
void handleKeypress_final(unsigned char key, //The key that was pressed
		int x, int y) { //The current mouse coordinates
	switch (key) {
	case 27: //key - 'esc' : exits the program
		t3dCleanup();
		exit(0);
		break;
	}
}

void processLeftClick_final() {
	//handleGridCharSwitch(downGrid_click, PROCESS_MOVE_LEFT_CLICK);
}

void myMouseClickHandler_final(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) { //saving just the state, action is performed on GLUT_UP
		Coordinate_openGl openGl = getOGLPos(x, y);
		downGrid_click = getGridCoordinatesFromOpenGl(openGl);
		return;
	}

	if (!isValidCell(downGrid_click)) {
		playEventSound( PATH_SOUND_WRONG_CLICK);
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { //reaches here only if GLUT_UP
		processLeftClick_final();
	}
}

void create_window_final() {
	windowId_current = glutCreateWindow("Game Over, press 'esc' to exit");
	initRendering_final(); //Initialize rendering

	//set handler functions
	glutDisplayFunc(drawScene_final);
	glutKeyboardFunc(handleKeypress_final);
	glutReshapeFunc(handleResize);
	glutMouseFunc(myMouseClickHandler_final);

	//timerRefresh(0); //redisplays "glutPostRedisplay()" after every 'REFRESH_RATE' msec

	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

-------------------------------------------------------------------------------
