#ifndef gameobjects_h
#define gameobjects_h

#include"tvector.h"

typedef struct 
{
	TVector position;
	TVector normal;
} plane_t;

typedef struct 
{
	TVector velocity;
	TVector position;
	TVector oldPos;
} ball_t;

typedef struct 
{
	TVector position;
	bool active;
	TVector point[4];
} box_t;

typedef struct {
	TVector position;
	float   alpha;
	float   scale;
} explosion_t;

#endif