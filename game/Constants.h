#ifndef CONSTANTS_H
#define CONSTANTS_H

//-------------------------CONSTANTS------------------

#define ITEMS_ON_MAP_COUNT 3
#define CELL_LENGTH 0.2f

#define NUM_OF_PLAYERS 4
#define PLAYER_ID_PRIMARY 0

#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))
#define SPEED_ACTUAL(id) (SPEED_MAX-players[id].speedMove+1)

//Grid related
#define MAX_XCELL 1.0f
#define MAX_YCELL 2.0f

#define MIN_XCELL -2.4f
#define MIN_YCELL -2.0f

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

//Attributes
#define HEALTH_FULL_HERO 200
#define HEALTH_FULL_TEMPLE 500

#define STRENGTH_H_STUNNER 4 //DAMAGE
#define STRENGTH_H_DISABLER 6
#define STRENGTH_H_SLOWER 5
#define STRENGTH_H_BURSTER 7

#define CURSE_AMT_BURST_DAMAGE 30
#define CURSE_AMT_SLOW_STRENGTH 4

#define SPEED_MOVE_H_STUNNER 2
#define SPEED_MOVE_H_DISABLER 3
#define SPEED_MOVE_H_SLOWER 2
#define SPEED_MOVE_H_BURSTER 2

#define GAIN_ITEM_DAMAGE 2
#define GAIN_ITEM_HEALTH 20
#define GAIN_ITEM_SPEED_MOVE 1
#define GAIN_ITEM_TEMPLE_HEALER 50

#define SPEED_MAX 5
#define SPEED_MIN 0

//Number of Blocks
#define SPAWN_BLOCKS 3
#define ATTRIBUTE_WIDTH 2
#define TEMPLE_BLOCKS 2 //both on x & y direction
#define DIAG_BLOCKS 2 //numOfDiagonals = 2*DIAGONAL_BLOCKS+1
#endif
