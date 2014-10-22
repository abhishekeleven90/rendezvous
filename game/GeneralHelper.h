#include "OpenGlHelper.h"
#include "Headers.h"

void loadTextures() {
	grass_texId = getTextureFromImage(PATH_IMG_BG);
	spawn_texId = getTextureFromImage(PATH_IMG_SPAWN);
	war_texId = getTextureFromImage(PATH_IMG_WAR_GND);

	attribute_bg_texId = getTextureFromImage(PATH_IMG_ATTRIBUTE_BG);

	tree_texId = getTextureFromImage(PATH_IMG_TREE);
	stone_texId = getTextureFromImage(PATH_IMG_STONE);

	t_demons_texId = getTextureFromImage(PATH_IMG_TEMPLE_DEMONS);

	h_slower_texId = getTextureFromImage(PATH_IMG_HERO_MARIO);
	h_burster_texId = getTextureFromImage(PATH_IMG_HERO_SOLDIER);

	i_speedMov_texId = getTextureFromImage(PATH_IMG_ITEM_MOVEMENT_SPEED);
	i_speedAttack_texId = getTextureFromImage(PATH_IMG_ITEM_ATTACK_SPEED);
	i_damage_texId = getTextureFromImage(PATH_IMG_ITEM_DAMAGE);
	i_health_texId = getTextureFromImage(PATH_IMG_ITEM_HEALTH);
	i_templeHealer_texId = getTextureFromImage(PATH_IMG_ITEM_TEMPLE_HEALER);
}

Coordinate_grid getRandomCoordinatesForItem() {
	int randomRow;
	int randomCol;

	while (true) {
		randomRow = (rand() % (END_GRID_ROW - START_GRID_ROW + 1)) + 1; //1 extra since we want it to start from 1
		randomCol = (rand() % (END_INNER_GRID_COL - START_INNER_GRID_COL + 1))
				+ 1;

		if (gridChar[randomRow][randomCol + ATTRIBUTE_WIDTH] == BG_GRASS) { //assuming that items can come only on 'grass'
			break;
		}
	}
	return Coordinate_grid(randomRow, randomCol);
}

void placeItemAtRandomPos() {
	Coordinate_grid grid = getRandomCoordinatesForItem();
	putCharToGrid(grid.row, grid.col, itemCharCell[global_item_index], true);
	putCharToGrid(grid.col, grid.row, itemCharCell[global_item_index], true);
	global_item_index = (global_item_index + 1) % ARRAY_SIZE(itemCharCell);
}

void printGrid() {
	cout << "Printing grid" << "\n";
	for (int r = START_GRID_ROW; r <= END_GRID_ROW; r++) {
		for (int c = START_OUTER_GRID_COL; c <= END_OUTER_GRID_COL; c++) {
			switch (gridChar[r][c]) {
			case BG_GRASS:
				cout << " GRA";
				break;
			case BG_SPAWN:
				cout << " SPA";
				break;
			case BG_WAR:
				cout << " war";
				break;
			case BG_ATTRIBUTE:
				cout << " ATT";
				break;
			case TEMPLE_ANGELS:
				cout << " TEa";
				break;
			case TEMPLE_DEMONS:
				cout << " TEd";
				break;
			case H_DISABLER:
				cout << " Hdi";
				break;
			case H_SLOWER:
				cout << " Hsl";
				break;
			case H_BURSTER:
				cout << " Hbu";
				break;
			case H_STUNNER:
				cout << " Hst";
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
