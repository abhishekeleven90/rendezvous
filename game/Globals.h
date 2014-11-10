#ifndef GLOBALS_H
#define GLOBALS_H

#include "Objects.h"
#include "Constants.h"
#include "CustomVectorStruct.h"

GameDetails gameDetails;

float windowWidth = 850, windowHeight = 700;

float cellWidth = windowWidth / (END_OUTER_GRID_COL - START_OUTER_GRID_COL + 1);
float cellHeight = windowHeight / (END_GRID_ROW - START_GRID_ROW + 1);

charCellType
		gridCharPrimary[END_GRID_ROW - START_GRID_ROW + 2][END_OUTER_GRID_COL
				- START_OUTER_GRID_COL + 2]; //+1 extra since starting from 1

charCellType gridChar[END_GRID_ROW - START_GRID_ROW + 2][END_OUTER_GRID_COL
		- START_OUTER_GRID_COL + 2]; //+1 extra since starting from 1

charCellType
		initialGridChar[END_GRID_ROW - START_GRID_ROW + 2][END_OUTER_GRID_COL
				- START_OUTER_GRID_COL + 2]; //+1 extra since starting from 1

int g_item_index_angels = 0; //holds the global index of next item to be placed
int g_item_index_demons = 0;

int currPlayerId;
TeamStruct angelsTeam, demonsTeam;
Player players[4];

//TextureIds
GLuint btn_start_texId;

GLuint texId_bg_grass;
GLuint texId_bg_spawn;
GLuint texId_bg_war;
GLuint texId_bg_blocked;

GLuint texId_bg_attribute;

GLuint texId_tree;
GLuint texId_stone;
GLuint texId_tree_war;
GLuint texId_stone_war;

GLuint texId_t_angels; //temples
GLuint texId_t_demons;

GLuint texId_h_stunner;
GLuint texId_h_disabler;
GLuint texId_h_slower;
GLuint texId_h_burster;

GLuint texId_i_speedMov;
GLuint texId_i_speedAttack;
GLuint texId_i_damage;
GLuint texId_i_health;
GLuint texId_i_tHealer;

GLuint temp_texId;

MyTexture texId_att_cBurst;
MyTexture texId_att_cDisable;
MyTexture texId_att_cSlow;
MyTexture texId_att_cStun;
MyTexture texId_att_cNone;

MyTexture texId_att_h_burster;
MyTexture texId_att_h_disabler;
MyTexture texId_att_h_slower;
MyTexture texId_att_h_stunner;

MyTexture texId_att_mBasic;
MyTexture texId_att_mMagic;
MyTexture texId_att_mStun;

MyTexture texId_att_team_angels;
MyTexture texId_att_team_demons;

MyTexture texId_att_team_enemy;
MyTexture texId_att_team_my;

MyTexture texId_att_timers;
MyTexture texId_att_time_itemOff;
MyTexture texId_att_time_itemOn;
MyTexture texId_att_time_magicOff;
MyTexture texId_att_time_magicOn;

MyTexture texId_att_health;
MyTexture texId_att_temple_health;
MyTexture texId_att_strength;
MyTexture texId_att_speed;

MyTexture texId_att_wall;

MyTexture texId_bg;
MyTexture texId_logo;
MyTexture texId_help;
MyTexture texId_btn_back;

MyTexture texId_join_game;
MyTexture texId_host_game;
MyTexture texId_white;
MyTexture texId_input;
MyTexture texId_btn_next;

MyTexture texId_pick_team;

MyTexture texId_player_single;
MyTexture texId_player_multi;

//window IDs
int windowId_current;

Coordinate_grid downGrid_click; //used for clickHandler

//locks
pthread_mutex_t mutexQueuePrimary;
pthread_mutex_t mutexTargetCell;

bool isGameOver=false;
teamName winningTeam;

#endif
