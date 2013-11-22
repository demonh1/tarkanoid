#include"stdafx.h"

#include <windows.h>		
#include <cmath>			
#include <cstdlib>			
#include <ctime>

#include "tvector.h"
#include "tray.h"
#include "image.h"
#include "gameobjects.h"
#include "common.h"
#include "freetype.h"


//font
freetype::font_data font; 
freetype::font_data font1; 


TVector dir(0,0,-10);                     //initial direction of camera
TVector pos(0,0,1000);                  //initial position of camera

TVector accel(0,0,0);                 //acceleration ie. gravity of balls


plane_t planes[PLANE_COUNT];          
ball_t ball;

box_t paddle;
box_t target[TARGET_ROW_COUNT][TARGET_COLUMN_COUNT];               
TVector normal[4];
explosion_t ExplosionArray[20];             

//Perform Intersection tests with primitives
int TestIntersectionPlane(const plane_t& plane,const TVector& position,const TVector& direction, double& lamda, TVector& pNormal);
int TestIntersectionTarget(const box_t& target, const TVector& position, const TVector& direction, double& lamda, TVector& pNormal);

void LoadGLTextures();                    
void InitVars();
void idle();

HDC				hDC = NULL;			
HGLRC			hRC = NULL;		
HWND			hWnd = NULL;			
HINSTANCE		hInstance;		

DEVMODE			DMsaved;			


BOOL ProcessKeys();
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	


//==========================================================
//	GLvoid ReSizeGLScene(GLsizei width, GLsizei height)
//	Resize And Initialize The GL Window
//==========================================================
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		
{
	if (height == 0)										
	{
		height = 1;										
	}

	glViewport(0,0,width,height);						

	glMatrixMode(GL_PROJECTION);						
	glLoadIdentity();									

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(50.0f,(GLfloat)width/(GLfloat)height,10.f,1700.0f);

	glMatrixMode(GL_MODELVIEW);							
	glLoadIdentity();									
}

//=================================================================
//	int InitGL(GLvoid)	
//	setup for OpenGL 
//==================================================================
int InitGL(GLvoid)										
{
	float df = 100.0f;
	

	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	glClearColor(0,0,0,0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glMaterialfv(GL_FRONT,GL_SPECULAR,spec);
	glMaterialfv(GL_FRONT,GL_SHININESS,&df);

	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0,GL_POSITION,posl);
	glLightfv(GL_LIGHT0,GL_AMBIENT,amb2);
	glEnable(GL_LIGHT0);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amb);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glEnable(GL_TEXTURE_2D);
	LoadGLTextures();
	
	//Construct billboarded explosion primitive as display list
	//4 quads at right angles to each other
	glNewList(dlist=glGenLists(1), GL_COMPILE);
	glBegin(GL_QUADS);
	glRotatef(-45,0,1,0);
	glNormal3f(0,0,1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-50,-40,0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(50,-40,0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(50,40,0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-50,40,0);
	glNormal3f(0,0,-1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-50,40,0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(50,40,0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(50,-40,0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-50,-40,0);

	glNormal3f(1,0,0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0,-40,50);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0,-40,-50);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0,40,-50);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0,40,50);
	glNormal3f(-1,0,0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0,40,50);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0,40,-50);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0,-40,-50);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0,-40,50);
	
	glEnd();
	glEndList();
    
	font.init("knight.TTF", 20);
	font1.init("test.TTF", 13);
	
	return TRUE;										
}

//===========================================================
// DrawGLScene()
//===========================================================
BOOL DrawGLScene(GLvoid)	            
{								
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(pos.X(),pos.Y(),pos.Z(), pos.X()+dir.X(),pos.Y()+dir.Y(),pos.Z()+dir.Z(), 0,1.0,0.0);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	if (flag == 1) {
	
		glPushMatrix();
	glLoadIdentity();
	
	glTranslatef(-180,0,0);
	freetype::print(font, 320, 170, "Winner!!! Score: %d",int(score/cor));
	glPopMatrix(); 
	pause = true;

	glPushMatrix();
	glLoadIdentity();
	glColor3ub(0,0,0xff);
	freetype::print(font, 160, 120, "\nNew game? [y/n]");
	//glTranslatef(-180,0,0);
	glPopMatrix(); 

	gof = true;
	
	}
	if  (flag == 0) pause = false;
	
	//render balls
	glColor3f(1.0f,0.0f,1.0f);

	glPushMatrix();
	glTranslated(ball.position.X(),ball.position.Y(),ball.position.Z());

	gluSphere(cylinder_obj,BALL_RADIUS,20,20);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f,1.0f,1.0f);
	glBegin(GL_QUADS);
	glNormal3f( 0.0f, 0.0f, 1.0f);
	glVertex3d(paddle.position.X()-PADDLE_LENGTH,paddle.position.Y()-PADDLE_HEIGHT,paddle.position.Z());
	glVertex3d(paddle.position.X()+PADDLE_LENGTH,paddle.position.Y()-PADDLE_HEIGHT,paddle.position.Z());
	glVertex3d(paddle.position.X()+PADDLE_LENGTH,paddle.position.Y()+PADDLE_HEIGHT,paddle.position.Z());
	glVertex3d(paddle.position.X()-PADDLE_LENGTH,paddle.position.Y()+PADDLE_HEIGHT,paddle.position.Z());

	glEnd();
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);

	for (int i = 0; i < TARGET_ROW_COUNT; i++)
	{
		for (int j = 0; j  < TARGET_COLUMN_COUNT; j++)
		{
			if (target[i][j].active)
			{
				glBindTexture(GL_TEXTURE_2D, texture[i%4]);
				glPushMatrix();
				glTranslated(target[i][j].position.X(),target[i][j].position.Y(),target[i][j].position.Z());
				glBegin(GL_QUADS);
				glNormal3f( 0.0f, 0.0f, 1.0f);
				glTexCoord2f(0.0f,0.0f); glVertex3f(-1*TARGET_LENGTH/2, -1*TARGET_HEIGHT/2, 0);
				glTexCoord2f(1.0f,0.0f); glVertex3f(TARGET_LENGTH/2, -1*TARGET_HEIGHT/2, 0);
				glTexCoord2f(1.0f,1.0f); glVertex3f(TARGET_LENGTH/2, TARGET_HEIGHT/2, 0);
				glTexCoord2f(0.0f,1.0f); glVertex3f(-1*TARGET_LENGTH/2, TARGET_HEIGHT/2, 0);

				glEnd();
				glPopMatrix();
			}
		}
	}
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	
	for(int i = 0; i < 20; i++)
	{
		if(ExplosionArray[i].alpha>=0)
		{
			glPushMatrix();
			score += 0.5;
			
			ExplosionArray[i].alpha-=0.01f;
			ExplosionArray[i].scale+=0.03f;
			glColor4f(1,1,0,ExplosionArray[i].alpha);	 
			glScalef(ExplosionArray[i].scale,ExplosionArray[i].scale,ExplosionArray[i].scale);
			glTranslatef((float)ExplosionArray[i].position.X()/ExplosionArray[i].scale, (float)ExplosionArray[i].position.Y()/ExplosionArray[i].scale, (float)ExplosionArray[i].position.Z()/ExplosionArray[i].scale);
			glCallList(dlist);
			glPopMatrix();
		}
	}
	
			if ( (!target[0][0].active) && (!target[0][1].active) &&  (!target[0][2].active) && (!target[0][3].active)&&
				(!target[0][4].active) && (!target[0][5].active) &&  (!target[0][6].active) && (!target[0][7].active)&&
				(!target[0][8].active) && (!target[0][9].active) &&
				(!target[1][0].active) && (!target[1][1].active) &&  (!target[0][2].active) && (!target[0][3].active)&&
				(!target[1][4].active) && (!target[1][5].active) &&  (!target[1][6].active) && (!target[1][7].active)&&
				(!target[1][8].active) && (!target[1][9].active) &&
				(!target[2][0].active) && (!target[2][1].active) &&  (!target[2][2].active) && (!target[2][3].active)&&
				(!target[2][4].active) && (!target[2][5].active) &&  (!target[2][6].active) && (!target[2][7].active)&&
				(!target[2][8].active) && (!target[2][9].active) &&
				(!target[3][0].active) && (!target[3][1].active) &&  (!target[3][2].active) && (!target[3][3].active)&&
				(!target[3][4].active) && (!target[3][5].active) &&  (!target[3][6].active) && (!target[3][7].active)&&
				(!target[3][8].active) && (!target[3][9].active) 
				) 
			flag = 1;
			
	
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	
	return TRUE;										
}

//===========================================================
// BOOL DrawLoadScreen ()
//===========================================================
BOOL DrawLoadScreen () {

glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();



	gluLookAt(pos.X(),pos.Y(),pos.Z(), pos.X()+dir.X(),pos.Y()+dir.Y(),pos.Z()+dir.Z(), 0,1.0,0.0);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glColor3f (1.0f, 0.0f, 1.0f);

	
	glPushMatrix();
	glTranslated(ball.position.X(),ball.position.Y(),ball.position.Z());
    gluSphere(cylinder_obj,BALL_RADIUS,20,20);

	glPopMatrix();
	
	glPushMatrix();
	glLoadIdentity();
	
	glTranslatef(-180,0,0);
	freetype::print(font, 320, 170, "Load Arkanoid game...\nPress space to continue...");
	glPopMatrix(); 
   
		glPushMatrix();
	glLoadIdentity();
	glColor3ub(0,0,0xff);
	freetype::print(font1, 160, 120, "\nGame control\nplatform: to the left[key_left], right[key_right];\nball: slower[f2], acceleration [f3]\nfullscreen / window mode [f11]");
	//glTranslatef(-180,0,0);
		
	glPopMatrix(); 

	glPushMatrix();
	glColor3f(1.0f,1.0f,1.0f);
	glBegin(GL_QUADS);

	glNormal3f( 0.0f, 0.0f, 1.0f);
	glVertex3d(paddle.position.X()-PADDLE_LENGTH,paddle.position.Y()-PADDLE_HEIGHT,paddle.position.Z());
	glVertex3d(paddle.position.X()+PADDLE_LENGTH,paddle.position.Y()-PADDLE_HEIGHT,paddle.position.Z());
	glVertex3d(paddle.position.X()+PADDLE_LENGTH,paddle.position.Y()+PADDLE_HEIGHT,paddle.position.Z());
	glVertex3d(paddle.position.X()-PADDLE_LENGTH,paddle.position.Y()+PADDLE_HEIGHT,paddle.position.Z());

	glEnd();
	glPopMatrix();


	for (int i = 0; i < TARGET_ROW_COUNT; i++)
	{
		for (int j = 0; j  < TARGET_COLUMN_COUNT; j++)
		{
			if (target[i][j].active)
			{
				glBindTexture(GL_TEXTURE_2D, texture[i%4]);
				glPushMatrix();
				glTranslated(target[i][j].position.X(),target[i][j].position.Y(),target[i][j].position.Z());
			    glBegin(GL_QUADS);
				glNormal3f( 0.0f, 0.0f, 1.0f);
			    glTexCoord2f(0.0f,0.0f); glVertex3f(-1*TARGET_LENGTH/2, -1*TARGET_HEIGHT/2, 0);
				glTexCoord2f(1.0f,0.0f); glVertex3f(TARGET_LENGTH/2, -1*TARGET_HEIGHT/2, 0);
				glTexCoord2f(1.0f,1.0f); glVertex3f(TARGET_LENGTH/2, TARGET_HEIGHT/2, 0);
				glTexCoord2f(0.0f,1.0f); glVertex3f(-1*TARGET_LENGTH/2, TARGET_HEIGHT/2, 0);
	
				glEnd();
				glPopMatrix();
			}
		}
	}      
	
	keys[VK_RIGHT] = FALSE; keys[VK_LEFT] = FALSE; 

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	return TRUE;
}
//====================================================================================
//GLvoid KillGLWindow(GLvoid)	
//Properly Kill The Window
//====================================================================================

GLvoid KillGLWindow(GLvoid)								
{
	if (fullscreen)										
	{
		if (!ChangeDisplaySettings(NULL,CDS_TEST)) { 
			ChangeDisplaySettings(NULL,CDS_RESET);		
			ChangeDisplaySettings(&DMsaved,CDS_RESET);	
		} else {
			ChangeDisplaySettings(NULL,CDS_RESET);
		}

		ShowCursor(FALSE);								
	}

	if (hRC)											
	{
		if (!wglMakeCurrent(NULL,NULL))					
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);

		if (!wglDeleteContext(hRC))						
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		
		hRC = NULL;										
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										
	}

	if (hWnd && !DestroyWindow(hWnd))					
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;										
	}

	if (!UnregisterClass("OpenGL",hInstance))			
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;									
	}
}

//===================================================================================================
//BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
//
//	This Code Creates Our OpenGL Window.  Parameters Are:					
//	title			- Title To Appear At The Top Of The Window				
//	width			- Width Of The GL Window Or Fullscreen Mode				
//	height			- Height Of The GL Window Or Fullscreen Mode			
//	bits			- Number Of Bits To Use For Color (8/16/24/32)			
//	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	
//=====================================================================================================
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			
	WNDCLASS	wc;						
	DWORD		dwExStyle;				
	DWORD		dwStyle;				
	RECT		WindowRect;				
	WindowRect.left=(long)0;			
	WindowRect.right=(long)width;		
	WindowRect.top=(long)0;				
	WindowRect.bottom=(long)height;		

	fullscreen = fullscreenflag;			

	hInstance			= GetModuleHandle(NULL);				
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	
	wc.lpfnWndProc		= (WNDPROC) WndProc;					
	wc.cbClsExtra		= 0;									
	wc.cbWndExtra		= 0;									
	wc.hInstance		= hInstance;							
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			
	wc.hbrBackground	= NULL;									
	wc.lpszMenuName		= NULL;									
	wc.lpszClassName	= "OpenGL";								

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DMsaved); // save the current display state

	if (fullscreen)												
	{
		DEVMODE dmScreenSettings;								
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		
		dmScreenSettings.dmPelsWidth	= width;				
		dmScreenSettings.dmPelsHeight	= height;				
		dmScreenSettings.dmBitsPerPel	= bits;					
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen = FALSE;		
			}
			else
			{
				
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									
			}
		}
	}

	if (!RegisterClass(&wc))									
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											
	}

	if (fullscreen)												
	{
		dwExStyle=WS_EX_APPWINDOW;								
		dwStyle=WS_POPUP;										
		ShowCursor(FALSE);										
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			
		dwStyle=WS_OVERLAPPEDWINDOW;							
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		

	// Create The Window
	if (!(hWnd = CreateWindowEx(dwExStyle,							
		"OpenGL",							
		title,								
		dwStyle |							
		WS_CLIPSIBLINGS |					
		WS_CLIPCHILDREN,					
		0, 0,								
		WindowRect.right-WindowRect.left,	
		WindowRect.bottom-WindowRect.top,	
		NULL,								
		NULL,								
		hInstance,							
		NULL)))								
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(hDC = GetDC(hWnd)))							
	{
		KillGLWindow();								
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	
	{
		KillGLWindow();								
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		
	{
		KillGLWindow();								
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if (!(hRC=wglCreateContext(hDC)))				
	{
		KillGLWindow();								
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if(!wglMakeCurrent(hDC,hRC))					
	{
		KillGLWindow();								
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	ShowWindow(hWnd,SW_SHOW);						
	SetForegroundWindow(hWnd);						
	SetFocus(hWnd);									
	ReSizeGLScene(width, height);					

	if (!InitGL())									
	{
		KillGLWindow();								
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	return TRUE;									
}

//============================================================
//LRESULT CALLBACK WndProc
//==============================================================
LRESULT CALLBACK WndProc(HWND	hWnd,			
						 UINT	uMsg,			
						 WPARAM	wParam,			
						 LPARAM	lParam)			
{
	switch (uMsg)									
	{
	case WM_ACTIVATE:							
		{
			if (!HIWORD(wParam))					
				active = TRUE;						
			else
				active = FALSE;						
			
			return 0;								
		}

	case WM_SYSCOMMAND:							
		{
			switch (wParam)							
			{
			case SC_SCREENSAVE:					
			case SC_MONITORPOWER:				
				return 0;							
			}
			break;									
		}

	case WM_CLOSE:								
		{
			PostQuitMessage(0);						
			return 0;								
		}

	case WM_KEYDOWN:							
		{
			keys[wParam] = TRUE;				
			return 0;								
		}

	case WM_KEYUP:								
		{
			keys[wParam] = FALSE;					
			return 0;								
		}
	case WM_SIZE:								
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  
			return 0;								
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

//=================================================================================================
//WinMain
//==================================================================================================

int WINAPI WinMain(	HINSTANCE	hInstance,			
	HINSTANCE	hPrevInstance,		
	LPSTR		lpCmdLine,			
	int			nCmdShow)			
{
	MSG		msg;									
	BOOL	done = FALSE;								


	if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
		fullscreen = FALSE;							

	InitVars();                                    

	// Create Our OpenGL Window
	if (!CreateGLWindow("Arkanoid",640,480,16,fullscreen))
	{
		return 0;									
	}
	srand(time(NULL));
	while(!done)									
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	
		{
			if (msg.message == WM_QUIT)				
			{
				done = TRUE;							
			}
			else									
			{
				TranslateMessage(&msg);				
				DispatchMessage(&msg);				
			}
		}
		else										
			if (active)
			{
				if (keys[VK_ESCAPE])	
					done = TRUE;							

				if (keys[78] && (gof)) { done = TRUE; gof = false; } //  78 -- n, 89 -- y
				if (keys[89] && (gof)) { gameOver = TRUE; flag = 0;/*TogglePause();*/  idle(); DrawGLScene(); SwapBuffers(hDC); gof = false; }

				else
				{

					if ( keys[VK_SPACE] ) { bflag = true;}

					if(bflag){

						if (keys[VK_PAUSE]){ TogglePause(); }
						if(pause) {
							DrawGLScene();                      
							SwapBuffers(hDC);
						} else{

							idle();                             
							DrawGLScene();              
							SwapBuffers(hDC);
						}
					} else { 
						DrawLoadScreen (); 
						SwapBuffers(hDC);	
					}

				}

				if (!ProcessKeys()) return 0;
			}
	}

	// Shutdown
	KillGLWindow();									
	glDeleteTextures(4,texture);
	return (msg.wParam);							
}


//===================================================================================
//             Main loop of the simulation                                      
//     Moves, finds the collisions and responses of the objects in the         
//     current time step.                                                     
//=====================================================================================

void idle()
{
	double rt,rt2,rt4,lamda=10000;
	TVector norm,uveloc;
	TVector normal,point;
	double RestTime;

	RestTime=Time;
	lamda = 1000;

	//Compute velocity for next timestep using Euler equations

	ball.velocity += accel*RestTime;

	//While timestep not over
	while (RestTime > ZERO)
	{
		lamda = 10000;   //initialize to very large value

		//For all the balls find closest intersection between balls and planes/cylinders
		//compute new position and distance
		ball.oldPos=ball.position;
		TVector::unit(ball.velocity,uveloc);
		ball.position = ball.position + ball.velocity * RestTime;
		rt2=ball.oldPos.dist(ball.position);

		//Test if collision occured between ball and all 5 planes
		for (int j = 0; j < PLANE_COUNT; j++)
		{
			if (TestIntersectionPlane(planes[j],ball.oldPos,uveloc,rt,norm))
			{  
				//Find intersection time
				rt4 = rt*RestTime/rt2;

				//if smaller than the one already stored replace and in timestep
				if (rt4 <= lamda)
				{ 
					if (rt4 <= RestTime+ZERO)
						if (! ((rt <= ZERO)&&(uveloc.dot(norm)>ZERO)) )
						{
							if(j == PLANE_COUNT-1) gameOver=TRUE;
							normal = norm;
							point = ball.oldPos + uveloc*rt;
							lamda = rt4;
						}
				}
			}
		}

		int intTarRow = -1;
		int intTarColumn = -1;

		for (int i = 0; i < TARGET_ROW_COUNT; i++)
		{
			for (int j = 0; j < TARGET_COLUMN_COUNT; j++)
			{
				if (target[i][j].active && TestIntersectionTarget(target[i][j],ball.oldPos,uveloc,rt,norm))
				{
					//Find intersection time
					rt4 = rt*RestTime/rt2;

					//if smaller than the one already stored replace and in timestep
					if (rt4 <= lamda)
					{ 

						if (rt4 <= RestTime + ZERO)
							if (! ((rt <= ZERO)&&(uveloc.dot(norm) > ZERO)) )
							{
								intTarRow = i;
								intTarColumn = j;
								gameOver = FALSE;
								normal = norm;
								point = ball.oldPos+uveloc*rt;
								lamda = rt4;
							}
					}
				}
			}
		}

		if (TestIntersectionTarget(paddle,ball.oldPos,uveloc,rt,norm))
		{
			//Find intersection time
			rt4 = rt*RestTime/rt2;

			//if smaller than the one already stored replace and in timestep
			if (rt4<=lamda)
			{ 

				if (rt4<=RestTime+ZERO)
					if (! ((rt<=ZERO)&&(uveloc.dot(norm)>ZERO)) )
					{
						intTarRow = -1;
						intTarColumn = -1;
						gameOver = FALSE;
						normal = norm;
						point = ball.oldPos+uveloc*rt;
						lamda = rt4;
					}
			}
		}
		//End of tests 
		//If test occured move simulation for the correct timestep
		//and compute response for the colliding ball
		if(gameOver)
		{
			InitVars();
			RestTime = 0;
			score = 0;
			gameOver = FALSE;
		}
		else if (lamda!=10000)
		{		 
			RestTime -= lamda;

			ball.position = ball.oldPos + ball.velocity*lamda;

			rt2 = ball.velocity.mag();
			ball.velocity.unit();
			ball.velocity = TVector::unit( (normal*(2*normal.dot(-ball.velocity))) + ball.velocity );
			ball.velocity = ball.velocity*rt2;
			if(intTarRow != -1 && intTarColumn != -1)
				target[intTarRow][intTarColumn].active = FALSE;

			//Update explosion array
			for(int j = 0;j < 20; j++)
			{
				if (ExplosionArray[j].alpha <= 0)
				{
					ExplosionArray[j].alpha = 1;
					ExplosionArray[j].position = point;
					ExplosionArray[j].scale = 1;
					break;
				}
			}
		}
		else RestTime = 0;

	}

}

/*======================================================================================
             Init Variables                                                        
=======================================================================================*/
void InitVars()
{
	int planePos = PLANE_POS-BALL_RADIUS;

	//create planes
	planes[0].position = TVector(planePos,0,0);
	planes[0].normal = TVector(-1,0,0);
	planes[1].position = TVector(-1*planePos,0,0);
	planes[1].normal = TVector(1,0,0);
	planes[2].position = TVector(0,planePos*0.75,0);
	planes[2].normal = TVector(0,-1,0);
	planes[3].position = TVector(0,-1*planePos,0);
	planes[3].normal = TVector(0,1,0);

	//create quadratic object to render cylinders
	cylinder_obj= gluNewQuadric();
	gluQuadricTexture(cylinder_obj, GL_TRUE);

	//Set initial positions and velocities of balls
	//also initialize array which holds explosions
	double velocX = rand()%20;
	velocX -= 10;
	velocX /= 10;
	ball.velocity=TVector(velocX,-1,0);
	ball.position=TVector(0,70,0);

	paddle.position = TVector(0,PADDLE_INITY,0);
	paddle.active = TRUE;
	paddle.point[0]=TVector(paddle.position.X()-PADDLE_LENGTH,paddle.position.Y()-PADDLE_HEIGHT,0);
	paddle.point[1]=TVector(paddle.position.X()+PADDLE_LENGTH,paddle.position.Y()-PADDLE_HEIGHT,0);
	paddle.point[2]=TVector(paddle.position.X()+PADDLE_LENGTH,paddle.position.Y()+PADDLE_HEIGHT,0);
	paddle.point[3]=TVector(paddle.position.X()-PADDLE_LENGTH,paddle.position.Y()+PADDLE_HEIGHT,0);

	for (int i = 0; i < TARGET_ROW_COUNT; i++)
	{
		for (int j = 0; j  < TARGET_COLUMN_COUNT/2; j ++)
		{
			target[i][j].position=TVector((j-TARGET_COLUMN_COUNT/2+0.5)*(TARGET_LENGTH+TARGET_SPACE) ,100+i*(TARGET_HEIGHT+TARGET_SPACE) ,0);
			target[i][j].active=TRUE;
			target[i][j].point[0]=TVector(target[i][j].position.X()-TARGET_LENGTH/2,target[i][j].position.Y()-TARGET_HEIGHT/2,0);
			target[i][j].point[1]=TVector(target[i][j].position.X()+TARGET_LENGTH/2,target[i][j].position.Y()-TARGET_HEIGHT/2,0);
			target[i][j].point[2]=TVector(target[i][j].position.X()+TARGET_LENGTH/2,target[i][j].position.Y()+TARGET_HEIGHT/2,0);
			target[i][j].point[3]=TVector(target[i][j].position.X()-TARGET_LENGTH/2,target[i][j].position.Y()+TARGET_HEIGHT/2,0);

			target[i][TARGET_COLUMN_COUNT-j-1].position=TVector((TARGET_COLUMN_COUNT/2-j-0.5)*(TARGET_LENGTH+TARGET_SPACE) ,100+i*(TARGET_HEIGHT+TARGET_SPACE) ,0);
			target[i][TARGET_COLUMN_COUNT-j-1].active=TRUE;
			target[i][TARGET_COLUMN_COUNT-j-1].point[0]=TVector(target[i][TARGET_COLUMN_COUNT-j-1].position.X()-TARGET_LENGTH/2,target[i][TARGET_COLUMN_COUNT-j-1].position.Y()-TARGET_HEIGHT/2,0);
			target[i][TARGET_COLUMN_COUNT-j-1].point[1]=TVector(target[i][TARGET_COLUMN_COUNT-j-1].position.X()+TARGET_LENGTH/2,target[i][TARGET_COLUMN_COUNT-j-1].position.Y()-TARGET_HEIGHT/2,0);
			target[i][TARGET_COLUMN_COUNT-j-1].point[2]=TVector(target[i][TARGET_COLUMN_COUNT-j-1].position.X()+TARGET_LENGTH/2,target[i][TARGET_COLUMN_COUNT-j-1].position.Y()+TARGET_HEIGHT/2,0);
			target[i][TARGET_COLUMN_COUNT-j-1].point[3]=TVector(target[i][TARGET_COLUMN_COUNT-j-1].position.X()-TARGET_LENGTH/2,target[i][TARGET_COLUMN_COUNT-j-1].position.Y()+TARGET_HEIGHT/2,0);
		}
	}
	normal[0]=TVector(0,-1,0);
	normal[1]=TVector(1,0,0);
	normal[2]=TVector(0,1,0);
	normal[3]=TVector(-1,0,0);
	for (int i = 0; i < 20; i++)
	{
		ExplosionArray[i].alpha = 0;
		ExplosionArray[i].scale = 1;
	}
}


/*===============================================================
        Fast Intersection Function between ray/plane                         
=================================================================*/
int TestIntersectionPlane(const plane_t& plane,const TVector& position,const TVector& direction, double& lamda, TVector& pNormal)
{

	double DotProduct = direction.dot(plane.normal);
	double l2;

	//determine if ray paralle to plane
	if ((DotProduct<ZERO)&&(DotProduct>-ZERO)) 
		return 0;

	l2 = (plane.normal.dot(plane.position-position))/DotProduct;

	if (l2<-ZERO) 
		return 0;

	pNormal = plane.normal;
	lamda = l2;
	return 1;

}

int TestIntersectionTarget(const box_t& target, const TVector& position, const TVector& direction, double& lamda, TVector& pNormal)
{
	TVector result;
	double chosenL2=10000;
	int chosen=-1;
	double l2;
	for (int i = 0; i < 4; i++)
	{
		double DotProduct=direction.dot(normal[i]);

		//determine if ray paralle to plane
		if ((DotProduct<ZERO)&&(DotProduct>-ZERO)) 
			continue;

		l2=(normal[i].dot(target.point[i]-position))/DotProduct;

		if (l2<-ZERO)
			continue;
		TVector intersect = position + TVector::multiply(direction,l2,result);
		if (intersect.X()+BALL_RADIUS >= target.point[0].X() && intersect.Y()+BALL_RADIUS >= target.point[0].Y() &&
			intersect.X()-BALL_RADIUS <= target.point[1].X() && intersect.Y()+BALL_RADIUS >= target.point[1].Y() &&
			intersect.X()-BALL_RADIUS <= target.point[2].X() && intersect.Y()-BALL_RADIUS <= target.point[2].Y() &&
			intersect.X()+BALL_RADIUS >= target.point[3].X() && intersect.Y()-BALL_RADIUS <= target.point[3].Y())
		{
			if(l2 < chosenL2)
			{
				chosenL2 = l2;
				chosen	= i;
			}
		}
	}
	if(chosen != -1)
	{
		pNormal=normal[chosen];
		lamda=chosenL2;
		return 1;
	}
	else
		return 0;
}


/*====================================================================================
        Load Bitmaps And Convert To Textures                                  
=====================================================================================*/

void LoadGLTextures() {	

	Image *image1, *image2, *image3, *image4, *image5;

	image1 = new Image();
	if (image1 == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	image2 = new Image();
	if (image2 == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	image3 = new Image();
	if (image3 == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	image4 = new Image();
	if (image4 == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	image5 = new Image();
	if (image5 == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	if (!ImageLoad("Data/Marble.bmp", image1)) {
		exit(1);
	} 
	if (!ImageLoad("Data/Wand.bmp", image2)) {
		exit(1);
	}
	if (!ImageLoad("Data/Boden.bmp", image3)) {
		exit(1);
	} 
	if (!ImageLoad("Data/Glass.bmp", image4)) {
		exit(1);
	}
	if (!ImageLoad("Data/Spark.bmp", image5)) {
		exit(1);
	}
	// Create Texture image1	
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);   /* 2d texture (x and y size)*/

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); /* scale linearly when image bigger than texture*/
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); /* scale linearly when image smalled than texture*/
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);

	/* 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image, 
	border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.*/
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);

	// Create Texture image2	
	glGenTextures(1, &texture[1]);
	glBindTexture(GL_TEXTURE_2D, texture[1]);   

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, image2->sizeX, image2->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image2->data);


	// Create Texture	image3
	glGenTextures(1, &texture[2]);
	glBindTexture(GL_TEXTURE_2D, texture[2]);   

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, image3->sizeX, image3->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image3->data);

	// Create Texture image4
	glGenTextures(1, &texture[3]);
	glBindTexture(GL_TEXTURE_2D, texture[3]);   

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, image4->sizeX, image4->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image4->data);

	//Create Texture image5
	glGenTextures(1, &texture[4]);
	glBindTexture(GL_TEXTURE_2D, texture[4]);   

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, image5->sizeX, image5->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image5->data);

	delete image1;
	delete image2;
	delete image3;
	delete image4;
	delete image5;
}

BOOL ProcessKeys()
{

	if (keys[VK_LEFT] && paddle.position.X() > -1*PLANE_POS+PADDLE_LENGTH)
	{
		paddle.position += TVector(-2,0,0);
		paddle.point[0] = TVector(paddle.position.X()-PADDLE_LENGTH,paddle.position.Y()-PADDLE_HEIGHT,0);
		paddle.point[1] = TVector(paddle.position.X()+PADDLE_LENGTH,paddle.position.Y()-PADDLE_HEIGHT,0);
		paddle.point[2] = TVector(paddle.position.X()+PADDLE_LENGTH,paddle.position.Y()+PADDLE_HEIGHT,0);
		paddle.point[3] = TVector(paddle.position.X()-PADDLE_LENGTH,paddle.position.Y()+PADDLE_HEIGHT,0);
	}
	if (keys[VK_RIGHT]&& paddle.position.X() < PLANE_POS-PADDLE_LENGTH)
	{
		paddle.position += TVector(2,0,0);
		paddle.point[0] = TVector(paddle.position.X()-PADDLE_LENGTH,paddle.position.Y()-PADDLE_HEIGHT,0);
		paddle.point[1] = TVector(paddle.position.X()+PADDLE_LENGTH,paddle.position.Y()-PADDLE_HEIGHT,0);
		paddle.point[2] = TVector(paddle.position.X()+PADDLE_LENGTH,paddle.position.Y()+PADDLE_HEIGHT,0);
		paddle.point[3] = TVector(paddle.position.X()-PADDLE_LENGTH,paddle.position.Y()+PADDLE_HEIGHT,0);
	}
	if (keys[VK_F3]) 
	{   
		Time += 0.1;
		keys[VK_F3] = FALSE;
	}
	if (keys[VK_F2])
	{
		if(Time > 0.1)
			Time -= 0.1;
		keys[VK_F2] = FALSE;
	}
	if (keys[VK_F11])						
	{
		keys[VK_F11] = FALSE;					
		KillGLWindow();						
		fullscreen =! fullscreen;	
		// Recreate Our OpenGL Window
		if (!CreateGLWindow("Arkanoid",640,480,16,fullscreen))
			return 0;						
	}

	return 1;
}
