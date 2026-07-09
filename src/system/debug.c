#include "debug.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_OVERLAY 12
#define MAX_CONSOLE 24
#define BUFFER_SIZE 256

//Overlays
static int overlayX=100, overlayY=10;
static int margins = 18;
static unsigned int overlayCount = 0;
static char overlays[MAX_OVERLAY][BUFFER_SIZE];

//Console queue
static float queueTimer;
static int consoleX = 30, consoleY = 10;
static char consoleQueue[MAX_CONSOLE][BUFFER_SIZE];
static unsigned int queueHead=0, queueTail=0;

void Debug_PrintOverlay(const char* txt, ...)
{
	
	if (overlayCount < MAX_OVERLAY)
	{
		va_list args;
		va_start(args, txt);
		vsnprintf(overlays[overlayCount],BUFFER_SIZE,txt,args);
		va_end(args);

		overlayCount++;
	}
		
}

void Debug_RenderOverlays(void)
{
	int y = overlayY;
	for (unsigned int i = 0; i < overlayCount; i++)
	{
		DrawText(overlays[i], overlayX, y, 16, GREEN);
		y += margins;
	}

}

void Debug_ResetOverlays(void)
{
	overlayCount = 0;
}

void Debug_PingConsole(const char* txt, ...)
{
	if ((queueTail+1)%MAX_CONSOLE==queueHead)return;

	va_list args;
	va_start(args, txt);
	vsnprintf(consoleQueue[queueTail], BUFFER_SIZE, txt, args);
	va_end(args);

	queueTail = (queueTail+1)%MAX_CONSOLE;

}

void Debug_UpdateConsole(float dt)
{
	if (queueHead == queueTail) return;

	queueTimer += dt;
	if (queueTimer >= 2)
	{
		queueTimer = 0;
		queueHead =  (queueHead + 1) % MAX_CONSOLE;
	}
}

void Debug_RenderConsole(void)
{
	int y = consoleY;
	for (int i = queueHead; i !=queueTail; i=(i+1)%MAX_CONSOLE)
	{
		DrawText(consoleQueue[i], consoleX, y, 16, GREEN);
		y += margins;
	}
}