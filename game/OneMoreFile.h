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
