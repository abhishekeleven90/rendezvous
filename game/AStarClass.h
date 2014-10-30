#ifndef A_STAR_CLASS
#define A_STAR_CLASS

#include <stdio.h>
#include <iostream>
#include <queue>
#include "Node.h"
#include "CustomVectorStruct.h"

//TODO uncomment
//#include "ConstantsNGlobals.h"

#define MOVEMENT_COST 1
//TODO remove
#define END_GRID_ROW 20

using namespace std;

class AStarClass {
	//TODO remove
	const int N = END_GRID_ROW;

	Node* gridAStar[END_GRID_ROW + 1][END_GRID_ROW + 1];
	Node* source = NULL;
	Node* target = NULL;
	priority_queue<Node*, vector<Node*> , NodeCompare> openList;

	//TODO: the target could be grass or spawn or the target could be an item
	//or the target could be temple, walk through!!
	//change would be in constructPath accordingly
public:

	void setTarget(int r, int c) {
		target = gridAStar[r][c];
	}

	void setSource(int r, int c) {
		source = gridAStar[r][c];
		source->gval = 0;
		source->parent = NULL;
		source->fval = (source->gval + heurisitic(source, target));
		//TODO may be fatal
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
		//TODO: to be filled with map details
		for (int i = 1; i <= N; i++) {
			for (int j = 1; j <= N; j++) {
				gridAStar[i][j] = newNode(i, j);
			}
		}
	}

	//AStar init
	void initAStar(Coordinate_grid source, Coordinate_grid target) {
		for (int i = 1; i <= N; i++) {
			for (int j = 1; j <= N; j++) {
				gridAStar[i][j]->fval = INT_MAX;
				gridAStar[i][j]->gval = INT_MAX;
				gridAStar[i][j]->parent = NULL;
				gridAStar[i][j]->alreadyVisited = false;
				gridAStar[i][j]->onPath = false;
				gridAStar[i][j]->onOpenList = false;

				//moving thsi code to OpenGlHelper
				/*if (isBlockedSite(i, j))
				 gridAStar[i][j]->blockstatus = true;
				 else
				 gridAStar[i][j]->blockstatus = false;//open*/
			}
		}
		setTarget(target.row, target.col - 2);//TODO:unable to find constant
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
		//TODO:change for walk through
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
		if (row < 1 || row > N)
			return NULL;
		if (col < 1 || col > N)
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
