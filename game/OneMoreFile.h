#ifndef ONE_MORE_FILE_H
#define ONE_MORE_FILE_H

void aStarMove(int whichPlayer, bool through) {
	//Coordinate_grid targetCell = players[whichPlayer].targetCell;
	Coordinate_grid targetCell;
	targetCell.row = players[whichPlayer].targetCell.row; //TODO remove
	 targetCell.col = players[whichPlayer].targetCell.col;

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
	players[whichPlayer].astar->AStar(through);
}

void aStarMoveThrough() {
	aStarMove(currPlayerId, true); //TODO: Abhishek, changed for legacy code
	//for testing with one player
}

void aStarMoveNotThrough() {
	aStarMove(currPlayerId, false); //TODO: Abhishek, changed for legacy code
	//for testing with one player
}

//used by primary node
void selectBasicPower(int whichPlayer) {
	cout << "selected power_basic by player" << whichPlayer << endl;
	players[whichPlayer].currentPowerMode = POWER_MODE_BASIC;
}

//used by primary node
//TODO: magic timer
void selectMagicPower(int whichPlayer) {
	if (players[whichPlayer].curseType != CURSE_DISABLE) {
		cout << "selected power_magic by player " << whichPlayer << endl;
		players[whichPlayer].currentPowerMode = POWER_MODE_MAGIC;
	} else {
		cout << "Player can not use Magic power, you are cursed!!!" << endl;
		//TODO: show on player's wall, will have to send through server
	}
}

#endif
