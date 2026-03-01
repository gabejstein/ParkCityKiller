#include "utils.h"
#include "..\const.h"
#include "..\game.h"
#include <raymath.h>

float GetRandomFloat(float min, float max) 
{
	int intMin = 0;
	int intMax = 10000; // Controls precision
	int r = GetRandomValue(intMin, intMax);
	float t = (float)r / (float)(intMax - intMin);
	return min + t * (max - min);
}

int CheckPointInBox(const Vector3* point, const BoundingBox* box)
{
	if (point->x > box->min.x && point->x < box->max.x &&
		point->y > box->min.y && point->y < box->max.y &&
		point->z > box->min.z && point->z < box->max.z)
		return 1;

	return 0;
}

//Note: I dont like all these dependencies here.
void DrawTextOverlay(const char* text, const Vector3 worldPos, int fontSize, Color color)
{
	if (Vector3DotProduct(Vector3Negate(gGame.mainCamera.transform.forward), Vector3Subtract(worldPos, gGame.mainCamera.transform.position)) < 0) return;

	Vector2 pos = GetWorldToScreenEx(worldPos, *gGame.mainCamera.camera, VIRTUAL_WINDOW_W, VIRTUAL_WINDOW_H);

	//TODO: Should probably account for text width/height as well, but whatever.
	//TODO: Should also account for camera distance and scale accordingly.
	if (pos.x >= 0 && pos.x <= VIRTUAL_WINDOW_W && pos.y >= 0 && pos.y <= VIRTUAL_WINDOW_H)
		DrawText(text, (int)pos.x, (int)pos.y, fontSize, color);
}

