//-------------------------CONSTANTS------------------
#define CELL_LENGTH 0.2f
#define REFRESH_RATE 2

//Grid related
#define MAX_XCELL 1.0f
#define MAX_YCELL 2.0f

#define MIN_XCELL -2.4f
#define MIN_YCELL -2.0f

#define ATTRIBUTE_WIDTH 2

#define START_GRID_ROW 1
#define END_GRID_ROW 20

#define START_INNER_GRID_COL 1
#define END_INNER_GRID_COL 20

#define START_OUTER_GRID_COL START_INNER_GRID_COL //1
#define END_OUTER_GRID_COL END_INNER_GRID_COL+(2*ATTRIBUTE_WIDTH)

#define START_LEFT_ATTRIBUTE_COL START_OUTER_GRID_COL //1
#define END_LEFT_ATTRIBUTE_COL START_LEFT_ATTRIBUTE_COL+ATTRIBUTE_WIDTH-1

#define END_RIGHT_ATTRIBUTE_COL END_OUTER_GRID_COL
#define START_RIGHT_ATTRIBUTE_COL END_RIGHT_ATTRIBUTE_COL-ATTRIBUTE_WIDTH+1

//Hero attributes related
#define MAX_HEALTH 100

#define DEFAULT_STRENGTH 1
#define DEFAULT_PISTOL_AMMO 20

#define MAGIC_SPELL_TIMER 5 //Time between using two consecutive magic spells
#define MAGIC_IMPACT_TIMER 2

#define GLOBAL_ITEM_TIMER 3 //Time between taking two consecutive Items
#define ATTACK_VISIBILITY 1

//Spawn
#define SPAWN_BLOCKS 6

//Number of Blocks
#define TEMPLE_BLOCKS 2

//------------------------GLOBALS-------------------
//TextureIds
GLuint grass_texId;
GLuint spawn_texId;

GLuint attribute_bg_texId;

GLuint tree_texId;
GLuint stone_texId;

GLuint t_angels_texId; //temples
GLuint t_demons_texId;

GLuint h_stunner_texId; //hero_mario
GLuint h_silencer_texId;
GLuint h_snatcher_texId; //hero_soldier
GLuint h_leecher_texId;

