#ifndef SKYBOX_H
#define SKYBOX_H

#include <raylib.h>

typedef struct
{
	char* texturePath;
	Model box;
}Skybox;

void Skybox_CommonInit(void);
void Skybox_CommonUnload(void);
void Skybox_Init(Skybox* skybox);
void Skybox_Unload(Skybox* skybox);
void Skybox_Update(Camera* camera, float dt);
void Skybox_Draw(Skybox* skybox);
void Skybox_SetRotSpeed(float speed);

#endif
