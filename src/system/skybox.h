#ifndef SKYBOX_H
#define SKYBOX_H

#include <raylib.h>

void InitSkybox(void);
void UnloadSkybox(void);
void UpdateSkybox(Camera* camera, float dt);
void DrawSkyBox(void);

#endif
