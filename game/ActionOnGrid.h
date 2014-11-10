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

//TODO: Abhishek: if more items add or try to add to the complex handleGridCharSwitch
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
	}
	setAttackTemple(whichPlayer, false);
	cout << "Yayy " << whichPlayer << " decreased enemy temple health to "
			<< enemyTeam->templeHealth << endl;
	//TODO: Abhishek play temple attack sound
	//TODO: Abhishek gif attack animation

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
	cout << "Yayy " << whichPlayer << " decreased health of " << enemyPlayer
			<< " to " << players[enemyPlayer].heroHealth << endl;
	//TODO: Abhishek play enemy attack sound
	//TODO: Abhishek gif attack animation

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
		//TODO: Abhishek may be redundant code
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
		//TODO: here!!!
	} else {
		decreaseEnemyPlayerHealthHelper(whichPlayer, false);
	}
	cout << "at master node: at end of decrease health" << endl;
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

	cout << "requesting to attack enemy" << endl;
	//note: validation is done itself at the client side
	charCellType toAttackHeroCellType =
			gridChar[onClickTargetCell.row][onClickTargetCell.col];
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

	switch (gridChar[grid.row][grid.col]) {
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
