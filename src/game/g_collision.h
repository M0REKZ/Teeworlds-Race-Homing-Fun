/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */
#ifndef GAME_MAPRES_COL_H
#define GAME_MAPRES_COL_H


#include <game/g_vmath.h>
int col_init();
int col_is_solid(int x, int y);
int col_is_damage(int x, int y);
int col_is_nohook(int x, int y);
int col_is_teleport(int x, int y);
int col_is_begin(int x, int y);
int col_is_end(int x, int y);
int col_width();
vec2 teleport(int z);
int col_height();
vec2 rand_point();
bool col_intersect_line(vec2 pos0, vec2 pos1, vec2 *out);
bool col_intersect_nohook(vec2 pos0, vec2 pos1);

#endif
