#include "selectionList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

#define MAX_OPTION_LENGTH 256

//TODO: Need to abstract this stuff out into an input manager, so the whole game can use it.
static float lastAxisDown = 0.0f;
static float lastAxisUp = 0.0f;
static float lastAxisLeft = 0.0f;
static float lastAxisRight = 0.0f;

static const float deadZone = 0.3f;
static const float threshold = 0.6f;

static void DefaultRenderItem(SelectionList* list, int i);

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

SelectionList* SelectionList_NewEx(int optionsCount, char** labels, int columns, int rows, void(*onSelect), void(*renderItem),Font font)
{
	SelectionList* list = (SelectionList*)malloc(sizeof(SelectionList));
	if (!list)
	{
		printf("Could not allocate memory for selection list.\n");
		return NULL;
	}
	memset(list, 0, sizeof(SelectionList));

	list->optionsCount = optionsCount;
	list->nColumns = columns; 
	list->nRows = optionsCount;
	list->OnSelect = onSelect;
	list->RenderItem = renderItem;
	list->bShowCursor = 1;
	list->colPadding = 64;
	list->rowPadding = 16;
	list->maxDisplayRows = optionsCount;
	list->font = font;
	list->data = NULL;

	list->optionLabels = (char**)malloc(sizeof(char*) * optionsCount);
	if (!list->optionLabels)
	{
		printf("Could not allocate option labels.\n");
		return NULL;
	}

	for (int i = 0; i < optionsCount; i++)
	{
		list->optionLabels[i] = (char*) malloc(sizeof(char) * MAX_OPTION_LENGTH);
		strncpy(list->optionLabels[i], labels[i], MAX_OPTION_LENGTH);
		//list->optionLabels[i][strlen(labels[i]) + 1] = '\0';
		printf("Option %d: %s\n", i, list->optionLabels[i]);
	}

	return list;
}

SelectionList* SelectionList_New(int optionsCount, char** labels, void(*onSelect))
{
	SelectionList* list = SelectionList_NewEx(optionsCount,labels, 2, optionsCount, onSelect, DefaultRenderItem,GetFontDefault());
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
		list->OnSelect(index,list->data); //TODO: Need to calculate 1d selection from 2d

	printf("Selected: %d\n", index);
}

void SelectionList_Update(SelectionList* list, float dt)
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

static void DefaultRenderItem(SelectionList* list, int i)
{

}

void SelectionList_Render(SelectionList* list)
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

			if (index < list->optionsCount)
				DrawText(list->optionLabels[index], x, y, 14, WHITE);
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

void SelectionList_Unload(SelectionList* list)
{
	if (!list)return;

	for (int i = 0; i < list->optionsCount; i++)
		free(list->optionLabels[i]);

	free(list->optionLabels);

	free(list);
	list = NULL;
}

void SelectionList_ChangeText(const int index, const char* text)
{

}