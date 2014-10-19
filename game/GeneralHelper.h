#include "Headers.h"

void printGrid() {
	cout << "Printing grid" << "\n";
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_GRID_COL; c <= END_GRID_COL; c++) {
			switch (gridChar[r][c]) {
			case GRASS:
				cout << " GR";
				break;
			case SPAWN:
				cout << " SP";
				break;
			case ANGELS_TEMPLE:
				cout << " TA";
				break;
			case DEMONS_TEMPLE:
				cout << " TD";
				break;
			case H_STUNNER:
				cout << " HX";
				break;
			case H_SILENCER:
				cout << " HS";
				break;
			case H_SNATCHER:
				cout << " HN";
				break;
			case H_LEECHER:
				cout << " HL";
				break;
			case STONE:
				cout << " ST";
				break;
			case TREE:
				cout << " TR";
				break;
			}
		}
		cout << "\n";
	}

}
