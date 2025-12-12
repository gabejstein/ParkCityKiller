#include "pauseMenuState.h"
#include "common.h"
#include "gui/selectionList.h"
#include "playState.h"

static void PauseMenuState_Update(float dt);
static void PauseMenuState_Render(void);
static void PauseMenuState_Unload(void);

SelectionList* listTest = NULL;

void PauseMenuState_Start(void)
{
	gGame.update = PauseMenuState_Update;
	gGame.render = PauseMenuState_Render;
	gGame.unload = PauseMenuState_Unload;

	char* options[] = {
		"Birdy",
		"Susan",
		"Level up",
		"Exit"
	};

	listTest = NewSelectionList(11, NULL, NULL);
	listTest->x = 30;
	listTest->y = 30;
	listTest->nColumns = 2;
	listTest->nRows = 8;
	listTest->maxDisplayRows = 4;
}

static void PauseMenuState_Update(float dt)
{
	UpdateSelectionList(listTest, dt);

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT))
	{
		gGame.unload();
		PlayState_Start();
	}
		
}

static void PauseMenuState_Render(void)
{
	Color color = (Color){100,100,100,255};
	DrawRectangle(10, 10, VIRTUAL_WINDOW_W-20, VIRTUAL_WINDOW_H-20, color);

	RenderSelectionList(listTest);
}

static void PauseMenuState_Unload(void)
{
	UnloadSelectionList(listTest);
}