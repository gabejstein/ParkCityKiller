#include "resource.h"
#include <string.h>
#include <stdlib.h>
#include <rlgl.h>
#include <raymath.h>

#ifndef MAX_FILENAME
	#define MAX_FILENAME 256
#endif

#define MAX_SOUNDS 256
#define MAX_TEXTURES 256
#define MAX_MODELS 256

typedef struct
{
	char filePath[MAX_FILENAME];
	Sound sound;
	int nUsers; //probably won't use this, but it doesn't hurt to keep track of how many people are using things.
}Resource_Sound;

typedef struct
{
	char filePath[MAX_FILENAME];
	Texture texture;
	int nUsers;
}Resource_Texture;

typedef struct
{
	char filePath[MAX_FILENAME];
	Model model;
	int nUsers;
}Resource_Model;

typedef struct
{
	int curSound;
	int curTexture;
	int curModel;
	Resource_Sound sounds[MAX_SOUNDS];
	Resource_Texture textures[MAX_TEXTURES];
	Resource_Model models[MAX_MODELS];
}ResourceManager;

ResourceManager gResourceManager;

static Shader shader;
static unsigned int cameraDirLoc;
static unsigned int cameraPosLoc;
static unsigned int sunDirLoc;

static void RES_LoadShader(void);
static void UnloadSounds(void);
static void UnloadTextures(void);
static void UnloadModels(void);

static void SetupModelMaterial(Model* m);

void RES_Init(void)
{
	RES_LoadShader();
}


void RES_SetShaderValues(const Vector3 sunDir)
{
	SetShaderValue(shader, sunDirLoc, (float[3]) { sunDir.x, sunDir.y, sunDir.z }, SHADER_UNIFORM_VEC3);
}

void RES_UpdateShader(const Vector3* cameraPos, const Vector3 *cameraDir)
{
	SetShaderValue(shader, cameraDirLoc, (float[3]) { cameraDir->x, cameraDir->y, cameraDir->z }, SHADER_UNIFORM_VEC3);
	SetShaderValue(shader, cameraPosLoc, (float[3]) { cameraPos->x, cameraPos->y, cameraPos->z }, SHADER_UNIFORM_VEC3);

}

static void SetupModelMaterial(Model* m)
{
	for (int i = 0; i < m->materialCount; i++)
	{
		m->materials[i].shader = shader;
		SetTextureFilter(m->materials[i].maps[MATERIAL_MAP_DIFFUSE].texture, TEXTURE_FILTER_POINT);
	}
}

static void RES_LoadShader(void)
{
	shader = LoadShader("assets/shaders/g_dos_retro.vs", "assets/shaders/g_dos_retro.fs");
	cameraDirLoc = GetShaderLocation(shader, "cameraDir");
	cameraPosLoc = GetShaderLocation(shader, "camPos");
	sunDirLoc = GetShaderLocation(shader, "sunDir");
}

SoundHandle RES_LoadSound(const char* fileName)
{
	if (fileName == NULL)
		return -1;

	for (int i = 0; i < gResourceManager.curSound; i++)
	{
		if (strcmp(fileName, gResourceManager.sounds[i].filePath) == 0)
		{
			gResourceManager.sounds[i].nUsers++;
			return i;
		}
			
	}

	if (gResourceManager.curSound >= MAX_SOUNDS)
		return -1;

	int id = gResourceManager.curSound;
	memset(&gResourceManager.sounds[id], 0, sizeof(Resource_Sound));
	gResourceManager.sounds[id].sound = LoadSound(fileName);

	strncpy(gResourceManager.sounds[id].filePath , fileName, MAX_FILENAME);
	gResourceManager.sounds[id].filePath[MAX_FILENAME - 1] = '\0';
	
	gResourceManager.curSound++;
	return id;
	
}

void RES_PlaySound(const SoundHandle id)
{
	if (id < 0 || id >= gResourceManager.curSound)
	{
		//TODO: throw in an error message as well.
		return;
	}

	PlaySound(gResourceManager.sounds[id].sound);
}

TextureHandle RES_LoadTexture(const char* fileName)
{
	if (fileName == NULL)
		return NULL;

	for (int i = 0; i < gResourceManager.curTexture; i++)
	{
		if (strcmp(fileName, gResourceManager.textures[i].filePath) == 0)
		{
			gResourceManager.textures[i].nUsers++;
			printf("Texture Found in cache: %s\n", fileName);
			return &gResourceManager.textures[i].texture;
		}

	}

	if (gResourceManager.curTexture >= MAX_TEXTURES)
		return NULL;

	int id = gResourceManager.curTexture;
	memset(&gResourceManager.textures[id], 0, sizeof(Resource_Texture));
	gResourceManager.textures[id].texture = LoadTexture(fileName);
	//TODO: might want to make this optional for other projects.
	SetTextureFilter(gResourceManager.textures[id].texture, TEXTURE_FILTER_BILINEAR);

	strncpy(gResourceManager.textures[id].filePath, fileName, MAX_FILENAME);
	gResourceManager.textures[id].filePath[MAX_FILENAME - 1] = '\0';

	gResourceManager.curTexture++;
	return &gResourceManager.textures[id].texture;
	
}

void RES_DrawTexture(const TextureHandle handle, int x, int y)
{
	if (!handle)return;

	DrawTexture(*handle,x,y,WHITE);
}

void RES_DrawTexturePro(const TextureHandle handle,Rectangle src, Rectangle dest,Vector2 origin, float rot, Color color)
{
	if (!handle)return;

	DrawTexturePro(*handle, src, dest, origin, rot, color);
}

void RES_Unload(void)
{
	UnloadSounds();
	UnloadTextures();
	UnloadModels();
}

static void UnloadSounds(void)
{
	for (int i = 0; i < gResourceManager.curSound; i++)
	{
		printf("Unloading Sound: %s\n", gResourceManager.sounds[i].filePath);
		UnloadSound(gResourceManager.sounds[i].sound);
		gResourceManager.sounds[i].filePath[0] = '\0';
		gResourceManager.textures[i].nUsers = 0;
	}
	gResourceManager.curSound = 0;
}

static void UnloadTextures(void)
{
	for (int i = 0; i < gResourceManager.curTexture; i++)
	{
		printf("Unloading Texture: %s\n", gResourceManager.textures[i].filePath);
		UnloadTexture(gResourceManager.textures[i].texture);
		gResourceManager.textures[i].filePath[0] = '\0';
		gResourceManager.textures[i].nUsers = 0;
	}
	gResourceManager.curTexture = 0;
}

ModelHandle RES_LoadModel(const char* fileName)
{
	if (fileName == NULL)
		return -1;

	for (int i = 0; i < gResourceManager.curModel; i++)
	{
		if (strcmp(fileName, gResourceManager.models[i].filePath) == 0)
		{
			gResourceManager.models[i].nUsers++;
			printf("Model Found in cache: %s\n", fileName);
			return i;
		}
	}

	if (gResourceManager.curModel >= MAX_MODELS)
		return -1;

	int id = gResourceManager.curModel;
	memset(&gResourceManager.models[id], 0, sizeof(Resource_Model));
	gResourceManager.models[id].model = LoadModel(fileName);
	
	SetupModelMaterial(&gResourceManager.models[id].model);

	strncpy(gResourceManager.models[id].filePath, fileName, MAX_FILENAME);
	gResourceManager.models[id].filePath[MAX_FILENAME - 1] = '\0';

	gResourceManager.curModel++;
	return id;
}

static void UnloadModels(void)
{
	for (int i = 0; i < gResourceManager.curModel; i++)
	{
		printf("Unloading Model: %s\n", gResourceManager.models[i].filePath);
		UnloadModel(gResourceManager.models[i].model);
		gResourceManager.models[i].filePath[0] = '\0';
		gResourceManager.models[i].nUsers = 0;
	}
	gResourceManager.curModel = 0;
}

void RES_DrawModel(const ModelHandle m, Vector3 pos, float scale)
{
	if (m < 0 || m >= gResourceManager.curModel)
	{
		//TODO: throw in an error message as well.
		return;
	}

	DrawModel(gResourceManager.models[m].model, pos, scale, WHITE);

}

void RES_DrawModelEx(const ModelHandle m, Vector3 pos, Vector3 rotAxis,float angle, Vector3 scale)
{
	if (m < 0 || m >= gResourceManager.curModel)
	{
		//TODO: throw in an error message as well.
		return;
	}

	DrawModelEx(gResourceManager.models[m].model, pos, rotAxis,angle, scale, WHITE);
}

void RES_DrawModelWiresEx(const ModelHandle m, Vector3 pos, Vector3 rotAxis, float angle, Vector3 scale,Color color)
{
	if (m < 0 || m >= gResourceManager.curModel)
	{
		//TODO: throw in an error message as well.
		return;
	}

	DrawModelWiresEx(gResourceManager.models[m].model, pos, rotAxis, angle, scale, color);
}

void RES_UpdateModelAnimation(const ModelHandle m, ModelAnimation anim, int frame)
{
	if (m < 0 || m >= gResourceManager.curModel)
	{
		//TODO: throw in an error message as well.
		return;
	}
	UpdateModelAnimation(gResourceManager.models[m].model, anim, frame);
}

Model* RES_GetModel(const ModelHandle m)
{
	if (m < 0 || m >= gResourceManager.curModel)
	{
		//TODO: throw in an error message as well.
		return NULL;
	}

	return &gResourceManager.models[m].model;
}

Vector3 RES_GetModelCenter(const ModelHandle m)
{
	int vertCount = 0;
	Vector3 center = { 0 };
	Model* model = RES_GetModel(m);
	if (m)
	{
		for (int i = 0; i < model->meshCount; i++)
		{
			Mesh* mesh = &model->meshes[i];
			if (!mesh->vertices)continue;

			vertCount += mesh->vertexCount;
			Vector3* verts = (Vector3*)mesh->vertices;
			for (int j = 0; j < mesh->vertexCount; j++)
				center = Vector3Add(center, verts[j]);
		}

		if (vertCount != 0)
		{
			center.x /= vertCount;
			center.y /= vertCount;
			center.z /= vertCount;
		}
	}

	return center;
}