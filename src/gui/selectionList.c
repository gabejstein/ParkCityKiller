#include "selectionList.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

//TODO: Need to abstract this stuff out into an input manager, so the whole game can use it.
static float lastAxisDown = 0.0f;
static float lastAxisUp = 0.0f;
static float lastAxisLeft = 0.0f;
static float lastAxisRight = 0.0f;

static const float deadZone = 0.3f;
static const float threshold = 0.6f;

static int IsGamepadStickUp(int gamepad)
{
	float vertAxis = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_LEFT_Y);
	if (fabs(vertAxis) < deadZone) vertAxis = 0.0f;

	int isPressed = (vertAxis < -threshold && lastAxisUp >= -threshold);

	lastAxisUp = vertAxis;

	return isPressed;
}

static int IsGamepadStickDown(int gamepad)
{
	float vertAxis = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_LEFT_Y);
	if (fabs(vertAxis) < deadZone) vertAxis = 0.0f;

	int isPressed = (vertAxis > threshold && lastAxisDown <= threshold);

	lastAxisDown = vertAxis;

	return isPressed;
}

static int IsGamepadStickLeft(int gamepad)
{
	float horAxis = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_LEFT_X);
	if (fabs(horAxis) < deadZone) horAxis = 0.0f;

	int isPressed = (horAxis < -threshold && lastAxisLeft >= -threshold);

	lastAxisLeft = horAxis;

	return isPressed;
}

static int IsGamepadStickRight(int gamepad)
{
	float horAxis = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_LEFT_X);
	if (fabs(horAxis) < deadZone) horAxis = 0.0f;

	int isPressed = (horAxis > threshold && lastAxisRight <= threshold);

	lastAxisRight = horAxis;

	return isPressed;
}

SelectionList* NewSelectionList(int nOptions, void(*onSelect), void(*renderItem))
{
	SelectionList* list = (SelectionList*) malloc(sizeof(SelectionList));
	if (!list)
	{
		printf("Could not allocate memory for selection list.\n");
		return NULL;
	}
	
	memset(list, 0, sizeof(SelectionList));

	list->nColumns = 1; //just having one column for now.
	list->nRows = nOptions;
	list->nOptions = nOptions;
	list->OnSelect = onSelect;
	list->RenderItem = renderItem;
	list->bShowCursor = 1;
	list->colPadding = 64;
	list->rowPadding = 16;
	list->maxDisplayRows = nOptions;

	/*list->options = (char*) malloc(sizeof(char) * MAX_OPTION_LENGTH * nOptions);
	
	if (!list->options)
	{
		printf("Could not create selection list options\n");
		return NULL;
	}

	memset(list->options, 0, sizeof(char) * MAX_OPTION_LENGTH * nOptions);

	for (int i = 0; i < nOptions; i++)
	{
		strncpy(list->options[i], options[i], MAX_OPTION_LENGTH);
	}*/

	int n = 0;
	list->options[n++] = "Fire 1";
	list->options[n++] = "Firaga";
	list->options[n++] = "Thunder";
	list->options[n++] = "Thunder 2";
	list->options[n++] = "Cure 1";
	list->options[n++] = "Run Away";
	list->options[n++] = "Yo mama";
	list->options[n++] = "Cheese soup";
	list->options[n++] = "Cure 2";
	list->options[n++] = "Curaga";
	list->options[n++] = "Tornado";
	
	return list;
}

static void CalculateListWidth(SelectionList* list)
{
	//TODO: Get width by getting longest label word

}

static void CalculateListHeight(SelectionList* list)
{
	//TODO: Get height based on text height + padding

}

static void OnSelect(SelectionList* list)
{
	int index = list->curY * list->nColumns + list->curX;
	if (list->OnSelect)
		list->OnSelect(index); //TODO: Need to calculate 1d selection from 2d

	printf("Selected: %d\n", index);
}

void UpdateSelectionList(SelectionList* list, float dt)
{
	//TODO: Really need to abstract things out into an input manager.
	int gamepad = 0;

	//(vertAxis > 0)
	int upPressed = IsGamepadStickUp(gamepad) || IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_LEFT_FACE_UP);
	int downPressed = IsGamepadStickDown(gamepad) || IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN);
	int leftPressed = IsGamepadStickLeft(gamepad) || IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
	int rightPressed = IsGamepadStickRight(gamepad) || IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);

	if (upPressed)
	{
		list->curY = MAX(0, list->curY - 1);
		if (list->curY < list->displayStart)
		{
			list->displayStart--;
		}
	}
	else if (downPressed)
	{
		list->curY = MIN(list->nRows-1, list->curY + 1);
		if (list->curY > (list->displayStart+list->maxDisplayRows-1))
		{
			list->displayStart++;
		}
	}
	else if (leftPressed)
	{
		list->curX = MAX(0, list->curX - 1);
	}
	else if (rightPressed)
	{
		list->curX = MIN(list->nColumns-1, list->curX + 1);
	}

	if (IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT))
	{
		OnSelect(list);
	}
}

void RenderSelectionList(SelectionList* list)
{
	unsigned int x = list->x, y = list->y;

	int displayStart = list->displayStart;
	int displayEnd = list->displayStart + list->maxDisplayRows;
	int index = list->displayStart * list->nColumns;

	for (int row = displayStart; row < displayEnd; row++)
	{
		for (int col = 0; col < list->nColumns; col++)
		{
			//draw cursor
			if (list->curY == row && list->curX== col)
			{
				DrawRectangle(x - 14, y, 8, 8, YELLOW);
			}

			if (index < list->nOptions)
				DrawText(list->options[index], x, y, 14, WHITE);
			else
				DrawText("---", x, y, 14, WHITE);
			
			x += list->colPadding;
			index++;
		}

		x = list->x;
		y += list->rowPadding;
		
	}

	//TODO: display some indicator if there's more stuff than on screen. Could be done by a higher level object.
	//DrawTriangle()
}

void UnloadSelectionList(SelectionList* list)
{
	if (!list)return;

	free(list);
	list = NULL;
}