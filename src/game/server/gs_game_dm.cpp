/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */
#include <engine/e_config.h>
#include "gs_common.h"
#include "gs_game_dm.h"

void gameobject_dm::tick()
{
	do_player_score_wincheck();
	gameobject::tick();
}
