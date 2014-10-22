#include "OpenGlHelper.h"

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

	i_movSpeed_texId = getTextureFromImage(PATH_IMG_ITEM_MOVEMENT_SPEED);
	i_attackSpeed_texId = getTextureFromImage(PATH_IMG_ITEM_ATTACK_SPEED);
	i_damage_texId = getTextureFromImage(PATH_IMG_ITEM_DAMAGE);
	i_health_texId = getTextureFromImage(PATH_IMG_ITEM_HEALTH);
}

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
			case WAR_GROUND:
				cout << " war";
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
