#include "gameWorld.h"
#include "game.h"
#include "const.h"

extern Game gGame;

void PlayerStats_Init(void)
{
	gGame.playerStats.bullets = 50;
	gGame.playerStats.money = 500;
	gGame.playerStats.health = 10;
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