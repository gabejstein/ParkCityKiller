#ifndef UTILS_H
#define UTILS_H

#include <raylib.h>

#define SET_FLAG(value,flag) (value) |= (flag)
#define HAS_FLAG(value,flag) (((value) & (flag))!=0)
#define CLEAR_FLAG(value,flag) (value) &= ~(flag)

float GetRandomFloat(float min, float max);
int CheckPointInBox(const Vector3* point, const BoundingBox* box);
void DrawTextOverlay(const char* text, const Vector3 worldPos, int fontSize, Color color);

#endif
