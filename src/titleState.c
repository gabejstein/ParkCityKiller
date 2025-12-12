#include "titleState.h"
#include "common.h"

static const Color bgColor = { 229,215,194,255 };

static void TitleState_Update(float dt);
static void TitleState_Render(void);

void TitleState_Start(void)
{

	gGame.update = TitleState_Update;
	gGame.render = TitleState_Render;
	
}

static void TitleState_Update(float dt)
{
	
}

static void TitleState_Render(void)
{
	ClearBackground(bgColor);

}