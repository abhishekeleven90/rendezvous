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
