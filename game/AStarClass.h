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

	//TODO: temple work
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
