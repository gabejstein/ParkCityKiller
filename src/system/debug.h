#ifndef DEBUG_H
#define DEBUG_H

void Debug_PrintOverlay(const char* txt,...);
void Debug_RenderOverlays(void);
void Debug_ResetOverlays(void);
void Debug_PingConsole(const char* txt, ...);
void Debug_UpdateConsole(float dt);
void Debug_RenderConsole(void);

#endif
