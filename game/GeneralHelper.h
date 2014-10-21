#include "Headers.h"

void printGrid() {
	cout << "Printing grid" << "\n";
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			switch (gridChar[r][c]) {
			case GRASS:
				cout << " GRA";
				break;
			case SPAWN:
				cout << " SPA";
				break;
			case ATTRIBUTE_BG:
				cout << " ATT";
				break;
			case TEMPLE_ANGELS:
				cout << " TEa";
				break;
			case TEMPLE_DEMONS:
				cout << " TEd";
				break;
			case H_STUNNER:
				cout << " Hst";
				break;
			case H_SILENCER:
				cout << " Hsi";
				break;
			case H_SNATCHER:
				cout << " Hsn";
				break;
			case H_LEECHER:
				cout << " Hle";
				break;
			case STONE:
				cout << " STO";
				break;
			case TREE:
				cout << " TRE";
				break;
			case TREE_BACK:
				cout << " TRb";
				break;
			case T_ANGELS_BACK:
				cout << " Tab";
				break;
			case T_DEMONS_BACK:
				cout << " Tdb";
				break;
			case DEFAULT:
				cout << " DEF";
				break;
			default:
				cout << " DDD";
			}
		}
		cout << "\n";
	}

}
