#ifndef SELECTION_LIST_H
#define SELECTION_LIST_H

#define MAX_OPTION_LENGTH 256

typedef struct
{
	unsigned int nOptions;
	int x, y;
	unsigned int width, height;
	unsigned int nRows, nColumns;
	int curX, curY;
	int displayStart;
	int maxDisplayRows;
	int colPadding, rowPadding;
	int bShowCursor;
	void (*OnSelect)(int);
	void (*RenderItem)(char* label);
	char* options[11];
}SelectionList;

SelectionList* NewSelectionList(int nOptions, void(*onSelect), void(*renderItem));
void UpdateSelectionList(SelectionList* list, float dt);
void RenderSelectionList(SelectionList* list);
void UnloadSelectionList(SelectionList* list);

#endif
