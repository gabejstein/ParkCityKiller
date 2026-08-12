#include "gameWorld.h"
#include "game.h"
#include "const.h"
#include "entity/player.h"

extern Game gGame;

void PlayerStats_Init(void)
{
	gGame.playerStats.bullets = 50;
	gGame.playerStats.money = 500;
	gGame.playerStats.health = 10;

	gGame.playerStats.playerState = PLAYER_STATE_MOVE;
}

void PlayerStats_AddHealth(int amount)
{
	gGame.playerStats.health = MAX(0,MIN(gGame.playerStats.health + amount, gGame.playerStats.maxHealth));
}

void PlayerState_AddMoney(int amount)
{
	gGame.playerStats.money = gGame.playerStats.money + amount;
}

void PlayerStats_AddBullets(int amount)
{
	gGame.playerStats.health = MAX(0, MIN(gGame.playerStats.health + amount, gGame.playerStats.maxHealth));
}

void ToggleHoverBike(void)
{
	//gGame.playerStats.playerState = (gGame.playerStats.playerState == PLAYER_STATE_MOVE ? PLAYER_STATE_HOVERBIKE : PLAYER_STATE_MOVE);
	if (gGame.playerStats.playerState == PLAYER_STATE_MOVE)
		gGame.playerStats.playerState = PLAYER_STATE_HOVERBIKE;
	else if (gGame.playerStats.playerState == PLAYER_STATE_HOVERBIKE)
		gGame.playerStats.playerState = PLAYER_STATE_MOVE;

	if (gGame.curLevel)
	{
		if (gGame.playerStats.playerState == PLAYER_STATE_MOVE)
		{
			Vector3 position = gGame.curLevel->player->transform.position;
			//gGame.curLevel->player = NewEntity();
			Player_New(gGame.curLevel->player, position);

		}
		else if (gGame.playerStats.playerState == PLAYER_STATE_HOVERBIKE)
		{
			Vector3 position = gGame.curLevel->player->transform.position;
			//gGame.curLevel->player = NewEntity();
			//Hoverbike_New(gGame.curLevel->player, position);
		}
	}
	
}