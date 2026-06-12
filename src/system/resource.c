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

typedef struct Resource_Sound
{
	char filePath[MAX_FILENAME];
	Sound sound;
	int nUsers;
	struct Resource_Sound* next;
	int index;
}Resource_Sound;

typedef struct Resource_Texture
{
	char filePath[MAX_FILENAME];
	Texture texture;
	int nUsers;
	struct Resource_Texture* next;
	int index;
}Resource_Texture;

typedef struct Resource_Model
{
	char filePath[MAX_FILENAME];
	Model model;
	int nUsers;
	struct Resource_Model* next;
	int index;
}Resource_Model;

typedef struct
{
	Resource_Sound sounds[MAX_SOUNDS];
	Resource_Sound* soundFreeList;
	Resource_Sound* soundUsedList;

	Resource_Texture textures[MAX_TEXTURES];
	Resource_Texture* textureFreeList;
	Resource_Texture* textureUsedList;

	Resource_Model models[MAX_MODELS];
	Resource_Model* modelFreeList;
	Resource_Model* modelUsedList;
}ResourceManager;

static ResourceManager gResourceManager;

static Shader shader;
static unsigned int cameraDirLoc;
static unsigned int cameraPosLoc;
static unsigned int fogDensityLoc;
static unsigned int fogColorLoc;
static unsigned int sunDirLoc;
static unsigned int lightColorLoc;

static void RES_LoadShader(void);
static void UnloadSounds(void);
static void UnloadTextures(void);
static void UnloadModels(void);

static void SetupModelMaterial(Model* m);

void RES_Init(void)
{
	unsigned int i;
	RES_LoadShader();

	//Init model free-list
	for (i = 0; i < MAX_MODELS - 1; i++)
	{
		gResourceManager.models[i].next = &gResourceManager.models[i + 1];
		gResourceManager.models[i].index = i;
	}
	gResourceManager.models[i].next = NULL;
	gResourceManager.models[i].index = i;

	gResourceManager.modelFreeList = gResourceManager.models;
	gResourceManager.modelUsedList = NULL;

	//Init texture free-list
	for (i = 0; i < MAX_TEXTURES - 1; i++)
	{
		gResourceManager.textures[i].next = &gResourceManager.textures[i + 1];
		gResourceManager.textures[i].index = i;
	}
	gResourceManager.textures[i].next = NULL;
	gResourceManager.textures[i].index = i;

	gResourceManager.textureFreeList = gResourceManager.textures;
	gResourceManager.textureUsedList = NULL;

	//Init sounds free-list
	for (i = 0; i < MAX_SOUNDS - 1; i++)
	{
		gResourceManager.sounds[i].next = &gResourceManager.sounds[i + 1];
		gResourceManager.sounds[i].index = i;
	}
	gResourceManager.sounds[i].next = NULL;
	gResourceManager.sounds[i].index = i;

	gResourceManager.soundFreeList = gResourceManager.sounds;
	gResourceManager.soundUsedList = NULL;
}

//Delete this. Just testing something with it
Shader RES_GetShader(void) { return shader; }

void RES_SetShaderValues(const Vector3 sunDir)
{
	SetShaderValue(shader, sunDirLoc, (float[3]) { sunDir.x, sunDir.y, sunDir.z }, SHADER_UNIFORM_VEC3);
}

void RES_UpdateShader(const Vector3* cameraPos, const Vector3* cameraDir)
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
		return NULL_SOUND;

	for (Resource_Sound* cur = gResourceManager.soundUsedList; cur != NULL; cur = cur->next)
	{
		if (strncmp(fileName, cur->filePath, MAX_FILENAME) == 0)
		{
			cur->nUsers++;
			printf("RES: Sound Found in cache: %s\n", fileName);
			return cur->index;
		}
	}

	Resource_Sound* r = gResourceManager.soundFreeList;
	if (r)
	{
		gResourceManager.soundFreeList = gResourceManager.soundFreeList->next;
		r->next = gResourceManager.soundUsedList;
		gResourceManager.soundUsedList = r;

		r->sound = LoadSound(fileName);
		strncpy(r->filePath, fileName, MAX_FILENAME);
		r->filePath[MAX_FILENAME - 1] = '\0';
		r->nUsers++;
		return r->index;
	}
	else
		printf("RES ERROR: Sound load unsuccessful: Out of sound resources.\n");

	return NULL_SOUND;
}

inline bool RES_IsValidSoundHandle(const SoundHandle id) { return (id > 0 && id < MAX_SOUNDS && gResourceManager.sounds[id].nUsers); }

void RES_PlaySound(const SoundHandle id)
{
	if (RES_IsValidSoundHandle(id))
		PlaySound(gResourceManager.sounds[id].sound);
}

//For unloading individual sounds at runtime.
void RES_UnloadSound(const SoundHandle s)
{
	if (!RES_IsValidSoundHandle(s))
	{
		printf("Error: Trying to unload with invalid sound handle.\n");
		return;
	}

	Resource_Sound* r = &gResourceManager.sounds[s];

	Resource_Model* cur, * prev = NULL;
	for (cur = gResourceManager.soundUsedList; cur != NULL; cur = cur->next)
	{
		if (cur->index == r->index)
		{
			if (prev)
				prev->next = cur->next;
			else
				gResourceManager.soundUsedList = cur->next;

			break;
		}
		prev = cur;
	}

	if (cur)
	{
		printf("RES: Unloading sound: %s\n", r->filePath);
		UnloadSound(r->sound);
		r->nUsers--;
		r->filePath[0] = '\0';
		r->next = gResourceManager.soundFreeList;
		gResourceManager.soundFreeList = r;
	}
	else
	{
		printf("RES Error: Could not find sound in used list\n");
	}

}

TextureHandle RES_LoadTexture(const char* fileName)
{
	if (fileName == NULL)
		return NULL_TEXTURE;

	for (Resource_Texture* cur = gResourceManager.textureUsedList; cur != NULL; cur = cur->next)
	{
		if (strncmp(fileName, cur->filePath, MAX_FILENAME) == 0)
		{
			cur->nUsers++;
			printf("RES: Texture Found in cache: %s\n", fileName);
			return &cur->texture;
		}
	}

	Resource_Texture* rm = gResourceManager.textureFreeList;
	if (rm)
	{
		gResourceManager.textureFreeList = gResourceManager.textureFreeList->next;
		rm->next = gResourceManager.textureUsedList;
		gResourceManager.textureUsedList = rm;

		rm->texture = LoadTexture(fileName);
		strncpy(rm->filePath, fileName, MAX_FILENAME);
		rm->filePath[MAX_FILENAME - 1] = '\0';
		SetTextureFilter(rm->texture, TEXTURE_FILTER_BILINEAR); //TODO: might want to make this optional for other projects.
		rm->nUsers++;
		return &rm->texture;
	}
	else
		printf("RES Error: Texture load unsuccessful: Out of texture resources.\n");

	return NULL_TEXTURE;
}

inline bool RES_IsValidTextureHandle(const TextureHandle t) { return (t != NULL_TEXTURE); }

//TODO: cannot unload individual textures without the index being in the handler.

void RES_DrawTexture(const TextureHandle handle, int x, int y)
{
	if (RES_IsValidTextureHandle(handle))
		DrawTexture(*handle, x, y, WHITE);
}

void RES_DrawTexturePro(const TextureHandle handle, Rectangle src, Rectangle dest, Vector2 origin, float rot, Color color)
{
	if (RES_IsValidTextureHandle(handle))
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
	for (Resource_Sound* cur = gResourceManager.soundUsedList; cur != NULL; cur = cur->next)
	{
		printf("RES: Unloading Sound: %s\n", cur->filePath);
		UnloadSound(cur->sound);
		cur->nUsers = 0;
		cur->filePath[0] = '\0';
	}
	gResourceManager.soundUsedList = NULL;
	//TODO: should also reset or add to freelist, but maybe wont assuming this is only called on program cleanup.
}

static void UnloadTextures(void)
{
	for (Resource_Texture* cur = gResourceManager.textureUsedList; cur != NULL; cur = cur->next)
	{
		printf("RES: Unloading Texture: %s\n", cur->filePath);
		UnloadTexture(cur->texture);
		cur->nUsers = 0;
		cur->filePath[0] = '\0';
	}
	gResourceManager.textureUsedList = NULL;
	//TODO: should also reset freelist, but maybe wont assuming this is only called on program cleanup.
}

ModelHandle RES_LoadModel(const char* fileName)
{
	if (fileName == NULL)
		return NULL_MODEL;

	for (Resource_Model* cur = gResourceManager.modelUsedList; cur != NULL; cur = cur->next)
	{
		if (strncmp(fileName, cur->filePath, MAX_FILENAME) == 0)
		{
			cur->nUsers++;
			printf("Model Found in cache: %s\n", fileName);
			return cur->index;
		}
	}

	Resource_Model* rm = gResourceManager.modelFreeList;
	if (rm)
	{
		gResourceManager.modelFreeList = gResourceManager.modelFreeList->next;
		rm->next = gResourceManager.modelUsedList;
		gResourceManager.modelUsedList = rm;

		rm->model = LoadModel(fileName);
		strncpy(rm->filePath, fileName, MAX_FILENAME);
		rm->filePath[MAX_FILENAME - 1] = '\0';
		SetupModelMaterial(&rm->model);
		rm->nUsers++;
		return rm->index;
	}
	else
		printf("Model load unsuccessful: Out of model resources.\n");

	return NULL_MODEL;
}

inline bool RES_IsValidModelHandle(const ModelHandle m) { return (m >= 0 && m < MAX_MODELS && gResourceManager.models[m].nUsers); }

//For unloading individual models at runtime.
void RES_UnloadModel(const ModelHandle m)
{
	if (!RES_IsValidModelHandle(m))
	{
		printf("Error: Trying to unload with invalid model handle.\n");
		return;
	}

	Resource_Model* rm = &gResourceManager.models[m];

	Resource_Model* cur, * prev = NULL;
	for (cur = gResourceManager.modelUsedList; cur != NULL; cur = cur->next)
	{
		if (cur->index == rm->index)
		{
			if (prev)
				prev->next = cur->next;
			else
				gResourceManager.modelUsedList = cur->next;

			break;
		}
		prev = cur;
	}

	if (cur)
	{
		printf("RES: Unloading model: %s\n", rm->filePath);
		UnloadModel(rm->model);
		rm->nUsers--;
		rm->filePath[0] = '\0';
		rm->next = gResourceManager.modelFreeList;
		gResourceManager.modelFreeList = rm;
	}
	else
	{
		printf("Error: Could not find model in used list\n");
	}

}

static void UnloadModels(void)
{
	for (Resource_Model* cur = gResourceManager.modelUsedList; cur != NULL; cur = cur->next)
	{
		UnloadModel(cur->model);
		cur->nUsers = 0;
		cur->filePath[0] = '\0';
	}
	gResourceManager.modelUsedList = NULL;
	//TODO: should also reset freelist, but maybe wont assuming this is only called on program cleanup.
}

Model* RES_GetModel(const ModelHandle m)
{
	if (!RES_IsValidModelHandle(m))
		return NULL; //TODO: throw in an error message as well.

	return &gResourceManager.models[m].model;
}

void RES_DrawModel(const ModelHandle m, Vector3 pos, float scale)
{
	if (RES_IsValidModelHandle(m))
		DrawModel(gResourceManager.models[m].model, pos, scale, WHITE);
}

void RES_DrawModelEx(const ModelHandle m, Vector3 pos, Vector3 rotAxis, float angle, Vector3 scale)
{
	if (RES_IsValidModelHandle(m))
		DrawModelEx(gResourceManager.models[m].model, pos, rotAxis, angle, scale, WHITE);
}

void RES_DrawModelWiresEx(const ModelHandle m, Vector3 pos, Vector3 rotAxis, float angle, Vector3 scale, Color color)
{
	if (RES_IsValidModelHandle(m))
		DrawModelWiresEx(gResourceManager.models[m].model, pos, rotAxis, angle, scale, color);
}

void RES_UpdateModelAnimation(const ModelHandle m, ModelAnimation anim, int frame)
{
	if (RES_IsValidModelHandle(m))
		UpdateModelAnimation(gResourceManager.models[m].model, anim, frame);
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

