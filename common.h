#ifndef common_h
#define common_h

#include"stdafx.h"

GLfloat spec[] = {1.0f, 1.0f ,1.0f ,1.0f };      //sets specular highlight of balls (отражение мяча)
GLfloat posl[] = {0,0,1000};               //position of ligth source (положение источника)
GLfloat amb[] = {0.2f, 0.2f, 0.2f ,1.0f};   //global ambient (окружение)
GLfloat amb2[] = {0.3f, 0.3f, 0.3f ,1.0f};  //ambient of lightsource

GLUquadricObj* cylinder_obj;              //Quadratic object to render the cylinders
GLuint texture[5], dlist;                 //stores texture objects and display list
GLuint	base;


const int PLANE_COUNT = 4;
const int  PLANE_POS = 610;
const int BALL_RADIUS = 14;

const int PADDLE_LENGTH = 100;
const int PADDLE_HEIGHT = 10;
const int PADDLE_INITY = -400;

const int TARGET_HEIGHT = 60;
const int TARGET_LENGTH = 80;
const int TARGET_SPACE = 30;

const int TARGET_ROW_COUNT = 4;
const int TARGET_COLUMN_COUNT = 10;

const int cor = 60;

double Time = 0.06;                          //timestep of simulation

bool pause = false;
bool gof = false;
bool bflag = false;
int flag = 0;

float score = 0;       // score
BOOL gameOver = FALSE;
	
bool			keys[256];			
bool			active = TRUE;		
bool			fullscreen = TRUE;	


void  TogglePause() { pause = !pause; }

#endif