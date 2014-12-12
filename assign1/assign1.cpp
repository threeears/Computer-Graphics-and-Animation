// assign1.cpp : Defines the entry point for the console application.
//

/*
  CSCI 480 Computer Graphics
  Assignment 1: Height Fields
  C++ starter code
*/

#include "stdafx.h"
#include <pic.h>
#include <windows.h>
#include <stdlib.h>
#include <GL/glu.h>
#include <GL/glut.h>
char myFilenm[2048];
int g_iMenuId;

int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;

CONTROLSTATE g_ControlState = ROTATE;

/* state of the world */
float g_vLandRotate[3] = {0.0, 0.0, 0.0};
float g_vLandTranslate[3] = {0.0, 0.0, 0.0};
float g_vLandScale[3] = {1.0, 1.0, 1.0};

/* see <your pic directory>/pic.h for type Pic */
Pic * g_pHeightData;
unsigned char shape=GL_TRIANGLE_STRIP;
/* Write a screenshot to the specified filename */
void saveScreenshot (char *filename)
{
  int i, j;
  Pic *in = NULL;
  printf("%s", filename);
  if (filename == NULL)
    return;

  /* Allocate a picture buffer */
  in = pic_alloc(640, 480, 3, NULL);

  printf("File to save to: %s\n", filename);

  for (i=479; i>=0; i--) {
    glReadPixels(0, 479-i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
                 &in->pix[i*in->nx*in->bpp]);
  }

  if (jpeg_write(filename, in))
    printf("File saved Successfully\n");
  else
    printf("Error in Saving\n");

  pic_free(in);
}

void myinit()
{
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glShadeModel(GL_SMOOTH);
	/* setup gl view here */
  //glClearColor(0.0, 0.0, 0.0, 0.0);
   glutPostRedisplay();
}

void display()
{
  /* draw 1x1 cube about origin */
  /* replace this code with your height field implementation */
  /* you may also want to precede it with your 
rotation/translation/scaling */
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0,0,500,0,0,0,0,1,0);
	
		glTranslatef(g_vLandTranslate[0],g_vLandTranslate[1],g_vLandTranslate[2]);
		glRotatef(g_vLandRotate[0],1,0,0);
		glRotatef(g_vLandRotate[1],0,1,0);
		glRotatef(g_vLandRotate[2],0,0,1);
		glScalef(g_vLandScale[0],g_vLandScale[1],g_vLandScale[2]);

		GLfloat ambientColor[]={0.4,0.4,0.4,1};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
		GLfloat lightColor0[]={0.6,0.6,0.6,1};
		GLfloat lightPos0[]={-0.5,0.8,0.1,0};
		glLightfv(GL_LIGHT0,GL_DIFFUSE,lightColor0);
		glLightfv(GL_LIGHT0,GL_POSITION,lightPos0);


    int M_height=0;
	int L_height=256;
	for(int i=0;i<g_pHeightData->ny;i++)
		for(int j=0;j<g_pHeightData->nx;j++)
		{
			if(PIC_PIXEL(g_pHeightData, j, i, 0)>=M_height)
				M_height=PIC_PIXEL(g_pHeightData, j, i, 0);
			if(PIC_PIXEL(g_pHeightData, j, i, 0)<=L_height)
				L_height=PIC_PIXEL(g_pHeightData, j, i, 0);

		}

	//glColor3f(0.3,0,0);
	for(int i=0;i<g_pHeightData->ny-1;i++)
	{   int set=g_pHeightData->ny/2;
        glBegin(shape);
			for(int j=0;j<g_pHeightData->nx;j++)
			{
				float index1=(float)PIC_PIXEL(g_pHeightData, j, i, 0)/M_height;
				float index2=(float)PIC_PIXEL(g_pHeightData, j, i+1, 0)/M_height;
				
				glColor3f(0,index1,0);
				glVertex3f(j-set,PIC_PIXEL(g_pHeightData, j, i, 0)-set,i-set);
			    glColor3f(0,index2,0);
			    glVertex3f(j-set,PIC_PIXEL(g_pHeightData, j, i+1, 0)-set,i+1-set);
			 }

		glEnd();
	}
  glFlush();
  glEnable(GL_DEPTH_TEST);
  glutSwapBuffers();
  

}

void menufunc(int value)
{
  switch (value)
  {
    case 0:
      exit(0);
      break;
  }
}

void doIdle()
{
  /* do some stuff... */
	
	
  /* make the screen update */
  glutPostRedisplay();
}

void keyboard(unsigned char key,int x,int y)
{
if(key=='l') shape=GL_LINE;//0 stands for use Line
if(key=='t') shape=GL_TRIANGLE_STRIP;//0 stands for use triangle strip
if(key=='p') shape=GL_POINT;//0 stands for use Points
glutPostRedisplay();//标记当前窗口需要重新绘制。通过glutMainLoop下一次循环时，窗口显示将被回调以重新显示窗口的正常面板。
}


/* converts mouse drags into information about 
rotation/translation/scaling */
void mousedrag(int x, int y)
{
  int vMouseDelta[2] = {x-g_vMousePos[0], y-g_vMousePos[1]};//x - mouseposition;y- mouseposition why??

  switch (g_ControlState)//choose the way you transform the image
  {
    case TRANSLATE:  
      if (g_iLeftMouseButton)
      {
        g_vLandTranslate[0] += vMouseDelta[0]*0.1;
        g_vLandTranslate[1] -= vMouseDelta[1]*0.1;
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandTranslate[2] += vMouseDelta[1]*0.1;
      }
      break;
    case ROTATE:
      if (g_iLeftMouseButton)
      {
        g_vLandRotate[0] += vMouseDelta[1];
        g_vLandRotate[1] += vMouseDelta[0];
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandRotate[2] += vMouseDelta[1];
      }
      break;
    case SCALE:
      if (g_iLeftMouseButton)
      {
        g_vLandScale[0] *= 1.0+vMouseDelta[0]*0.01;
        g_vLandScale[1] *= 1.0-vMouseDelta[1]*0.01;
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandScale[2] *= 1.0-vMouseDelta[1]*0.01;
      }
      break;
  }
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;//new xy coordinates 
  
  glutPostRedisplay();
}

void mouseidle(int x, int y)
{
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
  
}
int i=0;
void mousebutton(int button, int state, int x, int y)//to deal with mouse button and modifier key
{
	char k=0;
  switch (button)
  {
    case GLUT_LEFT_BUTTON:
		{
		 g_iLeftMouseButton = (state==GLUT_DOWN);
		 sprintf(myFilenm, "anim.%04d.jpg", i);
		 saveScreenshot(myFilenm);
      break;
		}
	case GLUT_MIDDLE_BUTTON:
      g_iMiddleMouseButton = (state==GLUT_DOWN);
	  sprintf(myFilenm, "anim.%04d.jpg", i);
		 saveScreenshot(myFilenm);
      break;
    case GLUT_RIGHT_BUTTON:
      g_iRightMouseButton = (state==GLUT_DOWN);
	  sprintf(myFilenm, "anim.%04d.jpg", i);
	 saveScreenshot(myFilenm);
      break;
  }
	
  switch(glutGetModifiers())//组合键modifier key
  {
    case GLUT_ACTIVE_CTRL://返回它，当按下control
      g_ControlState = TRANSLATE;
      break;
    case GLUT_ACTIVE_SHIFT:
      g_ControlState = SCALE;//press shift turn to scale
      break;
    default:
      g_ControlState = ROTATE;//else rotate
      break;
  }
  i++;
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// I've set the argv[1] to spiral.jpg.
	// To change it, on the "Solution Explorer",
	// right click "assign1", choose "Properties",
	// go to "Configuration Properties", click "Debugging",
	// then type your texture name for the "Command Arguments"
	if (argc<2)
	{  
		printf ("usage: %s heightfield.jpg\n", argv[0]);
		exit(1);
	}

	g_pHeightData = jpeg_read((char*)argv[1], NULL);//读文件
	if (!g_pHeightData)
	{
	    printf ("error reading %s.\n", argv[1]);
	    exit(1);
	}
	else{  printf("%d",PIC_PIXEL(g_pHeightData, 0,128,0));}
	glutInit(&argc,(char**)argv);
  
	/*
		create a window here..should be double buffered and use depth testing
  
	    the code past here will segfault if you don't have a window set up....
	    replace the exit once you add those calls.
	*/
	//exit(0);

	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(640,480);
	glutCreateWindow("My OpenGL");
	
	glClearColor(1,1,1,1);
	printf("hello");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(45,1,0.1,1000);
	glMatrixMode(GL_MODELVIEW);


	/* tells glut to use a particular display function to redraw */
	glutDisplayFunc(display);
  
	/* allow the user to quit using the right mouse button menu */
	g_iMenuId = glutCreateMenu(menufunc);//创建一个弹出式菜单，指定menufunc为菜单回调函数，该函数包含一个参数
	glutSetMenu(g_iMenuId);//在有了这个标识符后，可以用函数glutSetMenu（nMenu）指定对应的菜单为当前的菜单；
	glutAddMenuEntry("Quit",0);//使用函数中菜单中加入菜单项 名称quit, 对应ID ：0
	glutAttachMenu(GLUT_RIGHT_BUTTON);//将菜单项和鼠标右键相关联
  
	/* replace with any animate code */
	glutIdleFunc(doIdle);

	/* callback for mouse drags */
	glutMotionFunc(mousedrag);
	/* callback for idle mouse movement */
	glutPassiveMotionFunc(mouseidle);
	/* callback for mouse button changes */
	glutMouseFunc(mousebutton);
	glutKeyboardFunc(keyboard);
	/* do initialization */
//	myinit();
	glEnable(GL_DEPTH_TEST);
	
	glutMainLoop();
	
	return 0;
}