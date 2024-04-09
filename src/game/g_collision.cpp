/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */
#include <engine/e_system.h>
#include <game/g_vmath.h>
#include <math.h>
#include <engine/e_common_interface.h>
#include <game/g_math.h>
#include <game/g_mapitems.h>
#include <game/g_layers.h>

static TILE *tiles;
static int *dest[10];
static int width = 0;
static int height = 0;
static int len[10];

int col_width() { return width; }
int col_height() { return height; }

int col_init()
{
	width = layers_game_layer()->width;
	height = layers_game_layer()->height;
	tiles = (TILE *)map_get_data(layers_game_layer()->data);
	mem_zero(&len, sizeof(len));
	for(int i=width*height-1;i>=0;i--) {
		if(tiles[i].index==TILE_AIR) {
			len[0]++;
		} else if((tiles[i].index&1)!=0 && tiles[i].index>2 && tiles[i].index<21) {
			len[tiles[i].index>>1]++;
		}
	}
	for(int i=0;i<10;i++) {
		dest[i] = new int[len[i]];
		len[i] = 0;
	}
	for(int i=width*height-1;i>=0;i--) {
		if(tiles[i].index==TILE_AIR) {
			dest[0][len[0]++] = i;
		} else if((tiles[i].index&1)!=0 && tiles[i].index>2 && tiles[i].index<21) {
			dest[tiles[i].index>>1][len[tiles[i].index>>1]++] = i;
		}
	}
	return 1;
}


int col_is_solid(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(nx < 0 || nx >= width || ny >= height)
		return 1;
	
	if(y<0) {
		return(tiles[nx].index==TILE_SOLID);
	}
	
	return tiles[ny*width+nx].index == TILE_SOLID;
}

int col_is_damage(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	
	return tiles[ny*width+nx].index == TILE_DAMAGE;
}

int col_is_nohook(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(nx < 0 || nx >= width || ny >= height)
		return 0;

	if(y<0) {
		return(tiles[nx].index==TILE_NOHOOK);
	}
	
	return tiles[ny*width+nx].index == TILE_NOHOOK;
}

int col_is_teleport(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y < 0 || nx < 0 || nx >= width || ny >= height)
		return 0;

	int z = tiles[ny*width+nx].index-1;
	if(z>2 && z<21 && (z&1)!=0) {
		return(z>>1);
	}
	return(0);
}
int col_is_begin(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	
	return tiles[ny*width+nx].index == TILE_BEGIN;
}
int col_is_end(int x, int y)
{
	int nx = x/32;
	int ny = y/32;
	if(y<0 || nx < 0 || nx >= width || ny >= height)
		return 0;
	
	return tiles[ny*width+nx].index == TILE_END;
}
vec2 teleport(int a) {
	if(len[a]>0) {
		int r = rand()%len[a];
		int x = (dest[a][r]%width)<<5;
		int y = (dest[a][r]/width)<<5;
		return(vec2((float)x+16.0,(float)y+16.0));
	} else {
		return(vec2(0,0));
	}
}

vec2 rand_point() {
	int p = rand()%len[0];
	int x = (dest[0][p]%width)<<5;
	int y = (dest[0][p]/width)<<5;
	return(vec2((float)x+16.0,(float)y+16.0));
}

// TODO: rewrite this smarter!
bool col_intersect_line(vec2 pos0, vec2 pos1, vec2 *out)
{
	float d = distance(pos0, pos1);
	
	for(float f = 0; f < d; f++)
	{
		float a = f/d;
		vec2 pos = mix(pos0, pos1, a);
		if(col_is_solid((int)pos.x, (int)pos.y))
		{
			if(out)
				*out = pos;
			return true;
		}
	}
	if(out)
		*out = pos1;
	return false;
}

bool col_intersect_nohook(vec2 pos0, vec2 pos1)
{
	float d = distance(pos0, pos1);
	
	for(float f = 0; f < d; f++)
	{
		float a = f/d;
		vec2 pos = mix(pos0, pos1, a);
		if(col_is_nohook((int)pos.x, (int)pos.y)) {
			return true;
		}
	}
	return false;
}

/*
	Simple collision rutines!
*/
/*
struct collision
{
	int w, h;
	unsigned char *data;
};

static collision col;
static int global_dividor;

int col_width()
{
	return col.w;
}

int col_height()
{
	return col.h;	
}

int col_init(int dividor)
{
	mapres_collision *c = (mapres_collision*)map_find_item(MAPRES_COLLISIONMAP,0);
	if(!c)
	{
		dbg_msg("mapres_col", "failed!");
		return 0;
	}
	col.w = c->width;
	col.h = c->height;
	global_dividor = dividor;
	col.data = (unsigned char *)map_get_data(c->data_index);
	return col.data ? 1 : 0;
}

*/
