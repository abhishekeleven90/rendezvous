#ifndef ONE_MORE_FILE_H
#define ONE_MORE_FILE_H

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

#endif
