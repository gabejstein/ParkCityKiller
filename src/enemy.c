#include "enemy.h"
#include "playState.h"
#include "bullet.h"
#include "pickup.h"

#define MAX_ENEMY 100

static Enemy enemyPool[MAX_ENEMY];
static unsigned int curEnemy = 0;

static Entity* player = NULL;

//Enemy state functions. Should keep params the same in case they're used in a more elaborate state machine structure.
static void UpdateAttack(Entity* e, Enemy* enemy, float dt);
static void UpdatePursue(Entity* e, Enemy* enemy, float dt);
static void UpdateEscape(Entity* e, Enemy* enemy, float dt);
static void UpdateHurt(Entity* e, Enemy* enemy, float dt);
static void FaceObject(Entity* e,Vector3 target, float dt);

static float triggerDistance = 50.0f;
static float attackDistance = 30.0f;
static float loseInterestDistance = 80.0f;
static float attackWaitMin = 2.2f, attackWaitMax = 3.2f;
static float runSpeed = 30.0f;

static const char* dialogues[] =
{
	[0] = "Screw this! I'm out of here!",
	[1] = "Run away! Run away!",
	[2] = "I can't take it anymore!",
	[3] = "I'm outta here!",
	[4] = "I don't get paid enough for this."
};

void Enemy_SetPlayer(Entity* p)
{
	player = p;
}

static void UpdateEnemy(Entity* e, float dt)
{
	
	e->velocity.x = e->velocity.z = 0;

	Enemy* enemySelf = (Enemy*)e->data;

	switch (enemySelf->state)
	{
	case ENEMY_STATE_NEUTRAL:
		if (Vector3Distance(player->transform.position, e->transform.position) < triggerDistance)
		{
			enemySelf->waitTimer = 0.0f;
			enemySelf->state = ENEMY_STATE_PURSUE;
		}	
		break;

	case ENEMY_STATE_HURT:
		UpdateHurt(e, enemySelf, dt);
		break;

	case ENEMY_STATE_ATTACK:
		UpdateAttack(e,enemySelf, dt);
		break;

	case ENEMY_STATE_PURSUE:
		UpdatePursue(e, enemySelf, dt);
		break;

	case ENEMY_STATE_ESCAPE:
		UpdateEscape(e, enemySelf, dt);
		break;
	}
}

static void FaceObject(Entity* e,Vector3 target, float dt)
{
	Vector3 distVect = Vector3Subtract(target,e->transform.position);

	float targetAngle = atan2f(distVect.x, distVect.z) * RAD2DEG - e->transform.rotation.y;
	targetAngle = (int)(targetAngle + 180) % 360 - 180;

	e->transform.rotation.y += targetAngle * dt * 15;

}

static void UpdateHurt(Entity* e, Enemy* enemy, float dt)
{
	enemy->waitTimer -= dt;
	if (enemy->waitTimer <= 0.0f)
	{
		enemy->state = enemy->prevState;
		enemy->waitTimer = 0.4f; //add a bit of delay before doing anything.
	}
}

static void UpdateAttack(Entity* e, Enemy* enemy, float dt)
{
	FaceObject(e,player->transform.position, dt);

	float dist = Vector3DistanceSqr(player->transform.position, e->transform.position);
	if (dist >= attackDistance * attackDistance)
	{
		enemy->state = ENEMY_STATE_PURSUE;
		return;
	}

	enemy->waitTimer -= dt;
	if (enemy->waitTimer <= 0.0f)
	{
		//TODO: perhaps every entity should have a head position.
		Vector3 head = Vector3Add(e->transform.position, (Vector3) { 0, 1.0, 0 });
		Vector3 fireVel = Vector3Subtract(Vector3Add(player->transform.position, (Vector3) { 0, 1.0, 0 }), head);
		fireVel = Vector3Normalize(fireVel);
		fireVel = Vector3Scale(fireVel, 0.5f);
		SpawnBullet(head, fireVel, e->tag);

		enemy->waitTimer = GetRandomFloat(attackWaitMin,attackWaitMax);
	}
}

static void UpdatePursue(Entity* e, Enemy* enemy, float dt)
{
	FaceObject(e, player->transform.position, dt);
	float dist = Vector3DistanceSqr(player->transform.position, e->transform.position);
	if (dist <= attackDistance*attackDistance)
	{
		enemy->state = ENEMY_STATE_ATTACK;
		enemy->waitTimer = GetRandomFloat(attackWaitMin, attackWaitMax);
	}
	else
	{
		Vector3 playerVec = Vector3Subtract(player->transform.position, e->transform.position);
		Vector3 heading = Vector3Normalize(playerVec);
		e->velocity = Vector3Scale(heading, runSpeed);
		e->velocity = Vector3Scale(e->velocity, dt);
	}
}

static void UpdateEscape(Entity* e, Enemy* enemy, float dt)
{
	Vector3 escapeVel = Vector3Subtract(e->transform.position, player->transform.position);
	escapeVel = Vector3Normalize(escapeVel);
	escapeVel.y = 0; //we just want to run along x,z
	escapeVel = Vector3Scale(escapeVel, runSpeed*0.7);
	e->velocity = Vector3Scale(escapeVel, dt);

	FaceObject(e, Vector3Add(e->transform.position, escapeVel), dt);
}

static void RenderEnemy(Entity* e)
{
	Color color = BLUE;

	if (((Enemy*)e->data)->state == ENEMY_STATE_HURT)
		color = RED;
	else if (((Enemy*)e->data)->state == ENEMY_STATE_DEAD)
		color = YELLOW;

	Vector3 head =Vector3Add( e->transform.position,Vector3Scale(e->transform.forward,0.6f));
	head.y += 1.5;
	Vector3 end = (Vector3){ 0, 1.5, 0 };
	DrawCapsule(e->transform.position, Vector3Add(e->transform.position,end), 0.5, 6, 6, color);
	DrawSphere(head, 0.2f, RED);
}

static void EnemyOnCollision(Entity* self, Entity* other)
{
	if (other->tag == ET_BULLET)
	{
		BulletData* b = (BulletData*)other->data;
		if (b->shooter == ET_PLAYER)
		{
			Enemy* enemySelf = (Enemy*)self->data;
			if (enemySelf->state == ENEMY_STATE_HURT || enemySelf->state==ENEMY_STATE_DEAD)
				return;

			self->health--;
			if (self->health <= 0)
			{
				enemySelf->state = ENEMY_STATE_DEAD;
				self->collider.type = CT_NULL;

				if (GetRandomValue(1, 10) > 6)
				{
					float randX = GetRandomFloat(-4.0f, 4.0f);
					float randZ = GetRandomFloat(-4.0f, 4.0f);

					SpawnRandomPickup(Vector3Add(self->transform.position, (Vector3) { randX, 0, randZ }));
				}
				
			}
			else
			{
				enemySelf->prevState = enemySelf->state;
				enemySelf->state = ENEMY_STATE_HURT;
				enemySelf->waitTimer = 0.6f;

				if (self->health == 1)
				{
					int isCoward = GetRandomValue(1, 10) > 7; //TODO: make this value an enemy parameter later.
					if (isCoward)
					{
						enemySelf->state = ENEMY_STATE_ESCAPE;
						int dialogue = GetRandomValue(0, 4);
						PushMsgBox(dialogues[dialogue]);
					}
				}
			}
		}
	}

}

void NewEnemy(Entity* e)
{
	e->update = UpdateEnemy;
	e->render = RenderEnemy;
	e->onCollision = EnemyOnCollision;
	e->bFloat = 1;
	e->health = 3;
	e->mass = 2;
	e->collider.type = CT_SPHERE;
	e->collider.radius = 2.0f;
	e->tag = ET_ENEMY;

}

void InitEnemyPool(void)
{
	curEnemy = 0;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		Enemy* enemy = &enemyPool[i];
		enemy->entity = NewEntity();
		if (enemy->entity)
		{
			NewEnemy(enemy->entity);
			enemy->entity->bActive = 0;
			enemy->entity->data = enemy;
		}
		else
		{
			printf("Could not create enemy pool.\n");
			exit(0);
		}

		enemy->state = ENEMY_STATE_NEUTRAL;
	}
}

Enemy* SpawnEnemy(Vector3 pos)
{
	if (curEnemy >= MAX_ENEMY)
		return NULL;

	Enemy* enemy = &enemyPool[curEnemy];
	curEnemy = (curEnemy + 1) % MAX_ENEMY;

	enemy->entity->transform.position = pos;
	enemy->entity->bActive = 1;
	enemy->entity->collider.type = CT_SPHERE;

	return enemy;
}

