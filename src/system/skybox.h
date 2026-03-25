#ifndef SKYBOX_H
#define SKYBOX_H

#include <raylib.h>

void Skybox_Init(void);
void Skybox_Unload(void);
void Skybox_Update(Camera* camera, float dt);
void Skybox_Draw(void);

#endif
