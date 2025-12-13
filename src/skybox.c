#include "skybox.h"
#include <raymath.h>

//skybox
static Shader skyShader = { 0 };
static Mesh skyBoxMesh;
static Model skyBox = { 0 };
static unsigned int moveLoc;
static float skyRot = 0.0f;

void InitSkybox(void)
{
	skyShader = LoadShader("assets/shaders/skybox.vs", "assets/shaders/skybox.fs");
	moveLoc = GetShaderLocation(skyShader, "movement");

	skyBoxMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
	skyBox = LoadModelFromMesh(skyBoxMesh);

	skyBox.materials[0].shader = skyShader;
	SetShaderValue(skyBox.materials[0].shader, GetShaderLocation(skyBox.materials[0].shader, "environmentMap"), (int[1]) { MATERIAL_MAP_CUBEMAP }, SHADER_UNIFORM_INT);

	Image img = LoadImage("assets/textures/skybox_02.png");
	skyBox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(img, CUBEMAP_LAYOUT_LINE_HORIZONTAL);
	SetTextureFilter(skyBox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture, TEXTURE_FILTER_POINT);
	UnloadImage(img);
}

void UnloadSkybox(void)
{
	UnloadModel(skyBox);
	UnloadShader(skyShader);
}

void UpdateSkybox(Camera* camera, float dt)
{
	skyRot += 2.0f * dt;
}

void DrawSkyBox(void)
{
	//SetShaderValue(skyShader, moveLoc, (float[3]) { 0.0, skyRot, 0.0 }, SHADER_UNIFORM_VEC3);

	rlDisableBackfaceCulling();
	rlDisableDepthMask();
		DrawModelEx(skyBox, Vector3Zero(), (Vector3) { 0, 1.0f, 0 }, skyRot, Vector3One(), WHITE);
	rlEnableBackfaceCulling();
	rlEnableDepthMask();
}