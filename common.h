#ifndef common_h
#define common_h

#include"stdafx.h"

GLfloat spec[] = {1.0f, 1.0f ,1.0f ,1.0f };      //sets specular highlight of balls 
GLfloat posl[] = {0,0,1000};               //position of ligth source 
GLfloat amb[] = {0.2f, 0.2f, 0.2f ,1.0f};   //global ambient 
GLfloat amb2[] = {0.3f, 0.3f, 0.3f ,1.0f};  //ambient of lightsource

GLUquadricObj* cylinder_obj;              //Quadratic object to render the cylinders
GLuint texture[5], dlist;                 //stores texture objects and display list
GLuint	base;


const int BALL_RADIUS = 14;

enum Plane { PLANE_COUNT = 4, PLANE_POS = 610 };

enum Paddle { PADDLE_LENGTH = 100, PADDLE_HEIGHT = 10, PADDLE_INITY = -400};

enum {TARGET_HEIGHT = 60, TARGET_LENGTH = 80, TARGET_SPACE = 30};
enum TargetCount {TARGET_ROW_COUNT = 4, TARGET_COLUMN_COUNT = 10 };


const int cor = 60;

double Time = 0.06;                          //timestep of simulation

bool pause = false;
bool gof = false;
bool bflag = false;
int flag = 0;

float score = 0;     
BOOL gameOver = FALSE;
	
bool			keys[256];			
bool			active = TRUE;		
bool			fullscreen = TRUE;	


void  TogglePause() { pause = !pause; }

#endif
