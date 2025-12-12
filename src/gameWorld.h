#ifndef GAME_WORLD_H
#define GAME_WORLD_H

typedef struct
{
    unsigned int bullets, maxBullets;
    int money;
    int health, maxHealth;
    int exp, expLevel, expNextLevel;
    int popularity; //a kind of reputation system for getting into clubs and better jobs and stuff.
}PlayerStats;

#endif
