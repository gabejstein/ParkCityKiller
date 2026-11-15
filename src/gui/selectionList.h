#ifndef SELECTION_LIST_H
#define SELECTION_LIST_H

#include <raylib.h>

typedef struct
{
	unsigned int optionsCount;
	char** optionLabels;
	int x, y;
	unsigned int width, height;
	unsigned int nRows, nColumns;
	int curX, curY;
	int displayStart;
	int maxDisplayRows;
	int colPadding, rowPadding;
	int bShowCursor;
	void (*OnSelect)(int,void*);
	void (*RenderItem)(char* label);
	Font font;
	void* data;
}SelectionList;

SelectionList* SelectionList_New(int optionsCount,char** labels, void(*onSelect));
SelectionList* SelectionList_NewEx(int optionsCount,char** labels,int columns, int rows, void(*onSelect), void(*renderItem),Font font);
void SelectionList_Update(SelectionList* list, float dt);
void SelectionList_Render(SelectionList* list);
void SelectionList_Unload(SelectionList* list);
void SelectionList_ChangeText(const int index, const char* text);

#endif
