/*
CSCI 480
Assignment 3 Raytracer

Name:Zichen Nie		
*/

#include <pic.h>
#include <windows.h>
#include <stdlib.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <string>
#include <math.h>


#define MAX_TRIANGLES 10
#define MAX_SPHERES 10
#define MAX_LIGHTS 10

char *filename="7.jpg";
 
//different display modes
#define MODE_DISPLAY 1
#define MODE_JPEG 2
int mode=MODE_JPEG;

//you may want to make these smaller for debugging purposes
#define WIDTH 640
#define HEIGHT 480

//the field of view of the camera
#define fov 90.0

struct Vertex
{
	double position[3];
	double color_diffuse[3];
	double color_specular[3];
	double normal[3];
	double shininess;
	double dir[3];//ray dirction on image
	double pic_color[3];//color on the image
};

typedef struct _Triangle
{
  struct Vertex v[3];
  double n[3];
  double a;//assume the plane function is a*c+b*y+c*z=d=0
  double b;
  double c;
  double d;

} Triangle;

typedef struct _Sphere
{
  double position[3];
  double color_diffuse[3];
  double color_specular[3];
  double shininess;
  double radius;
} Sphere;

typedef struct _Light
{
  double position[3];
  double color[3];
} Light;

Triangle triangles[MAX_TRIANGLES];
Sphere spheres[MAX_SPHERES];
Light lights[MAX_LIGHTS];
double ambient_light[3];

int num_triangles=0;
int num_spheres=0;
int num_lights=0;

void plot_pixel_display(double x,double y,double r,double g,double b);
void plot_pixel(double x,double y,double r,double g,double b);
int Is_shadow(Vertex v, double m[],int num,int N,int L);
void trace(int x,int y);
void gen_ray();
void Calculate_traingle_function();
void init_pix();

Vertex pix[700][700];//final pixel image

int Is_shadow(Vertex v, double m[],int num,int N,int L)//v is the intersect, m[] is the direction of v, num denotes the shape of the object that the intersect is on
	//N denotes the number of the object the intersect is on, L denotes the intersect is under which light
{ 
	double t;
	int i;
	int flag=1;// not in shadow
	double distance=pow((lights[L].position[0]-v.position[0]),2)+pow((lights[L].position[1]-v.position[1]),2)+pow((lights[L].position[2]-v.position[2]),2);
	distance=sqrt(distance);

	for(i=0;i<num_triangles;i++)//search every triangle and draw them
   {
	 if(num==0 && i==N) continue;//num:0 denotes the triangle, 3 denotes the sphere, N is the current object that the intersect is on
	  	
			//judge if there is any intersection with the plane
			double root=triangles[i].a*m[0]+triangles[i].b*m[1]+triangles[i].c*m[2];
			
			if(root!=0)
			{
				//n dot product with dir is not zero , has intersection
				t=-(triangles[i].a*v.position[0]+triangles[i].b*v.position[1]+triangles[i].c*v.position[2]+triangles[i].d);
				t=t/root;
				
				if(t>0.000001 && t<distance)
				{
					//the intersection is valid,judge weather the intersecton is within the triangle plane
					double area_abc,area_pab,area_pca,area_pbc;
					double o,p,q;
					double t_A[3],t_B[3],t_C[3],P[3];
					// something wrong
					if(triangles[i].c!=0)
					{
						P[0]=v.position[0]+m[0]*t;
						P[1]=v.position[1]+m[1]*t;
						P[2]=0; 
						//project triangle into plane z=0
						t_C[0]=triangles[i].v[0].position[0];t_C[1]=triangles[i].v[0].position[1];t_C[2]=0;
						t_B[0]=triangles[i].v[1].position[0];t_B[1]=triangles[i].v[1].position[1];t_B[2]=0;
						t_A[0]=triangles[i].v[2].position[0];t_A[1]=triangles[i].v[2].position[1];t_A[2]=0;//printf("AB:(%f,%f,%f)",AB[0],AB[1],AB[2]);
					
					    area_abc=0.5*((t_B[0]-t_C[0])*(t_A[1]-t_C[1])-(t_A[0]-t_C[0])*(t_B[1]-t_C[1]));
						area_pab=0.5*((t_B[0]-P[0])*(t_A[1]-P[1])-(t_A[0]-P[0])*(t_B[1]-P[1]));
						area_pbc=0.5*((t_C[0]-P[0])*(t_B[1]-P[1])-(t_B[0]-P[0])*(t_C[1]-P[1]));
						area_pca=0.5*((t_A[0]-P[0])*(t_C[1]-P[1])-(t_C[0]-P[0])*(t_A[1]-P[1]));

					
					}
					else if(triangles[i].b!=0)
					{
							//y=0
							t_C[0]=triangles[i].v[0].position[0];t_C[1]=0;t_C[2]=triangles[i].v[0].position[2];
							t_B[0]=triangles[i].v[1].position[0];t_B[1]=0;t_B[2]=triangles[i].v[1].position[2];
							t_A[0]=triangles[i].v[2].position[0];t_A[1]=0;t_A[2]=triangles[i].v[2].position[2];//printf("AB:(%f,%f,%f)",AB[0],AB[1],AB[2]);

							P[0]=v.position[0]+m[0]*t;
							P[1]=0;
							P[2]=v.position[2]+m[2]*t;

							area_abc=0.5*((t_B[0]-t_C[0])*(t_A[2]-t_C[2])-(t_A[0]-t_C[0])*(t_B[2]-t_C[2]));
							area_pab=0.5*((t_B[0]-P[0])*(t_A[2]-P[2])-(t_A[0]-P[0])*(t_B[2]-P[2]));
							area_pbc=0.5*((t_C[0]-P[0])*(t_B[2]-P[2])-(t_B[0]-P[0])*(t_C[2]-P[2]));
							area_pca=0.5*((t_A[0]-P[0])*(t_C[2]-P[2])-(t_C[0]-P[0])*(t_A[2]-P[2]));
					
					}
					else
					{

					}
						
							o=area_pab/area_abc;
							p=area_pbc/area_abc;
							q=1-o-p;

				
					if(o>=0 && p>=0 && q>=0)
					{  
						
	                     flag=0;//is a shadow
						 return flag=0;
							
					}

				}//end t
			}//end root
	  
	}//end for
	
	for(i=0;i<num_spheres;i++)//search every intersect on sphere
	{
		if(num==3 && i==N) continue;//num:3 denotes the sphere, N denotes the number of sphere that the intersect is on
		
				double a,b,c;// interception equation a*t^2+b*t+c=0
				a=1;
				b=2*(m[0]*(v.position[0]-spheres[i].position[0])+m[1]*(v.position[1]-spheres[i].position[1])+m[2]*(v.position[2]-spheres[i].position[2]));
				c=pow(v.position[0]-spheres[i].position[0],2)+pow(v.position[1]-spheres[i].position[1],2)+pow(v.position[2]-spheres[i].position[2],2)-pow(spheres[i].radius,2);
		
				//calculate intersection if exist
				double root=b*b-4*c;
				if(root>0.00001)
				{
					t=(-b+sqrt(root))/2.0;
					if(t>0.00001 && t<=distance)
					{
						flag=0;
						return flag=0;
					}
				}
		 
	}
	return flag;
}


void trace(int x,int y)// find intersection of each ray from position of 2D imagle (x,y)
{
	//generate local model using ray tracing
	int i,j;
	double t; 
	double in_light[3]={0,0,0};//L vector
	double in_view[3]={0,0,0};//v vector
	double in_reflect[3]={0,0,0};//r vector
	double in_sh=0;//shiness
	double light_color[3]={0,0,0};

	Vertex intersect;// the intersect from ray

		for(i=0;i<num_triangles;i++)//search every triangle and draw them
		{
			
			//judge if there is any intersection with the plane
			double root=triangles[i].a*pix[x][y].dir[0]+triangles[i].b*pix[x][y].dir[1]+triangles[i].c*pix[x][y].dir[2];
			
			if(root!=0)
			{
				//n dot product with dir is not zero , has intersection
				t=-(triangles[i].a*pix[x][y].position[0]+triangles[i].b*pix[x][y].position[1]+triangles[i].c*pix[x][y].position[2]+triangles[i].d);
				t=t/root;
				if(t>0.00001)
				{
					//the intersection is valid,judge weather the intersecton is within the triangle plane
					double area_abc,area_pab,area_pca,area_pbc;
					double o,p,q;
					double t_A[3],t_B[3],t_C[3],P[3];
					
					if(triangles[i].c!=0)

					{
						
						P[0]=pix[x][y].position[0]+pix[x][y].dir[0]*t;
						P[1]=pix[x][y].position[1]+pix[x][y].dir[1]*t;
						P[2]=0; 

						//project triangle into plane z=0
						t_C[0]=triangles[i].v[0].position[0];t_C[1]=triangles[i].v[0].position[1];t_C[2]=0;
						t_B[0]=triangles[i].v[1].position[0];t_B[1]=triangles[i].v[1].position[1];t_B[2]=0;
						t_A[0]=triangles[i].v[2].position[0];t_A[1]=triangles[i].v[2].position[1];t_A[2]=0;//printf("AB:(%f,%f,%f)",AB[0],AB[1],AB[2]);
					     
						area_abc=0.5*((t_B[0]-t_C[0])*(t_A[1]-t_C[1])-(t_A[0]-t_C[0])*(t_B[1]-t_C[1]));
						area_pab=0.5*((t_B[0]-P[0])*(t_A[1]-P[1])-(t_A[0]-P[0])*(t_B[1]-P[1]));
						area_pbc=0.5*((t_C[0]-P[0])*(t_B[1]-P[1])-(t_B[0]-P[0])*(t_C[1]-P[1]));
						area_pca=0.5*((t_A[0]-P[0])*(t_C[1]-P[1])-(t_C[0]-P[0])*(t_A[1]-P[1]));
					
					
					
					}
					else if(triangles[i].b!=0)
					{
							//project triangle to xz plane
							t_C[0]=triangles[i].v[0].position[0];  t_C[1]=0;  t_C[2]=triangles[i].v[0].position[2];
							t_B[0]=triangles[i].v[1].position[0];  t_B[1]=0; t_B[2]=triangles[i].v[1].position[2];
							t_A[0]=triangles[i].v[2].position[0];  t_A[1]=0;  t_A[2]=triangles[i].v[2].position[2];
							P[0]=pix[x][y].position[0]+pix[x][y].dir[0]*t;  P[1]=0;  P[2]=pix[x][y].position[2]+pix[x][y].dir[2]*t;
				

							area_abc=0.5*((t_B[0]-t_C[0])*(t_A[2]-t_C[2])-(t_A[0]-t_C[0])*(t_B[2]-t_C[2]));
							area_pab=0.5*((t_B[0]-P[0])*(t_A[2]-P[2])-(t_A[0]-P[0])*(t_B[2]-P[2]));
							area_pbc=0.5*((t_C[0]-P[0])*(t_B[2]-P[2])-(t_B[0]-P[0])*(t_C[2]-P[2]));
							area_pca=0.5*((t_A[0]-P[0])*(t_C[2]-P[2])-(t_C[0]-P[0])*(t_A[2]-P[2]));

					}
					else
					{
					
						// unified normal cannot be (0,0,0),so no need to add code here
					}
						

							o=area_pab/area_abc;
							p=area_pbc/area_abc;
							q=area_pca/area_abc;
           
				
					if(p>=0 && q>=0 && o>=0)
					{  

						//get the intersect point which is in the triangle
						intersect.position[0]=pix[x][y].position[0]+pix[x][y].dir[0]*t;
						intersect.position[1]=pix[x][y].position[1]+pix[x][y].dir[1]*t;
						intersect.position[2]=pix[x][y].position[2]+pix[x][y].dir[2]*t;
						 
					
						//get the diffuse,specular color of the intersect point
						intersect.color_diffuse[0]=o*triangles[i].v[0].color_diffuse[0]+p*triangles[i].v[2].color_diffuse[0]+q*triangles[i].v[1].color_diffuse[0];
						intersect.color_diffuse[1]=o*triangles[i].v[0].color_diffuse[1]+p*triangles[i].v[2].color_diffuse[1]+q*triangles[i].v[1].color_diffuse[1];
						intersect.color_diffuse[2]=o*triangles[i].v[0].color_diffuse[2]+p*triangles[i].v[2].color_diffuse[2]+q*triangles[i].v[1].color_diffuse[2];

						intersect.color_specular[0]=o*triangles[i].v[0].color_specular[0]+p*triangles[i].v[2].color_specular[0]+q*triangles[i].v[1].color_specular[0];
						intersect.color_specular[1]=o*triangles[i].v[0].color_specular[1]+p*triangles[i].v[2].color_specular[1]+q*triangles[i].v[1].color_specular[1];
						intersect.color_specular[2]=o*triangles[i].v[0].color_specular[2]+p*triangles[i].v[2].color_specular[2]+q*triangles[i].v[1].color_specular[2];
						
						
						
					for(int k=0;k<num_lights;k++)
						{
							 double m=0;

								in_light[0]=lights[k].position[0]-intersect.position[0];
								in_light[1]=lights[k].position[1]-intersect.position[1];
								in_light[2]=lights[k].position[2]-intersect.position[2];

								//normalize light vector
								 m=sqrt(pow(in_light[0],2)+pow(in_light[1],2)+pow(in_light[2],2));
								in_light[0]=(double)in_light[0]/m;
								in_light[1]=(double)in_light[1]/m;
								in_light[2]=(double)in_light[2]/m;
								
								//normal of the intersect
                                intersect.normal[0]=o*triangles[i].v[0].normal[0]+p*triangles[i].v[2].normal[0]+q*triangles[i].v[1].normal[0];
								intersect.normal[1]=o*triangles[i].v[0].normal[1]+p*triangles[i].v[2].normal[1]+q*triangles[i].v[1].normal[1];
								intersect.normal[2]=o*triangles[i].v[0].normal[2]+p*triangles[i].v[2].normal[2]+q*triangles[i].v[1].normal[2];
				
								m=pow(intersect.normal[0],2)+pow(intersect.normal[1],2)+pow(intersect.normal[2],2);
								intersect.normal[0]=(double)intersect.normal[0]/sqrt(m);
								intersect.normal[1]=(double)intersect.normal[1]/sqrt(m);
								intersect.normal[2]=(double)intersect.normal[2]/sqrt(m);

								
								
								//get the light vector, view vector and reflect vector of the point
								in_view[0]=-pix[x][y].dir[0];
								in_view[1]=-pix[x][y].dir[1];
								in_view[2]=-pix[x][y].dir[2];//already normalized 

								in_sh=o*triangles[i].v[0].shininess+p*triangles[i].v[2].shininess+q*triangles[i].v[1].shininess;

								in_reflect[0]=2*(in_light[0]*intersect.normal[0]+in_light[1]*intersect.normal[1]+in_light[2]*intersect.normal[2])*intersect.normal[0]-in_light[0];
								in_reflect[1]=2*(in_light[0]*intersect.normal[0]+in_light[1]*intersect.normal[1]+in_light[2]*intersect.normal[2])*intersect.normal[1]-in_light[1];
								in_reflect[2]=2*(in_light[0]*intersect.normal[0]+in_light[1]*intersect.normal[1]+in_light[2]*intersect.normal[2])*intersect.normal[2]-in_light[2];
						        
								//normalize reflect vector
								m=sqrt(pow(in_reflect[0],2)+pow(in_reflect[1],2)+pow(in_reflect[2],2));
								in_reflect[0]=(double)in_reflect[0]/m;
								in_reflect[1]=(double)in_reflect[1]/m;
								in_reflect[2]=(double)in_reflect[2]/m;

						  
							if(Is_shadow(intersect,in_light,0,i,k)==1)//not a shadow intersect
							{ //to see if the intersection shadow cast can reach light
								

								//apply all the vectors to phong model
								double l_n=in_light[0]*intersect.normal[0]+in_light[1]*intersect.normal[1]+in_light[2]*intersect.normal[2];
								double v_r=in_view[0]*in_reflect[0]+in_view[1]*in_reflect[1]+in_view[2]*in_reflect[2];
								if(l_n<0)
									l_n=0;
								if(v_r<=0)
									v_r=0;
								
								light_color[0]+=lights[k].color[0]*((intersect.color_diffuse[0]*l_n)+intersect.color_specular[0]*pow(v_r,in_sh));
								light_color[1]+=lights[k].color[1]*((intersect.color_diffuse[1]*l_n)+intersect.color_specular[1]*pow(v_r,in_sh));
							    light_color[2]+=lights[k].color[2]*((intersect.color_diffuse[2]*l_n)+intersect.color_specular[2]*pow(v_r,in_sh));
			
							}//end is shadow 
							else
							{ 
							   if(pix[x][y].pic_color[0]!=0 && pix[x][y].pic_color[1]!=0 && pix[x][y].pic_color[2]!=0)
								{
									//the intersect is in shadow of black if and only if it is in shadow of all lights
								}
								else
								{
									light_color[0]=0;
									light_color[1]=0;
									light_color[2]=0;
								}
							}
							

						
						}//end light

						pix[x][y].pic_color[0]=light_color[0]+ambient_light[0];
						pix[x][y].pic_color[1]=light_color[1]+ambient_light[1];
						pix[x][y].pic_color[2]=light_color[2]+ambient_light[2];

						      //clamp the color to 1 if lighter is bigger than1
					            if( pix[x][y].pic_color[0]>1)
									 pix[x][y].pic_color[0]=1;
								if (pix[x][y].pic_color[1]>1)
									pix[x][y].pic_color[1]=1;
								if(pix[x][y].pic_color[2]>1)
									pix[x][y].pic_color[2]=1;
			 

					}//end area
				}//end t
				
			}//end root if 
			

		}//end for triangle search


		for(i=0;i<num_spheres;i++)//search every sphere and draw them
	{
		light_color[0]=0;
		light_color[1]=0;
		light_color[2]=0;
		double a,b,c;// interception equation a*t^2+b*t+c=0
		a=1;
		b=2*(pix[x][y].dir[0]*(pix[x][y].position[0]-spheres[i].position[0])+pix[x][y].dir[1]*(pix[x][y].position[1]-spheres[i].position[1])+pix[x][y].dir[2]*(pix[x][y].position[2]-spheres[i].position[2]));
		c=pow(pix[x][y].position[0]-spheres[i].position[0],2)+pow(pix[x][y].position[1]-spheres[i].position[1],2)+pow(pix[x][y].position[2]-spheres[i].position[2],2)-pow(spheres[i].radius,2);
		
		//calculate intersection if exist
		double root=b*b-4*c;
		if(root<0)
		{
			//no intersection with this sphere
		}
		
		else
		{
			//two intersection with this sphere, we take the closest one with the minimum t//one intersection with this sphere
			t=(-b-sqrt(root))/2.0;

			//get the intersect point
			intersect.position[0]=pix[x][y].position[0]+pix[x][y].dir[0]*t;
			intersect.position[1]=pix[x][y].position[1]+pix[x][y].dir[1]*t;
			intersect.position[2]=pix[x][y].position[2]+pix[x][y].dir[2]*t;
			
			//get diffuse color
			intersect.color_diffuse[0]=spheres[i].color_diffuse[0];
			intersect.color_diffuse[1]=spheres[i].color_diffuse[1];
			intersect.color_diffuse[2]=spheres[i].color_diffuse[2];

			//get specular color
			intersect.color_specular[0]=spheres[i].color_specular[0];
			intersect.color_specular[1]=spheres[i].color_specular[1];
			intersect.color_specular[2]=spheres[i].color_specular[2];

			//get shiniess
			intersect.shininess=spheres[i].shininess;

			 for(int k=0;k<num_lights;k++)
				{
						double m=0;

						in_light[0]=lights[k].position[0]-intersect.position[0];
						in_light[1]=lights[k].position[1]-intersect.position[1];
						in_light[2]=lights[k].position[2]-intersect.position[2];
						//normalize light vector
						m=sqrt(pow(in_light[0],2)+pow(in_light[1],2)+pow(in_light[2],2));
						in_light[0]=(double)in_light[0]/m;
						in_light[1]=(double)in_light[1]/m;
						in_light[2]=(double)in_light[2]/m;

                               
				//to see if the intersection shadow cast can reach light
					if(Is_shadow(intersect,in_light,3,i,k)==1)//not a shadow intersect
					{
								
						intersect.normal[0]=intersect.position[0]-spheres[i].position[0];
						intersect.normal[1]=intersect.position[1]-spheres[i].position[1];
						intersect.normal[2]=intersect.position[2]-spheres[i].position[2];


						m=sqrt(pow(intersect.normal[0],2)+pow(intersect.normal[1],2)+pow(intersect.normal[2],2));
						intersect.normal[0]=(double)intersect.normal[0]/m;
						intersect.normal[1]=(double)intersect.normal[1]/m;
						intersect.normal[2]=(double)intersect.normal[2]/m;

						//get the light vector, view vector and reflect vector of the point
						in_view[0]=-pix[x][y].dir[0];
						in_view[1]=-pix[x][y].dir[1];
						in_view[2]=-pix[x][y].dir[2];//already normalized 

						in_sh=spheres[i].shininess;
						
						in_reflect[0]=2*(in_light[0]*intersect.normal[0]+in_light[1]*intersect.normal[1]+in_light[2]*intersect.normal[2])*intersect.normal[0]-in_light[0];
						in_reflect[1]=2*(in_light[0]*intersect.normal[0]+in_light[1]*intersect.normal[1]+in_light[2]*intersect.normal[2])*intersect.normal[1]-in_light[1];
						in_reflect[2]=2*(in_light[0]*intersect.normal[0]+in_light[1]*intersect.normal[1]+in_light[2]*intersect.normal[2])*intersect.normal[2]-in_light[2];
						        
						//normalize reflect vector
						m=sqrt(pow(in_reflect[0],2)+pow(in_reflect[1],2)+pow(in_reflect[2],2));
						in_reflect[0]=(double)in_reflect[0]/m;
						in_reflect[1]=(double)in_reflect[1]/m;
						in_reflect[2]=(double)in_reflect[2]/m;

							
						//apply all the vectors to phong model
						double l_n=in_light[0]*intersect.normal[0]+in_light[1]*intersect.normal[1]+in_light[2]*intersect.normal[2];
						double v_r=in_view[0]*in_reflect[0]+in_view[1]*in_reflect[1]+in_view[2]*in_reflect[2];
						if(l_n<0)
							l_n=0;
						if(v_r<=0)
							v_r=0;
								
						light_color[0]+=lights[k].color[0]*((intersect.color_diffuse[0]*l_n)+intersect.color_specular[0]*pow(v_r,in_sh));
						light_color[1]+=lights[k].color[1]*((intersect.color_diffuse[1]*l_n)+intersect.color_specular[1]*pow(v_r,in_sh));
						light_color[2]+=lights[k].color[2]*((intersect.color_diffuse[2]*l_n)+intersect.color_specular[2]*pow(v_r,in_sh));
								
							
					}//end is shadow 
					
					else
					{
							if(light_color[0]!=0 && light_color[1]!=0 && light_color[2]!=0)
						{
						}
						else
						{
							light_color[0]=0;
							light_color[1]=0;
							light_color[2]=0;
						}
					}
							
						
				}//end light


					pix[x][y].pic_color[0]=light_color[0]+ambient_light[0];
					pix[x][y].pic_color[1]=light_color[1]+ambient_light[1];
					pix[x][y].pic_color[2]=light_color[2]+ambient_light[2];

					
								if( pix[x][y].pic_color[0]>1)
									 pix[x][y].pic_color[0]=1;
								if (pix[x][y].pic_color[1]>1)
									pix[x][y].pic_color[1]=1;
								if(pix[x][y].pic_color[2]>1)
									pix[x][y].pic_color[2]=1;
						
		}//end root
	
}//end sphere
		
	}
	






void gen_ray()
{
	int i,j;
	for(i=0;i<=WIDTH;i++)
		for(j=0;j<=HEIGHT;j++)
		{
			//generate direction
			pix[i][j].dir[0]=-4.0/3.0+i/240.0;//dir.x
			pix[i][j].dir[1]=-1+j/240.0;//dir.y
			pix[i][j].dir[2]=-1;//dir.z
			double m=pow(pix[i][j].dir[0],2)+pow(pix[i][j].dir[1],2)+pow(pix[i][j].dir[2],2);
			//unify dir[i][j]
			pix[i][j].dir[0]=pix[i][j].dir[0]/sqrt(m);
		    pix[i][j].dir[1]=pix[i][j].dir[1]/sqrt(m);
			pix[i][j].dir[2]=pix[i][j].dir[2]/sqrt(m);
		  
			trace(i,j);
		}
}

void init_pix()
{
	int i,j;
	
	for(i=0;i<=WIDTH;i++)
		for(j=0;j<=HEIGHT;j++)
			{
				pix[i][j].pic_color[0]=1;//background color is white
				pix[i][j].pic_color[1]=1;
				pix[i][j].pic_color[2]=1;

				//initial image vertex in 3d dimention
				pix[i][j].position[0]=-4.0/3.0+((double)i)/240;
				pix[i][j].position[1]=-1+((double)j)/240;
				pix[i][j].position[2]=-1;
		
		  
		}
 
}

/*void display()
{
	int i,j;
	for(i=0;i<=WIDTH;i++)
		for(j=0;j<=HEIGHT;j++)
		{
			glPointSize(2.0);//strange vertical lines in the output image
			glBegin(GL_POINTS);
				glColor3f(pix[i][j].pic_color[0],pix[i][j].pic_color[1],pix[i][j].pic_color[2]);
				glVertex2f(pix[i][j].position[0],pix[i][j].position[1]);
				
            glEnd(); 
		}
		glFlush();
}*/

void save_jpg()
{
  Pic *in = NULL;
  int i;
  in = pic_alloc(640, 480, 3, NULL);
  printf("Saving JPEG file: %s\n", filename);

  //memcpy(in->pix,buffer,3*WIDTH*HEIGHT);

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

void plot_pixel(double x,double y,double r,double g, double b)
{
  plot_pixel_display(x,y,r,g,b);
  if(mode == MODE_JPEG)
      //plot_pixel_jpeg(x,y,r,g,b);
	  save_jpg();
}



//MODIFY THIS FUNCTION
void draw_scene()
{
  //display();
  unsigned int x,y,i,j;
  //simple output
  for(i=0;i<=WIDTH;i++)
		for(j=0;j<=HEIGHT;j++)
		{
			glPointSize(2.0);//strange vertical lines in the output image
            glBegin(GL_POINTS);
	          { 
                plot_pixel(pix[i][j].position[0],pix[i][j].position[1],pix[i][j].pic_color[0],pix[i][j].pic_color[1],pix[i][j].pic_color[2]);
			  }
            glEnd();
		}
   
    glFlush();
	save_jpg();
  printf("Done!\n"); fflush(stdout);
}


void plot_pixel_display(double x,double y,double r,double g,double b)
{
  glColor3f(r,g,b);
  glVertex2f(x,y);
}


void parse_check(char *expected,char *found)
{
  if(stricmp(expected,found))
    {
      char error[100];
      printf("Expected '%s ' found '%s '\n",expected,found);
      printf("Parse error, abnormal abortion\n");
      exit(0);
    }

}

void parse_doubles(FILE*file, char *check, double p[3])
{
  char str[100];
  fscanf(file,"%s",str);
  parse_check(check,str);
  fscanf(file,"%lf %lf %lf",&p[0],&p[1],&p[2]);
  printf("%s %lf %lf %lf\n",check,p[0],p[1],p[2]);
}

void parse_rad(FILE*file,double *r)
{
  char str[100];
  fscanf(file,"%s",str);
  parse_check("rad:",str);
  fscanf(file,"%lf",r);
  printf("rad: %f\n",*r);
}

void parse_shi(FILE*file,double *shi)
{
  char s[100];
  fscanf(file,"%s",s);
  parse_check("shi:",s);
  fscanf(file,"%lf",shi);
  printf("shi: %f\n",*shi);
}

int loadScene(char *argv)
{
  FILE *file = fopen(argv,"r");
  int number_of_objects;
  char type[50];
  int i;
  Triangle t;
  Sphere s;
  Light l;
  fscanf(file,"%i",&number_of_objects);

  printf("number of objects: %i\n",number_of_objects);
  char str[200];

  parse_doubles(file,"amb:",ambient_light);

  for(i=0;i < number_of_objects;i++)
    {
      fscanf(file,"%s\n",type);
      printf("%s\n",type);
      if(stricmp(type,"triangle")==0)
	{

	  printf("found triangle\n");
	  int j;

	  for(j=0;j < 3;j++)         //initial triangle and its parameters
	    {
	      parse_doubles(file,"pos:",t.v[j].position);
	      parse_doubles(file,"nor:",t.v[j].normal);
	      parse_doubles(file,"dif:",t.v[j].color_diffuse);
	      parse_doubles(file,"spe:",t.v[j].color_specular);
	      parse_shi(file,&t.v[j].shininess);
	    }

	  if(num_triangles == MAX_TRIANGLES)
	    {
	      printf("too many triangles, you should increase MAX_TRIANGLES!\n");
	      exit(0);
	    }
	  triangles[num_triangles++] = t;
	}
      else if(stricmp(type,"sphere")==0)//initial sphere and its parameters
	{
	  printf("found sphere\n");

	  parse_doubles(file,"pos:",s.position);
	  parse_rad(file,&s.radius);
	  parse_doubles(file,"dif:",s.color_diffuse);
	  parse_doubles(file,"spe:",s.color_specular);
	  parse_shi(file,&s.shininess);

	  if(num_spheres == MAX_SPHERES)
	    {
	      printf("too many spheres, you should increase MAX_SPHERES!\n");
	      exit(0);
	    }
	  spheres[num_spheres++] = s;
	}
      else if(stricmp(type,"light")==0)
	{
	  printf("found light\n");
	  parse_doubles(file,"pos:",l.position);
	  parse_doubles(file,"col:",l.color);

	  if(num_lights == MAX_LIGHTS)
	    {
	      printf("too many lights, you should increase MAX_LIGHTS!\n");
	      exit(0);
	    }
	  lights[num_lights++] = l;
	}
      else
	{
	  printf("unknown type in scene description:\n%s\n",type);
	  exit(0);
	}
    }
  return 0;
}


void init()
{
  glMatrixMode(GL_PROJECTION);
  glOrtho(-4.0/3,4.0/3,-1,1,1,-1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void idle()
{
  //hack to make it only draw once
  static int once=0;
  if(!once)
  {
      draw_scene();
      if(mode == MODE_JPEG)
	save_jpg();
    }
  once=1;
}

void Calculate_traingle_function()
{
   int i=0;
   for(i=0;i<num_triangles;i++)//search every triangle and draw them
		{
			//calculate the normal vector of plain triangle
			double AB[3];//-v0+v1
			double AC[3];//-v0+v2
			
			AB[0]=-triangles[i].v[0].position[0]+triangles[i].v[1].position[0];
			AB[1]=-triangles[i].v[0].position[1]+triangles[i].v[1].position[1];
			AB[2]=-triangles[i].v[0].position[2]+triangles[i].v[1].position[2];

			AC[0]=-triangles[i].v[0].position[0]+triangles[i].v[2].position[0];
			AC[1]=-triangles[i].v[0].position[1]+triangles[i].v[2].position[1];
			AC[2]=-triangles[i].v[0].position[2]+triangles[i].v[2].position[2];

			
			//calculating normal of the plane which is a product of AB AC
			triangles[i].n[0]=AB[1]*AC[2]-AC[1]*AB[2];
			triangles[i].n[1]=AB[2]*AC[0]-AB[0]*AC[2];
			triangles[i].n[2]=AB[0]*AC[1]-AC[0]*AB[1];
			
			//calculating plane equation
			
			//unify n
			double m=(pow(triangles[i].n[0],2)+pow(triangles[i].n[1],2)+pow(triangles[i].n[2],2));
			triangles[i].n[0]=(double)triangles[i].n[0]/sqrt(m);
			triangles[i].n[1]=(double)triangles[i].n[1]/sqrt(m);;
			triangles[i].n[2]=(double)triangles[i].n[2]/sqrt(m);;
		

			triangles[i].a=triangles[i].n[0];
			triangles[i].b=triangles[i].n[1];
			triangles[i].c=triangles[i].n[2];
			triangles[i].d=-triangles[i].n[0]*triangles[i].v[0].position[0]-triangles[i].n[1]*triangles[i].v[0].position[1]-triangles[i].n[2]*triangles[i].v[0].position[2];
			
			
  }  
}


int main (int argc, char ** argv)
{
  if (argc<2 || argc > 3)
  {  
    printf ("usage: %s <scenefile> [jpegname]\n", argv[0]);
    exit(0);
  }
  if(argc == 3)
    {
      mode = MODE_JPEG;
      filename = argv[2];
    }
  else if(argc == 2)
    mode = MODE_DISPLAY;

  glutInit(&argc,argv);
  loadScene(argv[1]);

  glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
  glutInitWindowPosition(0,0);
  glutInitWindowSize(WIDTH,HEIGHT);
  int window = glutCreateWindow("Ray Tracer");

  Calculate_traingle_function();
  init_pix();
  gen_ray();
  glutDisplayFunc(draw_scene);
  glutIdleFunc(idle);
  init(); 
  glutMainLoop();
}
