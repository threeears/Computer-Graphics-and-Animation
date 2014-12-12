// assign2.cpp : Defines the entry point for the console application.
//

/*
	CSCI 480 Computer Graphics
	Assignment 2: Simulating a Roller Coaster
	C++ starter code
*/

#include "stdafx.h"
#include <pic.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
using namespace std;

char myFilenm[2048];



/* represents one control point along the spline */
struct point {
	double x;
	double y;
	double z;

};

/* spline struct which contains how many control points, and an array of control points */
struct spline {
	int numControlPoints;
	struct point *points;
};

/* the spline array */
struct spline *g_Splines;

/* total number of splines */
int g_iNumOfSplines;

//expanded control points and its' numbers
point control_point[50]={0};
int   control_point_num=0;
float M[4][4]={{-0.5,1.5,-1.5,0.5},{1.0,-2.5,2.0,-0.5},{-0.5,0,0.5,0},{0.0,1.0,0.0,0.0}};

/* all the points on curve*/
point c_point[2500]={0};//curve points and its' number
int c_num=0;
point dc_point[2500]={0};// the other curve of track

/*all the direction information on curve, tangent, normal binoral arrays*/
point normal_point[3500];
point tangent_point[3500];
point binormal_point[3500];

/*defination related to transformations*/
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


int i,j;
Pic* g_pHeightData;
static GLuint texture[10];
static GLubyte checkImage[256][256][4];//self defined, change later

/*camera control variables*/
int previousTime=0;
int currentTime=0;
int frameCount=0;
float fps=0.0;
int p=0;

int fn=0;

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



void calculateFPS()
{
    //  Increase frame count
    frameCount++;

    //  Get the number of milliseconds since glutInit called 
    
    currentTime = glutGet(GLUT_ELAPSED_TIME);

    //  Calculate time passed
    int timeInterval = currentTime - previousTime;
	
    if(timeInterval > 120)
    {
        //  calculate the number of frames per second
        fps = frameCount / (timeInterval / 1000.0f);
		
		p=p+1;
		/*if(currentTime>18000)//store screen shorts
		{
		 sprintf(myFilenm, "%03d.jpg", fn);
		 saveScreenshot(myFilenm);
		 fn++;
		}*/
        //  Set time
        previousTime = currentTime;

        //  Reset frame count
        frameCount = 0;
    }
}


void SetupLight()//add lighting
{
	GLfloat amb[]={0.5,0.5,0.5,0.0};
	GLfloat dif[]={6,6,6,0.0};
	GLfloat spe[]={1.0,1.0,1.0,0.0};
	GLfloat pos[]={50.0,50.0,50.0,0.0};
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_SMOOTH);

	
	glLightfv(GL_LIGHT0,GL_AMBIENT,amb);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,dif);
	glLightfv(GL_LIGHT0,GL_SPECULAR,spe);
	glEnable(GL_LIGHT0);
	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,spe);
	glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,128);



}


void init(void)//initial the texture
{
	glClearColor(0.0,0.0,0.0,0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

	/*first texture load*/
	g_pHeightData = jpeg_read((char*)"back.jpg", NULL);//读文件
	if (!g_pHeightData)
	{
	    printf ("error reading %s.\n", "back.jpg");
	    exit(1);
	}
	else{  
	//printf("%d",PIC_PIXEL(g_pHeightData, 0,128,0));
	//cout<<g_pHeightData->nx<<","<<g_pHeightData->ny<<","<<g_pHeightData->bpp<<endl;
	
	}

	

	for( i=0;i<g_pHeightData->ny;i++)
	   for( j=0;j<g_pHeightData->nx;j++)
	   {
		   checkImage[i][j][0]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 0);
		   checkImage[i][j][1]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 1);
		   checkImage[i][j][2]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 2);
		   checkImage[i][j][3]=(GLubyte) 255;
		 // cout<<(int)checkImage[i][j][0]<<","<<(int)checkImage[i][j][1]<<","<<(int)checkImage[i][j][2]<<","<<(int)checkImage[i][j][3]<<endl;
	   }



    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1,&texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA,g_pHeightData->nx,g_pHeightData->ny, 0,GL_RGBA,GL_UNSIGNED_BYTE, checkImage);//change w and h later



/*second texture load*/
	g_pHeightData = jpeg_read((char*)"bottom.jpg", NULL);//读文件
	if (!g_pHeightData)
	{
	    printf ("error reading %s.\n", "bottom.jpg");
	    exit(1);
	}
	else{  
	//printf("%d",PIC_PIXEL(g_pHeightData, 0,128,0));
//	cout<<g_pHeightData->nx<<","<<g_pHeightData->ny<<","<<g_pHeightData->bpp<<endl;
	
	}

	

	for( i=0;i<g_pHeightData->ny;i++)
	   for( j=0;j<g_pHeightData->nx;j++)
	   {
		   checkImage[i][j][0]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 0);
		   checkImage[i][j][1]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 1);
		   checkImage[i][j][2]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 2);
		   checkImage[i][j][3]=(GLubyte) 255;
		 // cout<<(int)checkImage[i][j][0]<<","<<(int)checkImage[i][j][1]<<","<<(int)checkImage[i][j][2]<<","<<(int)checkImage[i][j][3]<<endl;
	   }



    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1,&texture[1]);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA,g_pHeightData->nx,g_pHeightData->ny, 0,GL_RGBA,GL_UNSIGNED_BYTE, checkImage);//change w and h later


	//third texture
	g_pHeightData = jpeg_read((char*)"right.jpg", NULL);//读文件
	if (!g_pHeightData)
	{
	    printf ("error reading %s.\n", "right.jpg");
	    exit(1);
	}
	else{  
	//printf("%d",PIC_PIXEL(g_pHeightData, 0,128,0));
	//cout<<g_pHeightData->nx<<","<<g_pHeightData->ny<<","<<g_pHeightData->bpp<<endl;
	
	}

	

	for( i=0;i<g_pHeightData->ny;i++)
	   for( j=0;j<g_pHeightData->nx;j++)
	   {
		   checkImage[i][j][0]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 0);
		   checkImage[i][j][1]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 1);
		   checkImage[i][j][2]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 2);
		   checkImage[i][j][3]=(GLubyte) 255;
		 // cout<<(int)checkImage[i][j][0]<<","<<(int)checkImage[i][j][1]<<","<<(int)checkImage[i][j][2]<<","<<(int)checkImage[i][j][3]<<endl;
	   }



    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1,&texture[2]);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA,g_pHeightData->nx,g_pHeightData->ny, 0,GL_RGBA,GL_UNSIGNED_BYTE, checkImage);//change w and h later




	//fourth texture
	g_pHeightData = jpeg_read((char*)"left.jpg", NULL);//读文件
	if (!g_pHeightData)
	{
	    printf ("error reading %s.\n", "left.jpg");
	    exit(1);
	}
	else{  
	//printf("%d",PIC_PIXEL(g_pHeightData, 0,128,0));
//	cout<<g_pHeightData->nx<<","<<g_pHeightData->ny<<","<<g_pHeightData->bpp<<endl;
	
	}

	

	for( i=0;i<g_pHeightData->ny;i++)
	   for( j=0;j<g_pHeightData->nx;j++)
	   {
		   checkImage[i][j][0]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 0);
		   checkImage[i][j][1]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 1);
		   checkImage[i][j][2]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 2);
		   checkImage[i][j][3]=(GLubyte) 255;
		 // cout<<(int)checkImage[i][j][0]<<","<<(int)checkImage[i][j][1]<<","<<(int)checkImage[i][j][2]<<","<<(int)checkImage[i][j][3]<<endl;
	   }



    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1,&texture[3]);
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA,g_pHeightData->nx,g_pHeightData->ny, 0,GL_RGBA,GL_UNSIGNED_BYTE, checkImage);//change w and h later


	/*fifth texture load*/
	g_pHeightData = jpeg_read((char*)"top.jpg", NULL);//读文件
	if (!g_pHeightData)
	{
	    printf ("error reading %s.\n", "top.jpg");
	    exit(1);
	}
	else{  
	//printf("%d",PIC_PIXEL(g_pHeightData, 0,128,0));
	//cout<<g_pHeightData->nx<<","<<g_pHeightData->ny<<","<<g_pHeightData->bpp<<endl;
	
	}

	

	for( i=0;i<g_pHeightData->ny;i++)
	   for( j=0;j<g_pHeightData->nx;j++)
	   {
		   checkImage[i][j][0]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 0);
		   checkImage[i][j][1]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 1);
		   checkImage[i][j][2]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 2);
		   checkImage[i][j][3]=(GLubyte) 255;
		 // cout<<(int)checkImage[i][j][0]<<","<<(int)checkImage[i][j][1]<<","<<(int)checkImage[i][j][2]<<","<<(int)checkImage[i][j][3]<<endl;
	   }



    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1,&texture[4]);
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA,g_pHeightData->nx,g_pHeightData->ny, 0,GL_RGBA,GL_UNSIGNED_BYTE, checkImage);//change w and h later




	/*sixth texture load*/
	g_pHeightData = jpeg_read((char*)"center.jpg", NULL);//读文件
	if (!g_pHeightData)
	{
	    printf ("error reading %s.\n", "center.jpg");
	    exit(1);
	}
	else{  
	//printf("%d",PIC_PIXEL(g_pHeightData, 0,128,0));
	//cout<<g_pHeightData->nx<<","<<g_pHeightData->ny<<","<<g_pHeightData->bpp<<endl;
	
	}

	

	for( i=0;i<g_pHeightData->ny;i++)
	   for( j=0;j<g_pHeightData->nx;j++)
	   {
		   checkImage[i][j][0]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 0);
		   checkImage[i][j][1]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 1);
		   checkImage[i][j][2]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 2);
		   checkImage[i][j][3]=(GLubyte) 255;
		 // cout<<(int)checkImage[i][j][0]<<","<<(int)checkImage[i][j][1]<<","<<(int)checkImage[i][j][2]<<","<<(int)checkImage[i][j][3]<<endl;
	   }



    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1,&texture[5]);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA,g_pHeightData->nx,g_pHeightData->ny, 0,GL_RGBA,GL_UNSIGNED_BYTE, checkImage);//change w and h later





	/*seventh texture load*/
	g_pHeightData = jpeg_read((char*)"wood .jpg", NULL);//读文件
	if (!g_pHeightData)
	{
	    printf ("error reading %s.\n", "wood .jpg");
	    exit(1);
	}
	else{  
	//printf("%d",PIC_PIXEL(g_pHeightData, 0,128,0));
	//cout<<g_pHeightData->nx<<","<<g_pHeightData->ny<<","<<g_pHeightData->bpp<<endl;
	
	}

	

	for( i=0;i<g_pHeightData->ny;i++)
	   for( j=0;j<g_pHeightData->nx;j++)
	   {
		   checkImage[i][j][0]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 0);
		   checkImage[i][j][1]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 1);
		   checkImage[i][j][2]=(GLubyte) PIC_PIXEL(g_pHeightData, j, i, 2);
		   checkImage[i][j][3]=(GLubyte) 255;
		 // cout<<(int)checkImage[i][j][0]<<","<<(int)checkImage[i][j][1]<<","<<(int)checkImage[i][j][2]<<","<<(int)checkImage[i][j][3]<<endl;
	   }



    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1,&texture[6]);
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA,g_pHeightData->nx,g_pHeightData->ny, 0,GL_RGBA,GL_UNSIGNED_BYTE, checkImage);//change w and h later



}



void compute_Matrix()
{
	int i=0;
	int j=0;
	float u=0.0;
	float CM[4][3];//create a new control matrix each time

	control_point[control_point_num].x=g_Splines[0].points[0].x;		// recompute control points adding first and end point
	control_point[control_point_num].y=g_Splines[0].points[0].y;
	control_point[control_point_num].z=g_Splines[0].points[0].z;
	control_point_num++;

	for(i=0;i<g_iNumOfSplines;i++)
	{
		for(j=0;j<g_Splines[i].numControlPoints;j++)
		{
			control_point[control_point_num].x=g_Splines[i].points[j].x;
			control_point[control_point_num].y=g_Splines[i].points[j].y;
			control_point[control_point_num].z=g_Splines[i].points[j].z;
			control_point_num++;

			
		}
	}
	control_point[control_point_num].x=g_Splines[0].points[g_Splines[0].numControlPoints-1].x;
	control_point[control_point_num].y=g_Splines[0].points[g_Splines[0].numControlPoints-1].y;
	control_point[control_point_num].z=g_Splines[0].points[g_Splines[0].numControlPoints-1].z;
		
	control_point_num++;
	/*for(int i=0;i<control_point_num;i++)//new control points
	{
		cout<<control_point[i].x<<","<<control_point[i].y<<","<<control_point[i].z<<endl;
	}*/
	

	i=0;
	while (i<control_point_num-3)				//every time find four points,,compute curve points
	{
		int num=i;

			for (j=0;j<4;j++)//create the control point matrix
			{	
				    CM[j][0]=control_point[num].x;
					CM[j][1]=control_point[num].y;
					CM[j][2]=control_point[num].z;
					//cout<<CM[j][0]<<","<<CM[j][1]<<","<<CM[j][2]<<endl;
					num++;	
			}
			//cout<<endl;
			i++;
			u=0.0;
			while(u<=1.0)
			{
					double a,b,c,d,ta,tb,tc,td;
					a=u*u*u*M[0][0]+u*u*M[1][0]+u*M[2][0]+M[3][0];
					b=u*u*u*M[0][1]+u*u*M[1][1]+u*M[2][1]+M[3][1];
					c=u*u*u*M[0][2]+u*u*M[1][2]+u*M[2][2]+M[3][2];
					d=u*u*u*M[0][3]+u*u*M[1][3]+u*M[2][3]+M[3][3];
					
					c_point[c_num].x=a*CM[0][0]+b*CM[1][0]+c*CM[2][0]+d*CM[3][0];
					c_point[c_num].y=a*CM[0][1]+b*CM[1][1]+c*CM[2][1]+d*CM[3][1];
					c_point[c_num].z=a*CM[0][2]+b*CM[1][2]+c*CM[2][2]+d*CM[3][2];
					//cout<<c_point[c_num].x<<","<<c_point[c_num].y<<","<<c_point[c_num].z<<endl;//x, y ,z of points on spline
					
					//store the tangent of each vertex
					ta=3*u*u*M[0][0]+2*u*M[1][0]+M[2][0];
					tb=3*u*u*M[0][1]+2*u*M[1][1]+M[2][1];
					tc=3*u*u*M[0][2]+2*u*M[1][2]+M[2][2];
					td=3*u*u*M[0][3]+2*u*M[1][3]+M[2][3];
					
					tangent_point[c_num].x=ta*CM[0][0]+tb*CM[1][0]+tc*CM[2][0]+td*CM[3][0];
					tangent_point[c_num].y=ta*CM[0][1]+tb*CM[1][1]+tc*CM[2][1]+td*CM[3][1];
					tangent_point[c_num].z=ta*CM[0][2]+tb*CM[1][2]+tc*CM[2][2]+td*CM[3][2];

					c_num++;
					u=u+0.01;

		}
			//cout<<c_num<<endl;
    }
	/*for(int i=0;i<c_num;i++)
		{
			
			cout<<c_point[i].x<<","<<c_point[i].y<<","<<c_point[i].z<<endl;
		}*/


}


void build_direction()//calculate unit tangent, binormal and normal
{
	
	//calculate unit tangent
	double base=sqrt(tangent_point[0].x*tangent_point[0].x+tangent_point[0].y*tangent_point[0].y+tangent_point[0].z*tangent_point[0].z);
	tangent_point[0].x=tangent_point[0].x/base;
	tangent_point[0].y=tangent_point[0].y/base;
	tangent_point[0].z=tangent_point[0].z/base;//unit t

	point start_v;
	start_v.x=0;
	start_v.y=0;
	start_v.z=1;
	
	/*build the first normal vertex and binormal vertex*/
	normal_point[0].x=tangent_point[0].y*start_v.z-tangent_point[0].z*start_v.y;
	normal_point[0].y=tangent_point[0].z*start_v.x-tangent_point[0].x*start_v.z;
	normal_point[0].z=tangent_point[0].x*start_v.y-tangent_point[0].y*start_v.x;//first normal point T*V
	//calculate unit normal_point
	base=sqrt(normal_point[0].x*normal_point[0].x+normal_point[0].y*normal_point[0].y+normal_point[0].z*normal_point[0].z);
	normal_point[0].x=normal_point[0].x/base;
	normal_point[0].y=normal_point[0].y/base;
	normal_point[0].z=normal_point[0].z/base;//unit N

	binormal_point[0].x=tangent_point[0].y*normal_point[0].z-tangent_point[0].z*normal_point[0].y;//T*N
	binormal_point[0].y=tangent_point[0].z*normal_point[0].x-tangent_point[0].x*normal_point[0].z;
	binormal_point[0].z=tangent_point[0].x*normal_point[0].y-tangent_point[0].y*normal_point[0].x;
	
	//calculate unit binormal
	base=sqrt(binormal_point[0].x*binormal_point[0].x+binormal_point[0].y*binormal_point[0].y+binormal_point[0].z*binormal_point[0].z);
	binormal_point[0].x=binormal_point[0].x/base;
	binormal_point[0].y=binormal_point[0].y/base;
	binormal_point[0].z=binormal_point[0].z/base;//unit B

	//calculate the reat points of  T N B
	for(int i=1;i<=c_num;i++)
	{
		//Ni=Bi-1 * Ti
		normal_point[i].x=binormal_point[i-1].y*tangent_point[i].z-binormal_point[i-1].z*tangent_point[i].y;
		normal_point[i].y=binormal_point[i-1].z*tangent_point[i].x-binormal_point[i-1].x*tangent_point[i].z;
		normal_point[i].z=binormal_point[i-1].x*tangent_point[i].y-binormal_point[i-1].y*tangent_point[i].x;
		//unify normal 
		base=sqrt(normal_point[i].x*normal_point[i].x+normal_point[i].y*normal_point[i].y+normal_point[i].z*normal_point[i].z);
		normal_point[i].x=normal_point[i].x/base;
		normal_point[i].y=normal_point[i].y/base;
		normal_point[i].z=normal_point[i].z/base;//unit N

		//Bi=Ti * Ni
		binormal_point[i].x=tangent_point[i].y*normal_point[i].z-tangent_point[i].z*normal_point[i].y;//T*N
		binormal_point[i].y=tangent_point[i].z*normal_point[i].x-tangent_point[i].x*normal_point[i].z;
		binormal_point[i].z=tangent_point[i].x*normal_point[i].y-tangent_point[i].y*normal_point[i].x;
	
		//calculate unit binormal
		base=sqrt(binormal_point[i].x*binormal_point[i].x+binormal_point[i].y*binormal_point[i].y+binormal_point[i].z*binormal_point[i].z);
		binormal_point[i].x=binormal_point[i].x/base;
		binormal_point[i].y=binormal_point[i].y/base;
		binormal_point[i].z=binormal_point[i].z/base;//unit B

	}

}


void build_trail()
{
	double s=0.3;
	/*build another curve*/
	for(int i=0;i<c_num;i++)
	{
		dc_point[i].x=c_point[i].x+s*normal_point[i].x;
		dc_point[i].y=c_point[i].y+s*normal_point[i].y;
		dc_point[i].z=c_point[i].z+s*normal_point[i].z;
	
	}
	
}

void mousedrag(int x, int y)
{
  int vMouseDelta[2] = {x-g_vMousePos[0], y-g_vMousePos[1]};//x - mouseposition;y- mouseposition why??

  switch (g_ControlState)//choose the way you transform the image
  {
    case TRANSLATE:  
      if (g_iLeftMouseButton)
      {
        g_vLandTranslate[0] += vMouseDelta[0]*0.01;
        g_vLandTranslate[1] -= vMouseDelta[1]*0.1;
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandTranslate[2] += vMouseDelta[1]*0.01;
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
  g_vMousePos[1] = y;
  
  glutPostRedisplay();
}



void display()
{
	int i=0;
	int s=0.001;
	int t=0.000000001;

	//initials
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	SetupLight();
	//set camera
	gluLookAt((c_point[p].x+dc_point[p].x)/2+0.2*binormal_point[p].x,(c_point[p].y+dc_point[p].y)/2+0.2*binormal_point[p].y,(c_point[p].z+dc_point[p].z)/2+0.2*binormal_point[p].z,(c_point[p].x+dc_point[p].x)/2+tangent_point[p].x, ((c_point[p].y+dc_point[p].y)/2+tangent_point[p].y),( (c_point[p].z+dc_point[p].z)/2+tangent_point[p].z),binormal_point[p].x,binormal_point[p].y,binormal_point[p].z);
	
	//translations
	glTranslatef(g_vLandTranslate[0],g_vLandTranslate[1],g_vLandTranslate[2]);
	glRotatef(g_vLandRotate[0],1,0,0);
	glRotatef(g_vLandRotate[1],0,1,0);
	glRotatef(g_vLandRotate[2],0,0,1);
	glScalef(g_vLandScale[0],g_vLandScale[1],g_vLandScale[2]);

	//textures 0 back
			glEnable(GL_TEXTURE_2D);//draw the textures
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glBindTexture(GL_TEXTURE_2D, texture[0]);
			glBegin(GL_QUADS);


				glTexCoord2f(0.0, 0.0);  glVertex3f(-30.0, 90.0, -50.0);
				glTexCoord2f(0.0, 1.0); glVertex3f(-30.0, 90.0, 50.0);
				glTexCoord2f(1.0, 1.0); glVertex3f(-30.0, -10.0, 50.0);
				glTexCoord2f(1.0, 0.0);	glVertex3f(-30.0, -10.0, -50.0);//at the left of scence


			glEnd();
			glDisable(GL_TEXTURE_2D);





//textures 1 bottom
			glEnable(GL_TEXTURE_2D);//draw the textures
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glBindTexture(GL_TEXTURE_2D, texture[1]);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0, 0.0); glVertex3f(70.0, -10.0, 50.0);
				glTexCoord2f(0.0, 1.0);glVertex3f(-30.0, -10.0, 50.0);
				glTexCoord2f(1.0, 1.0); glVertex3f(-30.0, 90.0, 50.0);
				glTexCoord2f(1.0, 0.0);  glVertex3f(70.0, 90.0,50.0);

				
			glEnd();
			glDisable(GL_TEXTURE_2D);




//textrue2 right
			glEnable(GL_TEXTURE_2D);//draw the textures
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glBindTexture(GL_TEXTURE_2D, texture[2]);
			glBegin(GL_QUADS);
			
			
				glTexCoord2f(0.0, 0.0);  glVertex3f(70.0, 90.0, -50.0);
				glTexCoord2f(0.0, 1.0);glVertex3f(70.0, 90.0, 50.0);
				glTexCoord2f(1.0, 1.0); glVertex3f(-30.0, 90.0, 50.0);
				glTexCoord2f(1.0, 0.0); glVertex3f(-30.0, 90.0, -50.0);
			glEnd();
			glDisable(GL_TEXTURE_2D);

//textrue4 left
			glEnable(GL_TEXTURE_2D);//draw the textures
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glBindTexture(GL_TEXTURE_2D, texture[3]);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0);  glVertex3f(-30.0, -10.0, -50.0);
				glTexCoord2f(0.0, 1.0);glVertex3f(-30.0, -10.0, 50.0);
				glTexCoord2f(1.0, 1.0); glVertex3f(70.0, -10.0, 50.0);
				glTexCoord2f(1.0, 0.0); glVertex3f(70.0, -10.0, -50.0);	

			glEnd();
			glDisable(GL_TEXTURE_2D);






//textures 5 top
			glEnable(GL_TEXTURE_2D);//draw the textures
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glBindTexture(GL_TEXTURE_2D, texture[4]);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0, 0.0);  glVertex3f(-30.0, -10.0, -50.0);
				glTexCoord2f(0.0, 1.0);glVertex3f(70.0, -10.0, -50.0);
				glTexCoord2f(1.0, 1.0); glVertex3f(70.0, 90.0, -50.0);
				glTexCoord2f(1.0, 0.0); glVertex3f(-30.0, 90.0, -50.0);
				
			glEnd();
			glDisable(GL_TEXTURE_2D);






//textures 6 center
			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glBindTexture(GL_TEXTURE_2D, texture[5]);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0, 0.0);  glVertex3f(70.0, -10.0, -50.0);
				glTexCoord2f(0.0, 1.0);glVertex3f(70.0, -10.0, 50.0);
				glTexCoord2f(1.0, 1.0); glVertex3f(70.0, 90.0, 50.0);
				glTexCoord2f(1.0, 0.0); glVertex3f(70.0, 90.0, -50.0);
				
			glEnd();
			glDisable(GL_TEXTURE_2D);



//textrue3 for the tracks
			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glBindTexture(GL_TEXTURE_2D, texture[6]);

			 i=0;
			while(i<c_num-5)
		{
			glBegin(GL_QUADS);
				
			glTexCoord2f(0.0, 0.0);  glVertex3f(c_point[i].x, c_point[i].y, c_point[i].z);
			glTexCoord2f(0.0,0.5);  glVertex3f(c_point[i+2].x, c_point[i+2].y, c_point[i+2].z);
			glTexCoord2f(1.0, 0.5);  glVertex3f(dc_point[i+2].x, dc_point[i+2].y, dc_point[i+2].z);
			glTexCoord2f(1.0, 0.0);  glVertex3f(dc_point[i].x, dc_point[i].y, dc_point[i].z);

			glEnd();
			i=i+10;
		}
			glDisable(GL_TEXTURE_2D);

//draw the right curve
	glColor3f(0.5,0.5,0);
	glLineWidth(3.0f);

	for(int j=0;j<c_num-1;j++)
		{
			glBegin(GL_LINES);
			{
		    int i=j;
			glVertex3f(c_point[i].x,c_point[i].y,c_point[i].z);
			glVertex3f(c_point[i+1].x,c_point[i+1].y,c_point[i+1].z);
			//cout<<c_point[i].x<<","<<c_point[i].y<<","<<c_point[i].z<<endl;
			 }
		   glEnd();


		}


// another curve		
	glColor3f(0.5,0.5,0);
	for(int j=0;j<c_num-1;j++)//draw the left curve
		{
			glBegin(GL_LINES);
			{
		    int i=j;
			glVertex3f(dc_point[i].x,dc_point[i].y,dc_point[i].z);
			glVertex3f(dc_point[i+1].x,dc_point[i+1].y,dc_point[i+1].z);
			 }
		glEnd();
		}




/*draw the tracks between curves This is deleted because I use texture dircectly as the tracks*/ 
/*	glColor3f(0,0,0);
	 i=0;
	
	while(i<c_num)
	{
			glBegin(GL_LINES);
			{
				glVertex3f(c_point[i].x,c_point[i].y,c_point[i].z);
				glVertex3f(dc_point[i].x,dc_point[i].y,dc_point[i].z);

				glVertex3f(c_point[i].x+s*tangent_point[i].x,c_point[i].y+s*tangent_point[i].y,c_point[i].z+s*tangent_point[i].z);
				glVertex3f(dc_point[i].x+s*tangent_point[i].x,dc_point[i].y+s*tangent_point[i].y,dc_point[i].z+s*tangent_point[i].z);

				glVertex3f(c_point[i].x+s*binormal_point[i].x,c_point[i].y+s*binormal_point[i].y,c_point[i].z+s*binormal_point[i].z);
				glVertex3f(dc_point[i].x+s*binormal_point[i].x,dc_point[i].y+s*binormal_point[i].y,dc_point[i].z+s*binormal_point[i].z);

				glVertex3f(c_point[i].x+s*tangent_point[i].x+t*binormal_point[i].x,c_point[i].y+s*tangent_point[i].y+t*binormal_point[i].y,c_point[i].z+s*tangent_point[i].z+t*binormal_point[i].z);
				glVertex3f(dc_point[i].x+s*tangent_point[i].x+t*binormal_point[i].x,dc_point[i].y+s*tangent_point[i].y+t*binormal_point[i].y,dc_point[i].z+s*tangent_point[i].z+t*binormal_point[i].z);



			}
			glEnd();

		i=i+10;//draw track every 20 pixcels
	}*/


  glEnable(GL_DEPTH_TEST);
  glutSwapBuffers();
}

int loadSplines(char *argv) {
	char *cName = (char *)malloc(128 * sizeof(char));
	FILE *fileList;
	FILE *fileSpline;
	int iType, i = 0, j, iLength;

	/* load the track file */
	fileList = fopen(argv, "r");
	if (fileList == NULL) {
		printf ("can't open file\n");
		exit(1);
	}
  
	/* stores the number of splines in a global variable */
	fscanf(fileList, "%d", &g_iNumOfSplines);

	g_Splines = (struct spline *)malloc(g_iNumOfSplines * sizeof(struct spline));
	
	/* reads through the spline files */
	for (j = 0; j < g_iNumOfSplines; j++) {
		i = 0;
		fscanf(fileList, "%s", cName);
		fileSpline = fopen(cName, "r");

		if (fileSpline == NULL) {
			printf ("can't open file\n");
			exit(1);
		}

		/* gets length for spline file */
		fscanf(fileSpline, "%d %d", &iLength, &iType);

		/* allocate memory for all the points */
		g_Splines[j].points = (struct point *)malloc(iLength * sizeof(struct point));
		g_Splines[j].numControlPoints = iLength;

		/* saves the data to the struct */
		while (fscanf(fileSpline, "%lf %lf %lf", 
			&g_Splines[j].points[i].x, 
			&g_Splines[j].points[i].y, 
			&g_Splines[j].points[i].z) != EOF) {
			i++;
		}
	}

	free(cName);

	return 0;
}




void mousebutton(int button, int state, int x, int y)//to deal with mouse button and modifier key
{
	char k=0;
  switch (button)
  {
    case GLUT_LEFT_BUTTON:
		{
		 g_iLeftMouseButton = (state==GLUT_DOWN);
	///	 sprintf(myFilenm, "anim.%04d.jpg", i);
	//	 saveScreenshot(myFilenm);
      break;
		}
	case GLUT_MIDDLE_BUTTON:
      g_iMiddleMouseButton = (state==GLUT_DOWN);
	//  sprintf(myFilenm, "anim.%04d.jpg", i);
	//	 saveScreenshot(myFilenm);
      break;
    case GLUT_RIGHT_BUTTON:
      g_iRightMouseButton = (state==GLUT_DOWN);
	//  sprintf(myFilenm, "anim.%04d.jpg", i);
	// saveScreenshot(myFilenm);
      break;
  }
	
  switch(glutGetModifiers())
  {
    case GLUT_ACTIVE_CTRL:
      g_ControlState = TRANSLATE;
      break;
    case GLUT_ACTIVE_SHIFT:
      g_ControlState = SCALE;
      break;
    default:
      g_ControlState = ROTATE;  
      break;
  }
  //i++;
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}




void mouseidle(int x, int y)
{
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
  
}



void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
  // gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 30.0);
 //glOrtho(-150.0, 190.0,  -130.0, 210.0,170,-170);
  gluPerspective(70, (GLfloat) w/(GLfloat) h,  0.1, 1000);

   glMatrixMode(GL_MODELVIEW);
  
}

void idle (void)
{
  

    //  Calculate FPS
    calculateFPS();

    //  Call display function (draw the current frame)
    glutPostRedisplay ();
}





int _tmain(int argc, _TCHAR* argv[])
{
	// I've set the argv[1] to track.txt.
	// To change it, on the "Solution Explorer", 
	// right click "assign1", choose "Properties",
	// go to "Configuration Properties", click "Debugging",
	// then type your track file name for the "Command Arguments"
	if (argc<2)
	{  
		printf ("usage: %s <trackfile>\n", argv[0]);
		exit(0);
	}
	
	loadSplines(argv[1]);
	compute_Matrix();
	build_direction();
	build_trail();

	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(640,480);
	glutCreateWindow("My OpenGL");
	
	
	init();
	
	glutDisplayFunc(display);
	//SetupLight();
	glutReshapeFunc(reshape);
	glutIdleFunc (idle);

	/* callback for mouse drags */
	glutMotionFunc(mousedrag);
	/* callback for idle mouse movement */
	glutPassiveMotionFunc(mouseidle);
	/* callback for mouse button changes */
	glutMouseFunc(mousebutton);
	
	

	/* do initialization */
	glEnable(GL_DEPTH_TEST);
	
	glutMainLoop();
	return 0;
}