#ifndef CONST_H
#define CONST_H

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

//#define VIRTUAL_WINDOW_W 640
//#define VIRTUAL_WINDOW_H 360
#define VIRTUAL_WINDOW_W 320
#define VIRTUAL_WINDOW_H 180

#define MAX_DIALOGUE 256

#define MAX_FILENAME 256

#define FPS 60

static const float SECS_PER_FRAME = 1.0f / FPS;
static const float GRAVITY = 50.6f;
static const float MAX_FALL = -58.0f;
static const float MAX_HOR_VEL = 58.0f;

#define UP (Vector3){0,1,0}

#endif
