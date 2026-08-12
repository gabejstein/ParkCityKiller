#include "skybox.h"
#include <raymath.h>

//skybox
static Shader skyShader = { 0 };
static Mesh skyBoxMesh;
static unsigned int moveLoc;
static float skyRot = 0.0f;
static float skyRotSpeed = 1.0f;

void Skybox_CommonInit(void)
{
	skyShader = LoadShader("assets/shaders/skybox.vs", "assets/shaders/skybox.fs");
	moveLoc = GetShaderLocation(skyShader, "movement");
}

void Skybox_Init(Skybox* skybox)
{
	
	skyBoxMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
	skybox->box = LoadModelFromMesh(skyBoxMesh);

	skybox->box.materials[0].shader = skyShader;
	SetShaderValue(skybox->box.materials[0].shader, GetShaderLocation(skybox->box.materials[0].shader, "environmentMap"), (int[1]) { MATERIAL_MAP_CUBEMAP }, SHADER_UNIFORM_INT);

	Image img = LoadImage(skybox->texturePath);
	skybox->box.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(img, CUBEMAP_LAYOUT_LINE_HORIZONTAL);
	SetTextureFilter(skybox->box.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture, TEXTURE_FILTER_POINT);
	UnloadImage(img);
}

void Skybox_CommonUnload(void)
{
	UnloadShader(skyShader);
}

void Skybox_Unload(Skybox* skybox)
{
	UnloadModel(skybox->box);
}

void Skybox_SetRotSpeed(float speed)
{
	skyRotSpeed = speed;
}

void Skybox_Update(Camera* camera, float dt)
{
	skyRot += skyRotSpeed * dt;
}

void Skybox_Draw(Skybox* skybox)
{
	rlDisableBackfaceCulling();
	rlDisableDepthMask();
		DrawModelEx(skybox->box, Vector3Zero(), (Vector3) { 0, 1.0f, 0 }, skyRot, Vector3One(), WHITE);
	rlEnableBackfaceCulling();
	rlEnableDepthMask();
}