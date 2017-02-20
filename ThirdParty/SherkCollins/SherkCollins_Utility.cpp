#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <random>
//#include <unistd.h>
//#include <sys/time.h>
//#include <gl.h>
#include "SherkCollins_Utility.h"

//int random_int(int from, int to)
//{
//  static int first = 1;
//
//  if (first)
//    {
//      first = 0;
//      random(time(0));
//    }
//
//  return (((int)random()%(to-from)) + from);
//}
//
//float random_float(float from, float to)
//{
//  static int first = 1;
//
//  if (first)
//    {
//      first = 0;
//      srand48(time(0));
//    }
//
//  return ((float)drand48()*(to-from) + from);
//}

/* changes degrees to radians */
float deg_to_rad(float deg)
{
  return (deg*0.01745329252); /* M_PI/180 = 0.01745329252 */
}

/* changes radians to degrees */
float rad_to_deg(float rad)
{
  return (rad*57.29577951307); /* 180/M_PI = 57.29577951307 */
}

/* returns the distance between two points p1 and p2 */
float distance(float *p1, float *p2)
{
  return sqrt((p1[X] - p2[X])*(p1[X] - p2[X]) +
	      (p1[Y] - p2[Y])*(p1[Y] - p2[Y]) +
	      (p1[Z] - p2[Z])*(p1[Z] - p2[Z]));
}

/* returns the distance between the 2D points p1 and p2 */
float distance2d(float *p1, float *p2)
{
  return sqrt((p1[X] - p2[X])*(p1[X] - p2[X]) +
	      (p1[Y] - p2[Y])*(p1[Y] - p2[Y]));
}

/* returns the magnitude of the vector n */
float magnitude(float *n)
{
  return sqrt(n[X]*n[X] + n[Y]*n[Y] + n[Z]*n[Z]);
}

/* returns the dot product of u and v: u . v */
float dot_product(float *u, float *v)
{
  return (u[0]*v[0] + u[1]*v[1] + u[2]*v[2]);
}

/* sets the cross product of v1 and v2 (v1 x v2) to n */
void cross_product(float *v1, float *v2, float *n)
{
  float tmp[3];
  
  tmp[X] = v1[Y]*v2[Z] - v2[Y]*v1[Z];
  tmp[Y] = v1[Z]*v2[X] - v1[X]*v2[Z];
  tmp[Z] = v1[X]*v2[Y] - v2[X]*v1[Y];
  n[X] = tmp[X];
  n[Y] = tmp[Y];
  n[Z] = tmp[Z];
}

/* normalizes the vector n */
void normalize(float *n)
{
  float mag;

  mag = magnitude(n);
  if (mag != 1.0 && mag != 0.0)
    {
      n[X] /= mag;
      n[Y] /= mag;
      n[Z] /= mag;
    }
}

/* rotates a point pt about one of the three axis x, y, or z */
void arot(float *pt, char axis, float angle)
{
  float x, y, z, cos_angle, sin_angle;

  x = pt[X];
  y = pt[Y];
  z = pt[Z];

  angle = deg_to_rad(angle);
  cos_angle = cos(angle);
  sin_angle = sin(angle);
  
  switch(axis)
    {
    case 'x': case 'X':
      pt[Y] = (y * cos_angle) - (z * sin_angle);
      pt[Z] = (y * sin_angle) + (z * cos_angle);
      break;

    case 'y': case 'Y':
      pt[X] = (x * cos_angle) - (z * sin_angle);
      pt[Z] = (x * sin_angle) + (z * cos_angle);
      break;

    case 'z': case 'Z':
      pt[X] = (x * cos_angle) - (y * sin_angle);
      pt[Y] = (x * sin_angle) + (y * cos_angle);
      break;

    default:
      break;
    }
}

/* rotates a point pt about an axis by angle using quaternian */
void qrot(float *pt, float *axis, float angle)
{
  float  cos_angle, cos2_angle, u[3], v[3], w[3], ww[3], sin_angle, dotp;

  angle = 0.5*deg_to_rad(angle);

  u[0] = axis[0];
  u[1] = axis[1];
  u[2] = axis[2];
  normalize(u); 

  sin_angle = sin(angle);
  cos_angle = cos(angle);
  cos2_angle = cos_angle*cos_angle;
  
  v[0] = u[0]*sin_angle;
  v[1] = u[1]*sin_angle;
  v[2] = u[2]*sin_angle;

  cross_product(v, pt, w);
  cross_product(v, w,  ww);

  dotp = dot_product(pt, v);
  
  pt[0] = ((cos2_angle*pt[0]) + (v[0]*dotp) + (2.0*cos_angle*w[0]) + ww[0]);
  pt[1] = ((cos2_angle*pt[1]) + (v[1]*dotp) + (2.0*cos_angle*w[1]) + ww[1]);
  pt[2] = ((cos2_angle*pt[2]) + (v[2]*dotp) + (2.0*cos_angle*w[2]) + ww[2]);
}

/* sleeps for n cycles */
//void Sleep(int n)
//{
//  int i;
//  for (i=0; i<(n*100); i++)
//    {
//      sleep(0);
//    }
//}
//
///* sets up the origin and size of a window */
//void win_stat(long win, long *origin, long *size)
//{
//  winset(win);
//  getorigin(&(origin[X]), &(origin[Y]));
//  getsize(&(size[X]), &(size[Y]));
//}
//
///* clears the screen and the zbuffer, and sets the background color */
//void win_clear(short bg_red, short bg_green, short bg_blue)
//{
//  long background;
//
//  background = (255 << 24) | (bg_blue << 16) | (bg_green << 8) | bg_red;
//
//  czclear(background, getgdesc(GD_ZMAX));
//  gflush();
//}
//
///* sets the time to the current time */
//void init_timer(TIME *t0)
//{
//  gettimeofday(t0);
//}
//
///* returns the number of seconds passed since
//   the timer was last initialized
//   it is accurate to a microsecond */
//float elapsed_time(TIME *t0)
//{
//  TIME t1;
//  long sec_diff, usec_diff;
//
//  gettimeofday(&t1);
//
//  sec_diff = t1.tv_sec - t0->tv_sec;
//  usec_diff = t1.tv_usec - t0->tv_usec;
//
//  return (float)(sec_diff) + ((float)usec_diff * 1.0E-6);
//}

void normalize1(float *norm, float **p, int start, int size)
{
  int i,pj,pi;

  norm[0] = norm[1] = norm[2] = 0.0f;
  for(i = start; i < size; i++)
    {
      if(i == (size-1))
	pi = start;
      else
        pi = i+1;
      pj = i;
      norm[0] +=((p[pj][2] + p[pi][2]) * (p[pj][1] - p[pi][1]));
      norm[1] +=((p[pj][0] + p[pi][0]) * (p[pj][2] - p[pi][2]));
      norm[2] +=((p[pj][1] + p[pi][1]) * (p[pj][0] - p[pi][0]));
    }
  norm[0] *= 0.5;
  norm[1] *= 0.5;
  norm[2] *= 0.5;

  /*
  max = norm[0];
  for (i=1; i<3; i++)
    {
      if (norm[i] > max)
	{
	  max = norm[i];
	}
    }
  norm[0] = norm[0]/max;
  norm[1] = norm[1]/max;
  norm[2] = norm[2]/max;
  */
}

void Normal(float **p, float *n)
{
  float *p1 = p[0];
  float *p2 = p[1];
  float *p3 = p[2];
  float v1[3], v2[3];

  v1[X] = p1[X] - p2[X];
  v1[Y] = p1[Y] - p2[Y];
  v1[Z] = p1[Z] - p2[Z];

  v2[X] = p3[X] - p2[X];
  v2[Y] = p3[Y] - p2[Y];
  v2[Z] = p3[Z] - p2[Z];

  n[X] = v1[Y]*v1[Z] - v2[Y]*v2[Z];
  n[Y] = v2[X]*v2[Z] - v1[X]*v1[Z];
  n[Z] = v1[X]*v1[Y] - v2[X]*v2[Y];
}

//void EnvMapUV(float norm[], Matrix m)
//{
//  float n[2], uv[2];
//
//  n[X] = m[0][0]*norm[X] + m[1][0]*norm[Y] + m[2][0]*norm[Z];
//  n[Y] = m[0][1]*norm[X] + m[1][1]*norm[Y] + m[2][1]*norm[Z];
//  uv[X] = 0.5*n[X] + 0.5;
//  uv[Y] = 0.5*n[Y] + 0.5;
//
//  t2f(uv);
//}