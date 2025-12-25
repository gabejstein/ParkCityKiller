#include "utils.h"

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

