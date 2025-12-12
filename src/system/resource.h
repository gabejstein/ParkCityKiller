#ifndef RESOURCE_H
#define RESOURCE_H

#include <raylib.h>

typedef int SoundHandle;
typedef Texture* TextureHandle;
typedef int ModelHandle;

void RES_Init(void);
void RES_UpdateShader(const Vector3* cameraPos, const Vector3* cameraDir);
SoundHandle RES_LoadSound(const char* fileName);
void RES_PlaySound(const SoundHandle id);
void RES_Unload(void);

TextureHandle RES_LoadTexture(const char* fileName);
void RES_DrawTexture(const TextureHandle handle, int x, int y);
void RES_DrawTexturePro(const TextureHandle handle, Rectangle src, Rectangle dest, Vector2 origin, float rot, Color color);

ModelHandle RES_LoadModel(const char* fileName);
void RES_DrawModel(const ModelHandle m, Vector3 pos, float scale);
void RES_DrawModelEx(const ModelHandle m, Vector3 pos, Vector3 rotAxis, float angle, Vector3 scale);
void RES_DrawModelWiresEx(const ModelHandle m, Vector3 pos, Vector3 rotAxis, float angle, Vector3 scale, Color color);
Model* RES_GetModel(const ModelHandle m);

void RES_UpdateModelAnimation(const ModelHandle m, ModelAnimation anim, int frame);

#endif
