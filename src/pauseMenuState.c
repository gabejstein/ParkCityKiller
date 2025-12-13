#include "pauseMenuState.h"
#include "common.h"
#include "gui/selectionList.h"

static void PauseMenuState_Start(void);
static void PauseMenuState_Update(float dt);
static void PauseMenuState_Render(void);
static void PauseMenuState_Unload(void);

static SelectionList* listTest = NULL;

GameState GetPauseMenuState(void)
{
	GameState state = { 0 };

	state.start = PauseMenuState_Start;
	state.update = PauseMenuState_Update;
	state.render = PauseMenuState_Render;
	state.unload = PauseMenuState_Unload;

	return state;
}

void PauseMenuState_Start(void)
{
	printf("Starting pause state.\n");

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
		PopGameState();
	}
		
}

static void PauseMenuState_Render(void)
{
	Color color = (Color){10,10,10,200};
	DrawRectangle(10, 10, VIRTUAL_WINDOW_W-20, VIRTUAL_WINDOW_H-20, color);

	RenderSelectionList(listTest);
}

static void PauseMenuState_Unload(void)
{
	printf("Unloading pause menu.\n");
	UnloadSelectionList(listTest);
}