#ifndef __UTILITY_H__
#define __UTILITY_H__

enum Axis { X=0, Y, Z, W };

#define Epsilon 1.0E-5
#define MAX_STR 256

/* some useful function macros */
#define abs(a)  ((a) < 0) ? -(a) : (a)
#define min(a, b)  ((a) < (b)) ? (a) : (b)
#define max(a, b)  ((a) > (b)) ? (a) : (b)
#define odd(a)  (((a) & 0x1) != 0)
#define even(a)  (((a) & 0x1) == 0)
#define ROUND(a)  ((a) > 0 ? (int)((a) + 0.5) : -(int)(0.5 - (a)))
#define FLOOR(a)  ((a) > 0 ? (int)(a) : -(int)(-a))
#define CEILING(a)  ((a) == (int)(a) ? (a) : (a) > 0 ? 1 + (int)(a) : -(1 + (int)(-a)))
#define SWAP_INT(a, b)  ((a)^=(b)^=(a)^=(b))
#define Fatal(s)  fprintf(stderr, "Error: %s", (s)) ; exit(-1)
#define Point(f, x, y, z)  (f[0]) = (x); (f[1]) = (y); (f[2]) = (z)
#define PointCopy(p1, p0)  ((p1)[0]) = ((p0)[0]); \
                           ((p1)[1]) = ((p0)[1]); \
			   ((p1)[2]) = ((p0)[2])
#define PrintPoint(f)  printf("(%.2f %.2f %.2f)\n", f[0], f[1], f[2])
#define SWAP_POINT(p1, p0) \
{ \
  float p[3]; \
  p[0] = p1[0]; \
  p[1] = p1[1]; \
  p[2] = p1[2]; \
  p1[0] = p0[0]; \
  p1[1] = p0[1]; \
  p1[2] = p0[2]; \
  p0[0] = p[0]; \
  p0[1] = p[1]; \
  p0[2] = p[2]; \
}


/* use float instead of double
   sin, cos, tan, etc. take double and return double
   but fsin, fcos, ftan, etc. take float and return float
   so they're faster -> I use these */
//#define sin fsin
//#define cos fcos
//#define tan ftan
//#define asin fasin
//#define acos facos
//#define atan fatan
//#define sqrt fsqrt

typedef struct { float x, y, z, w; } Vertex;

typedef struct _POINTS
{
  float *point;
  struct _POINTS *next;
} POINTS;

typedef struct timeval TIME;

int random_int(int from, int to);
float random_float(float from, float to);
float deg_to_rad(float deg);
float rad_to_deg(float rad);
float distance(float *p1, float *p2);
float distance2d(float *p1, float *p2);
float magnitude(float *n);
float dot_product(float *u, float *v);
void cross_product(float *v1, float *v2, float *n);
void normalize(float *n);
void arot(float *pt, char axis, float angle);
void qrot(float *pt, float *axis, float angle);
void Sleep(int n);
void win_stat(long win, long *origin, long *size);
void win_clear(short bg_red, short bg_green, short bg_blue);
void init_timer(TIME *t0);
float elapsed_time(TIME *t0);

void normalize1(float *norm, float **p, int start, int size);
void Normal(float **p, float *n);

//void EnvMapUV(float norm[], Matrix m);

#endif /* __UTILITY_H__ */