#ifndef GAME_WORLD_H
#define GAME_WORLD_H

typedef enum
{
	PLAYER_STATE_MOVE,
	PLAYER_STATE_AIM,
	PLAYER_STATE_HURT,
	PLAYER_STATE_DEAD,
	PLAYER_STATE_HOVERBIKE,
	PLAYER_STATE_ATTACK,
	PLAYER_STATE_SHOOT,
}PLAYER_STATE;

typedef struct
{
    unsigned int bullets, maxBullets;
    int money;
    int health, maxHealth;
    int exp, expLevel, expNextLevel;
    int popularity; //a kind of reputation system for getting into clubs and better jobs and stuff.
	PLAYER_STATE playerState;
}PlayerStats;

void PlayerStats_Init(void);
void ToggleHoverBike(void);


#endif