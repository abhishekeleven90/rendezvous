#ifndef AI_H
#define AI_H

#define LATENCY_HUMAN 1100000
#define HEALTH_CRITICAL 30
#define BLOCKS_ITEM 5 //TODO: make it some less
#define HEALTH_TEMPLE_RELATIVE_DIFF 100

class AI {
	int id;
	Coordinate_grid locStart;
	Coordinate_grid locItem;

	pthread_t threadId;
	bool isMovingToSpwan;
	bool isMovingToTakeItem;

	Coordinate_grid myTempleAttackLocs[8];

public:
	AI(int id, Coordinate_grid startLoc) {
		this->id = id;
		this->locStart = startLoc;
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

			if (gameDetails.modeAi == AI_OFFENSIVE) {
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

			if (gameDetails.modeAi == AI_OFFENSIVE) {
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

			cout << "isSpawn for id: " << getId() << "--"
					<< this->isMovingToSpwan << endl;
			cout << "isItem for id: " << getId() << "--"
					<< this->isMovingToTakeItem << endl;

			if (!this->isMovingToSpwan && !this->isMovingToTakeItem) { //Abhishek: also call before attackEnemyPlayer
				usleep(LATENCY_HUMAN);
				attackEnemyTemple();
			}

			if (!this->isMovingToSpwan && gameDetails.modeAi != AI_OFFENSIVE) {
				protectMyTemple();
			}

			usleep(LATENCY_HUMAN);

		}
	}

};

pthread_t aiThreadId1;
pthread_t aiThreadId2;
pthread_t aiThreadId3;

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

void createAIThread1() {
	createThread(&aiThreadId1, startMe1);
}

void createAIThread2() {
	createThread(&aiThreadId2, startMe2);
}

void createAIThread3() {
	createThread(&aiThreadId3, startMe3);
}

#endif
