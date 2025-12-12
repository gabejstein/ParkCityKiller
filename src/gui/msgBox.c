#include "msgBox.h"
#include "../common.h"
#include <string.h>

#define MAX_MSG 256
#define MAX_QUEUE 12

static char msgQueue[MAX_QUEUE][MAX_MSG];

static int head,tail;
static float timer = 0.0f, msgDuration = 1.5f;

static int x = 10, y = 10, width = VIRTUAL_WINDOW_W-20, height = 20;

void ResetMsgBox(void)
{
	head = tail = 0;
}

void UpdateMsgBox(float dt)
{
	if (!head)return;

	timer += dt;

	if (timer >= msgDuration)
	{
		timer = 0;
		tail++;
		if (tail >= head)
			tail = head = 0;
	}

}

void PushMsgBox(const char* msg)
{
	if (head >= MAX_QUEUE)
		return;

	strncpy(msgQueue[head++], msg, MAX_MSG);

}

void DrawMsgBox(void)
{
	if (!head)return;

	//panel box
	Color bg = { 100,100,100,255 };
	DrawRectangle(x, y, width, height, bg);
	DrawRectangleLines(x, y, width, height, BLACK);

	//text
	const char* msg = msgQueue[tail];
	int padding = 5;
	DrawText(msg, x + padding, y + padding, 12, WHITE);
}