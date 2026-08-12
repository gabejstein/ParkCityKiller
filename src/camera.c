#include "camera.h"
#include <raylib.h>
#include <raymath.h>
#include "game.h"
#include "const.h"

static ModelHandle levelCollider;
static Entity* player;

//TODO set camera to behind player back on level load, etc
void InitGameCamera(Entity* _player, ModelHandle _levelCollider)
{
	player = _player;
	levelCollider = _levelCollider;
}

static RayCollision HandleCameraCollisions(Vector3 target)
{
	Model* level = RES_GetModel(levelCollider);

	//TODO: remember that we want to set target position (not position) later.
	Ray ray;
	ray.position = player->transform.position;
	ray.direction = Vector3Normalize(Vector3Subtract(gGame.mainCamera.camera->position, target));
	RayCollision hit = { 0 };
	hit.distance = INFINITY;

	//Check ground
	for (int i = 0; i < level->meshCount; i++)
	{
		RayCollision newHit = GetRayCollisionMesh(ray, level->meshes[i], level->transform);
		if (newHit.hit && newHit.distance < 10.0f && newHit.distance < hit.distance)
		{
			hit = newHit;
		}
	}

	return hit;
}

static void UpdateCameraTransform(void)
{
	GameCamera* cam = &gGame.mainCamera;
	//calculate camera forward
	cam->transform.forward.x = cosf(DEG2RAD * cam->transform.rotation.y) * cosf(DEG2RAD * cam->transform.rotation.x);
	cam->transform.forward.y = sinf(DEG2RAD * cam->transform.rotation.x);
	cam->transform.forward.z = sinf(DEG2RAD * cam->transform.rotation.y) * cosf(DEG2RAD * cam->transform.rotation.x);
}

static void UpdatePlayerCamera(float dt)
{
	//INPUT
	float mouseSensitivity = 100.0f;
	float minPitch = -20.0f;
	float maxPitch = 89.0f;
	GameCamera* cam = &gGame.mainCamera;
	int gamepad = 0;
	float horAxis = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_RIGHT_X);
	float vertAxis = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_RIGHT_Y);
	cam->transform.rotation.y += horAxis * mouseSensitivity * dt; //yaw
	cam->transform.rotation.x += vertAxis * mouseSensitivity * dt; //pitch
	cam->transform.rotation.x = Clamp(cam->transform.rotation.x, minPitch, maxPitch);

	UpdateCameraTransform();

	//TODO: have player's speed affect camera distance as well.
	float followDist = 10.0f;
	float minDist = 1.5f;

	//we don't really want the camera placed right at the player, since that would be at his feet.
	Vector3 playerTarget = Vector3Add(player->transform.position, (Vector3) { 0, 1.2f, 0.0f });
	Vector3 lookOffset = (Vector3){ 0,1.2,0 };

	RayCollision wallHit = HandleCameraCollisions(playerTarget);
	if (wallHit.hit && wallHit.distance < followDist) //we only want collisions to affect things if they're closer, not further than the default
	{
		followDist = wallHit.distance;
	}

	followDist = MAX(minDist, followDist);

	Vector3 scaledForward = Vector3Scale(cam->transform.forward, followDist);

	cam->targetPos = Vector3Add(playerTarget, scaledForward);
	cam->newLookTarget = Vector3Add(player->transform.position, lookOffset);
}

static void UpdateDeathCamera(float dt)
{
	UpdateCamera(gGame.mainCamera.camera, CAMERA_ORBITAL);
	gGame.mainCamera.camera->target = player->transform.position;

}

static void UpdateHoverbikeCamera(float dt)
{
	GameCamera* cam = &gGame.mainCamera;
	float followDist = 10.0f;
	float minDist = 1.5f;

	//we don't really want the camera placed right at the player, since that would be at his feet.
	Vector3 playerTarget = Vector3Add(player->transform.position, (Vector3) { 0, 1.2f, 0.0f });
	Vector3 lookOffset = (Vector3){ 0,1.2,0 };

	UpdateCameraTransform();

	//Hoverboard stuff
	Vector3 distance = (Vector3){ 0,2.4f,6.0f };
	Vector3 behindPlayer = Vector3Scale(player->transform.forward, -distance.z);
	behindPlayer.y += distance.y;

	behindPlayer = Vector3Add(player->transform.position, behindPlayer);
	gGame.mainCamera.targetPos = behindPlayer;

	Vector3 lookTarget = Vector3Scale(player->transform.forward, 2.0f);
	lookTarget.y += distance.y;
	lookTarget = Vector3Add(player->transform.position, lookTarget);

	gGame.mainCamera.camera->target = lookTarget;

	cam->newLookTarget = Vector3Add(player->transform.position, lookOffset);

}

static void UpdateAimCamera(float dt)
{
	//INPUT
	float mouseSensitivity = 100.0f;
	float minPitch = -20.0f;
	float maxPitch = 89.0f;
	GameCamera* cam = &gGame.mainCamera;
	int gamepad = 0;
	float horAxis = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_RIGHT_X);
	float vertAxis = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_RIGHT_Y);
	cam->transform.rotation.y += horAxis * mouseSensitivity * dt; //yaw
	cam->transform.rotation.x += vertAxis * mouseSensitivity * dt; //pitch
	cam->transform.rotation.x = Clamp(cam->transform.rotation.x, minPitch, maxPitch);

	UpdateCameraTransform();

	//TODO: have player's speed affect camera distance as well.
	float followDist = 10.0f;
	float minDist = 1.5f;

	//we don't really want the camera placed right at the player, since that would be at his feet.
	Vector3 playerTarget = Vector3Add(player->transform.position, (Vector3) { 0, 1.2f, 0.0f });
	Vector3 lookOffset = (Vector3){ 0,1.2,0 };

	//Stuff specific to aiming
	playerTarget.y = player->transform.position.y + 1.3f;
	lookOffset = Vector3Add(lookOffset, Vector3Scale(player->transform.right, -0.3f));

	followDist = 2.0f;
	Vector3 scaledForward = Vector3Scale(cam->transform.forward, followDist);

	cam->targetPos = Vector3Add(playerTarget, scaledForward);

	cam->newLookTarget = Vector3Add(player->transform.position, lookOffset);

}

void UpdateGameCamera(float dt)
{
	if (gGame.playerStats.playerState == PLAYER_STATE_MOVE && gGame.mainCamera.state!=CAMERA_STATE_CUTSCENE)
		gGame.mainCamera.state = CAMERA_STATE_PLAYER;
	else if (gGame.playerStats.playerState == PLAYER_STATE_AIM || gGame.playerStats.playerState == PLAYER_STATE_SHOOT)
		gGame.mainCamera.state = CAMERA_STATE_AIM;
	else if (gGame.playerStats.playerState == PLAYER_STATE_HOVERBIKE)
		gGame.mainCamera.state = CAMERA_STATE_VEHICLE;
	else if (gGame.playerStats.playerState == PLAYER_STATE_DEAD)
		gGame.mainCamera.state = CAMERA_STATE_DEATH;

	switch (gGame.mainCamera.state)
	{
	case CAMERA_STATE_PLAYER:
		UpdatePlayerCamera(dt);
		break;
	case CAMERA_STATE_AIM:
		UpdateAimCamera(dt);
		break;
	case CAMERA_STATE_DEATH:
		UpdateDeathCamera(dt);
		break;
	case CAMERA_STATE_VEHICLE:
		UpdateHoverbikeCamera(dt);
		break;
	case CAMERA_STATE_CUTSCENE:
		break;
	}

	//Shared
	float smooth = MIN(1, 12.0 * dt);
	//If I dont smooth the target too then the camera follows the player too tightly.
	//If the player jitters too much, the camera jitters too much.
	gGame.mainCamera.camera->target = Vector3Lerp(gGame.mainCamera.camera->target, gGame.mainCamera.newLookTarget, smooth);

	smooth = MIN(1, 8.0 * dt);
	gGame.mainCamera.camera->position = Vector3Lerp(gGame.mainCamera.camera->position, gGame.mainCamera.targetPos, smooth);
	gGame.mainCamera.transform.position = gGame.mainCamera.camera->position; //This is not very elegant. Should fix.
}