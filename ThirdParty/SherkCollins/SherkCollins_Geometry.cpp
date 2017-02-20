/*==== Simple Scherk-Collins Saddle-Chain Surface Generator ====*/
/* FIXES neded:
   check output !

==*/

/*--------------------------------------------------------
  Saddle-Chain is a generalized approach of generating 
  Scherk-towers, Collins-toroid, and related sculptures.
  It is set up to also do simple (twisted / Moebius) bands.

  This is a version that can produce modules with properly sealed off ends
  as should be used for demonstration units at exhibits.
  ==============================================================*/

//#include <stdio.h>
//#include <stdlib.h>
#include <Urho3D/Math/MathDefs.h>
#include "SherkCollins_Utility.h"
#include "SherkCollins_Geometry.h"


#define ROOT05 sqrt(0.5)
#define KMID 4
#define DMAX 1000
#define M_PI Urho3D::M_PI

/* global variables from output.c */
/* vertex and face data for printing to a file */
extern float xmin, xmax, ymin, ymax, zmin, zmax;  /* for bounding box */
extern int out_data;   /* save shape to output file */
extern int b_count;    /* count branches processed */


/*exported arrays*/
float n[DMAX+DMAX+3][DMAX+DMAX+1][3]; /*final vertex normals*/
float o[DMAX+DMAX+3][DMAX+DMAX+1][3]; /*offset vertices*/
float lid[DMAX+DMAX+3][KMID+KMID+1][3]; /* extra points on rims and mid-storey lids  */
float nrim[DMAX+DMAX+3][KMID+KMID+1][3]; /* rim normals  */
/* The area actually used is only (2d+2) by (2d). */
/* global array variables containing vertex info for one branch */
static float h[DMAX+DMAX+1]; /*normalized height coord in base_array*/
static float t[DMAX+DMAX+3][DMAX+DMAX+1][2]; /*texture coord*/
static float mt[DMAX+DMAX+3][DMAX+DMAX+1][2]; /* negated texture coord*/
static float v[DMAX+DMAX+3][DMAX+DMAX+1][3]; /*thin_surface pts*/
static float m[DMAX+DMAX+3][DMAX+DMAX+1][3]; /*initial vertex normals*/
float tlid[DMAX+DMAX+3][KMID+KMID+1][2]; /* texture array for lids  */
float rimt[DMAX+DMAX+3][KMID+KMID+1][2]; /* texture array for rims  */
static float tbulge[KMID+KMID+1]; /*sin fct values for elliptic rim */
static float wbulge[KMID+KMID+1]; /*cos fct values for elliptic rim */
static float trimnorm[KMID+KMID+1]; /*sin fct values for rim normals */
static float wrimnorm[KMID+KMID+1]; /*cos fct values for rim normals */

/*derived or modified global variables*/
int ring, vanes;
int d, D, dd, Dd, DD, kmax;
int total_trias;

/* local function prototypes */
static void  avnorm4(int i, int j);
static void  edgenorm(int i,  int j,  int topedge);
static void  drawnormal(float vert[], float norm[]);
static void  loadmaterial( int rim, int numbr);

static void  basearray( int branches, int ntext, float height,  
		       float flange, float twirl, float thick, float trim);
static int   surfbranch(int flat,  int ring, int stor,  
		        int ntext, float height,  float flange,  float thick,
                        int branch, float azimb,  float twirl,  float trim,
			/* used in Scherk */  float zbase,  float zoffset,
			/* used in Collins */ float ringrad, float archrad, float thetab, float thwarp);


/*========================================================
  BUILD BASE-ARRAY
  __________________________________________________________
  First, we build the generic template array of vertex and texture coordinates.
  In this array we maintain all quantities that can be reused
  and can easily be transformed when copies are made for
  the other branches and other storeys.
  ---------------------------------------------------------*/

static void basearray(int branches, int ntext, float height,  
		      float flange, float twirl, float thick, float trim)
{
  int i, j, k;
  float squashfactor, alfa, beta, gamma, kappa; 
  float hj, tj, zj, rj, r2, mi, mi2, fl, flt, fl2, flt2;

  fl = flange*sqrt(2);
  flt = (flange+thick*trim) *sqrt(2);
  fl2 = fl*fl;
  flt2 = flt*flt;

  /*----------------------------------------------------------
    CALCULATE SCHERK VERTEX and TEXTURE COORDINATES 
    We calculate all the vertex coordinates from the
    closed form expressions that approximate Scherk's surface
    with horizontal hyperbolical slices with suitable
    head-to-origin distances to make vertical elliptical openings.
    The extra columns of vertices on the left and right
    are to provide for a thin rim if the surface has thickness.
    Because array indices cannot be negative, there are some
    not-so-intuitive expressions below on the right hand side.
    The base geometry is generated symmetrically around the level z=0,
    but is then shifted in z to get the first storey rising up from z=0.
    The base surface branch opens symmetrically towards the +x axis.
    
    The rim points are generated as part of the regular patch,
    even if they are not needed. It is fast, simplifies the code,
    and it prevents garbage in these array locations.
    
    The normal texture coordinates count do NOT include the rim.
    ---------------------------------------------------------*/


  /*----------------------------------------------------------
    First calculate the texture coordinates, and the z- coordinates,
    since this is simple and they are the same for all base patches.
    ---------------------------------------------------------*/

  for (j=0; j<=dd; j++)		 /* bottom to top of patch */
    {
      if (branches==1)
        h[j] = (float)(j-d)/(float)(d);	/* linear */
      else
        h[j] = sin( 0.5*M_PI*(float)(j-d)/(float)(d) );
      /* gives denser sampling near saddle point */
      zj = height * (1 + h[j]); /* offset by half a storey */
      tj = ntext*0.5 * (1 + h[j]);
      
      for (i=1; i<=Dd; i++)	 /* all non-rim points */
	{
	  v[i][j][Z] = zj;
	  t[i][j][Y] = tj;
	  t[i][j][X] = ntext*(float)(i-1)/(float)(dd);
	}
      v[0][j][Z] = zj;		 /* left rim */
      t[0][j][Y] = tj;
      t[0][j][X] = t[1][j][X]-0.5;
      i=DD;
      v[i][j][Z] = zj;		 /* right rim */
      t[i][j][Y] = tj;
      t[i][j][X] = t[i-1][j][X]+0.5;

      /* Use a new special rim texture with with this 0.5 extension.
         (Originally I used only a small texture coordiate extension
	 for rim points so that only a small fraction of the
	 regular face texture pattern is stretched over the rim.) */
    }

  /*----------------------------------------------------------
    Now deal with the x- and y- coordinates.
    ---------------------------------------------------------*/

  if (branches==1)		 /* Special case: do just a flat panel */
    /* Give it a 90-degree fold so it can be treated like other branches. */
    {
      for (j=0; j<=dd; j++)
	{
	  v[0][j][X] = (flange + trim*thick*sqrt(0.5));     /* left RIM point */
	  for (i=1; i<=D; i++)	 /* left half */
	      v[i][j][X] = flange*(float)(D-i)/(float)(d);
	  for (i=0; i<=D; i++)	 /* left half */
	      v[i][j][Y] = -v[i][j][X];

	  for (i=D+1; i<DD; i++) /* right half */
	        v[i][j][X] = flange*(float)(i-D)/(float)(d);
	  v[DD][j][X] = (flange + trim*thick*sqrt(0.5));     /* right RIM point */
	  for (i=D+1; i<=DD; i++) /* right half */
	      v[i][j][Y] = v[i][j][X];
	}
    } /* flat panel */

  else				/* do the basic hyperbolic saddle surface */
    {
      for (j=0; j<=d; j++)	 /* lower half of patch */
	{
	  hj =  h[j];		 /* normalized height */
	  rj = sqrt(1.0 - hj*hj);
	  r2 = rj*rj;
	  
	  for (i=2; i<=D; i++)
	    {
	      /* compute vx, vy [i][j] from hyperbola formula */
              /* Watch out for the singularity for m=1 !! */
	      mi = -(float)(i-D)/(float)(d);
	      mi2 = mi*mi;

	      v[i][j][X]=(-mi2*fl+sqrt(mi2*(fl2-r2)+r2))/(1.0-mi2);
	      v[i][j][Y]= mi*(v[i][j][X] - fl);
	    }			

	  v[1][j][X] = (fl2+r2)/(2*fl);
	  v[1][j][Y] = (v[1][j][X] - fl);

	  v[0][j][X] = (flt2+r2)/(2*flt);
	  v[0][j][Y] = (v[0][j][X] - flt);

	  /* ---------------------------------------------------
             (Later I may also experiment with a cubic curve,
	     so that the endpoints of the curve can lie on the
	     diagonal, thereby leading to a straight rim.)
             ---------------------------------------------------  */

	  /* COPY left half of array flipped to right */
	  for (i=D+1; i<=DD; i++)
	    {
	      v[i][j][X] = v[DD-i][j][X];
	      v[i][j][Y] = -v[DD-i][j][Y];
	    }
	}			 /* end: for j */
      
      for (j=d+1; j<=dd; j++)	 /* COPY lower half flipped up */
	{
	  for (i=0; i<=DD; i++)
	    {
	      v[i][j][X] = v[i][dd-j][X];
	      v[i][j][Y] = v[i][dd-j][Y];
	    }
	}
    }				/* The template patch is built ! */
  /* RIM points have been taken care of in main loop above. */


  /* make a second array of negated (mirrored) texture coordinates 
     for non-flipped inner surfaces. */
  for (j=0; j<=dd; j++)
    {
      for (i=0; i<=DD; i++)
	{
	  mt[i][j][X] = (float)(DD)-t[i][j][X];
	  mt[i][j][Y] = t[i][j][Y];
	}
    }

  /*--------------------------------------------------------
    ADJUST OPENINGS OF HYPERBOLAS for multi-branch Scherks.
    Step through the basearray and modify the x & y-coordinates.
    Find angle {beta} in polar coordinates, and adjust it.
    The original opening is 90 degrees; it must be 180/branches;
    this yields a correction factor: (180/branches)/90;  
    move this unit by (f-1); then ADD in TWIRL rotations.
    TWIRL  is the twist per storey; is calculated by the caller.
    Step through the basearray and modify x & y-coordinates.
    ---------------------------------------------------------*/

  squashfactor = (2.0/(float)(branches) - 1.0);
  
  for (j=0; j<=dd; j++) 
    {
      alfa = twirl*0.5*(1 + h[j]); /* local adjust for twist */
	
      
      for (i=0; i<=DD; i++)
	{	
	  if (v[i][j][X]==0)
	    beta = 0.0;		 /* angle with x-axis*/
	  else
	    beta = atan(v[i][j][Y]/v[i][j][X])*180.0/M_PI;  /* degrees */
	  gamma = beta*squashfactor + alfa;   /* narrow the hyporbolas */
	  arot( v[i][j], 'z', gamma );   /* rotate each point around z */
	}
    }

  /*--------------------------------------------------------
    For the Scherk surface, it is easy to compute the surface normal vectors
    and the offset points for the thick surface in the template array;
    since they will not be affected by the rotations around
    and the shifting along the z-axis.
    But in the toroidal warp the normals do NOT just change
    in a rigid-body motion with the underlying surface;
    they need a more careful adjustment.
    
    However, we can compute the NORMAL information
    in the base array, and then also transform them with the WARP.
    We also gain consistency between the seams of
    different stories with this new approach, because of the
    inherent point-symmetry of the base array.
    In most places we average the face normals of the FOUR
    quadrilaterals surrounding the given vertex;
    but near the top and bottom edges of the surface 
    we just average TWO crossproducts of edges.
    At critical points we make a_priory normal calculations.
    The normal direction is calculated for an OUTER surface.
    ---------------------------------------------------------*/

  j=0;				 /*bottom band*/  /* excluding rim points */
  for (i=1; i<D; i++)		 /*left part of bottom seam*/
    {				 /* compute normal  from 3 near neighbors */
      edgenorm(i, j, 0 );
    }
    
  i=D;				 /*saddle point*/
  if (branches==1)
    {
      m[i][j][X]=1.0;
      m[i][j][Y]=0.0;
      m[i][j][Z]=0.0;
    }
  else
    {				 /* compute normal to point along the -z-axis */
      m[i][j][X]=0.0;
      m[i][j][Y]=0.0;
      m[i][j][Z]=-1.0;
    }
    
  for (i=D+1; i<=Dd; i++)	 /*right part of bottom seam*/
    {				 /* compute normal  from 3 near neighbors */
      edgenorm(i, j, 0 );
    }
  

  for (j=1; j<dd; j++)		 /*non-extreme bands*/
    { for (i=1; i<=Dd; i++)	 /*inner points*/
	{			 /* compute normal  from 4 near neighbors */
	  avnorm4(i, j);
	}
      }

  j=dd;				 /*top band*/
  for (i=1; i<D; i++)		 /*left part of top seam*/
    {				 /* compute normal  from 3 near neighbors */
      edgenorm(i, j, 1 );
    }
    
  i=D;				 /*saddle point*/
  if (branches==1)
    {
      m[i][j][X]=1.0;
      m[i][j][Y]=0.0;
      m[i][j][Z]=0.0;
      arot( m[i][j], 'z', twirl ); /* adjust normal */
    }
  else
    /* compute normal to lie on positive z-axis */
    {
      m[i][j][X]=0.0;
      m[i][j][Y]=0.0;
      m[i][j][Z]=1.0;
    }
     
  for (i=D+1; i<=Dd; i++)	 /*right part of top seam*/
    {				 /* compute normal  from 3 near neighbors */
      edgenorm(i, j, 1 );
    }
  

  /*--------------------------------------------------------
    Calculate the central RIM NORMALS.
    First I just aimed them in the +/-x, +/-y direction.
    But this does not work, since we have already added the twist !
    Instead I point them in the direction of the edges from the NN vertex 
    within the thin idealized surface.
    ---------------------------------------------------------*/

  for (j=0; j<=dd; j++)
    {
      /* left rim */
      m[0][j][X]=v[1][j][X]-v[2][j][X];           /* changed to inner point 0->1 */
      m[0][j][Y]=v[1][j][Y]-v[2][j][Y];
      m[0][j][Z]=v[1][j][Z]-v[2][j][Z];
      normalize(m[0][j]);

      /* right rim */
      m[DD][j][X]=v[Dd][j][X]-v[DD-2][j][X];
      m[DD][j][Y]=v[Dd][j][Y]-v[DD-2][j][Y];
      m[DD][j][Z]=v[Dd][j][Z]-v[DD-2][j][Z];
      normalize(m[DD][j]);
    }
      
  /*-------------------------------------------------------
   Define a generic array for the more varied  RIM PROFILEs
   Compute two arrays for "[X]" and "[Y]"
   that describe the distortion of a straight rim,
   i.e., the components of the offset and of the normals.
   ------------------------------------------------------*/

  if (trim==0.0)        /* flat rim rpofile */
    {
      kmax = 1;

      wbulge[0] = 0.0;
      tbulge[0] = 0.0;
      wbulge[kmax] = 0.0;
      tbulge[kmax] = -2.0 * scherk_thickness;

      wrimnorm[0] = 1.0;
      trimnorm[0] = 0.0;
      wrimnorm[kmax] = 1.0;
      trimnorm[kmax] = 0.0;
    }

  else if ((trim>0.0) && (trim<=0.1))   /* curved profile with sharp edges */
    {
      kmax = 2;

      wbulge[0] = 0.0;
      tbulge[0] = 0.0;
      wbulge[1] = 1.0 * thick * trim;
      tbulge[1] = -1.0 * thick;
      wbulge[kmax] = 0.0;
      tbulge[kmax] = -2.0 * thick;

      wrimnorm[0] = 1.0 - trim;
      trimnorm[0] = trim * trim * thick * trim;
      wrimnorm[1] = 1.0;
      trimnorm[1] = 0.0;
      wrimnorm[kmax] = 1.0 - trim;
      trimnorm[kmax] = -trim * trim * thick * trim;
    }

  else if ((trim>0.0) && (trim<=1.0))   /* triangular rim profile */
    {
      kmax = 3;  /* double point in center to make sharp edge */

      wbulge[0] = 0.0;
      tbulge[0] = 0.0;
      wbulge[1] = 1.0 * thick * trim;
      tbulge[1] = -0.98 * thick;
      wbulge[2] = 1.0 * thick * trim;
      tbulge[2] = -1.02 * thick;
      wbulge[kmax] = 0.0;
      tbulge[kmax] = -2.0 * thick;

      wrimnorm[0] = 1.0;
      trimnorm[0] = trim;
      wrimnorm[1] = 1.0;
      trimnorm[1] = trim;
      wrimnorm[2] = 1.0;
      trimnorm[2] = -trim;
      wrimnorm[kmax] = 1.0;
      trimnorm[kmax] = -trim;
    }

  else    /* semicircular or elliptical profile */
    {
      if (d<=3)
	kmax = 3;
      else if (d>=2*KMID)
	kmax = 2*KMID;
      else
	kmax = d;

      for (k=0; k<=kmax; k++)
        {
          kappa = k*M_PI/(kmax);  /*evenly divided half circle*/
    
          /* offset in flange/normal -directions */
          wbulge[k] = thick * trim * sin(kappa);
          tbulge[k] = thick * (cos(kappa)-1.0);  /* move pt against normal */
    
          /* normal components, properly weighted for elliptic profile */
          wrimnorm[k] = sin(kappa);
          trimnorm[k] = trim * cos(kappa);
        }
    }

  
  /*----------------------------------------------------------
    Make a texture coordinate array for the various lids.
    ---------------------------------------------------------*/
  for (k=0; k<=kmax; k++)
    {
      tj = (float)(k)/(float)(kmax);
      for (i=1; i<=Dd; i++)
        {
          tlid[i][k][X] = ntext*(float)(i-1)/(float)(dd);
          tlid[i][k][Y] = tj;
        }
    }
  
  /*----------------------------------------------------------
    Make a texture coordinate array for the rims.
    ---------------------------------------------------------*/
  for (k=0; k<=kmax; k++)
    {
      tj = (float)(k)/(float)(kmax);
      for (j=0; j<=dd; j++)
        {
          rimt[j][k][X] = ntext*0.5 * (1 + h[j]);
          rimt[j][k][Y] = tj;
        }
    }

}   /* end: basearray---------------------- */


/*========================================================
  AVERAGE VERTEX NORMALS
  __________________________________________________________
  Find the edges pointing to all regular nearest neighbors
  in the basic quadrilateral i,j grid. Form cross products
  in the regular quadrants, and average these normals.
  ---------------------------------------------------------*/

static void avnorm4( int i,  int j )
{
  float norm1[3], norm2[3], norm3[3], norm4[3];
  float edown[3], eright[3], eup[3], eleft[3];

  /* calculate neighbor edge vectors */
  edown[X] = v[i][j-1][X] - v[i][j][X];
  edown[Y] = v[i][j-1][Y] - v[i][j][Y];
  edown[Z] = v[i][j-1][Z] - v[i][j][Z];
  
  eup[X] = v[i][j+1][X] - v[i][j][X];
  eup[Y] = v[i][j+1][Y] - v[i][j][Y];
  eup[Z] = v[i][j+1][Z] - v[i][j][Z];

  eleft[X] = v[i-1][j][X] - v[i][j][X];
  eleft[Y] = v[i-1][j][Y] - v[i][j][Y];
  eleft[Z] = v[i-1][j][Z] - v[i][j][Z];

  eright[X] = v[i+1][j][X] - v[i][j][X];
  eright[Y] = v[i+1][j][Y] - v[i][j][Y];
  eright[Z] = v[i+1][j][Z] - v[i][j][Z];

  /* Calculate the 4 surrounding normals suitable for outer surface*/
  cross_product(eright, eup, norm1);
  cross_product(eup, eleft, norm2);
  cross_product(eleft, edown, norm3);
  cross_product(edown, eright, norm4);
 
  /* find the average normal at location [i][j] */
  m[i][j][X] = (norm1[X] + norm2[X] + norm3[X] + norm4[X]);
  m[i][j][Y] = (norm1[Y] + norm2[Y] + norm3[Y] + norm4[Y]);
  m[i][j][Z] = (norm1[Z] + norm2[Z] + norm3[Z] + norm4[Z]);
  normalize(m[i][j]);
}


/*--------------------------------------------------------------
  Since there are only two cases: upper and lower edges,
  we use this less general function in which we do
  the left and right edge vectors always, up or down conditionally;
  then choose the right pair of cross-products.
  ----------------------------------------------------------------*/

static void edgenorm( int i,  int j,  int topedge )
{
  float norm1[3], norm2[3], norm3[3], norm4[3];
  float edown[3], eright[3], eup[3], eleft[3];

  /* create edge vectors to existing nearest neighbor vertices */
  eleft[X] = v[i-1][j][X] - v[i][j][X];
  eleft[Y] = v[i-1][j][Y] - v[i][j][Y];
  eleft[Z] = v[i-1][j][Z] - v[i][j][Z];
    
  eright[X] = v[i+1][j][X] - v[i][j][X];
  eright[Y] = v[i+1][j][Y] - v[i][j][Y];
  eright[Z] = v[i+1][j][Z] - v[i][j][Z];

  if (topedge)
    { edown[X] = v[i][j-1][X] - v[i][j][X];
      edown[Y] = v[i][j-1][Y] - v[i][j][Y];
      edown[Z] = v[i][j-1][Z] - v[i][j][Z];
    }
  else
    { eup[X] = v[i][j+1][X] - v[i][j][X];
      eup[Y] = v[i][j+1][Y] - v[i][j][Y];
      eup[Z] = v[i][j+1][Z] - v[i][j][Z];
    }
  
  /* create cross-product normal vectors for facets */
  if (topedge)
    {
      cross_product(eleft, edown, norm3);
      cross_product(edown, eright, norm4);
      m[i][j][X] = norm3[X] + norm4[X];
      m[i][j][Y] = norm3[Y] + norm4[Y];
      m[i][j][Z] = norm3[Z] + norm4[Z];
    }
  else
    {
      cross_product(eright, eup, norm1);
      cross_product(eup, eleft, norm2);
      m[i][j][X] = norm1[X] + norm2[X];
      m[i][j][Y] = norm1[Y] + norm2[Y];
      m[i][j][Z] = norm1[Z] + norm2[Z];
    }

  normalize(m[i][j]);

} /* end edgenorm() -------------------------------------------*/



/*-----------------------------------------------------------------
  Draw face normals at a vertex.
  Draw a linesegment from the point vert[] in the direction norm[].
  It is assumed that norm is normalized, and that
  this function call is _not_ between a pair of GL begin/end calls.
  ---------------------------------------------------------------*/

static void drawnormal(float vert[], float norm[])
{
  const long norm_color = 0xffff00ff; /* magenta */
  const float norm_factor = 0.3;
  float vert2[3];
  
  /* find the other point of the line extended by the normal */
  vert2[X] = vert[X] + norm_factor*norm[X];
  vert2[Y] = vert[Y] + norm_factor*norm[Y];
  vert2[Z] = vert[Z] + norm_factor*norm[Z];
  
  cpack(norm_color);
  
  bgnline();
  v3f(vert);
  v3f(vert2);
  endline();

}/*end drawnormal() ---------------------------------------*/



/*-------------------------------------------------------
  Load the appropriate material and texture definition
  for the face (rim==0)  or for the rim (rim==1)  t-meshes.
  One can color 2-sided surfaces differently, using numbr;
  one can also employ up to 4 different rim materials.
  --------------------------------------------------------*/
static void loadmaterial( int rim, int numbr)
{ 
  /* bind RIM material and texture */
  if (rim)
    {
      numbr=numbr%4;   /* To digest more then four different edges */
      switch(numbr)
        {
	case 1:
	  {
	    lmbind(MATERIAL, SCHERK_RIM1_MATERIAL);
	    lmbind(BACKMATERIAL, SCHERK_RIM1_MATERIAL);
	    if (gstate.texmap)
	      {
		texbind(TX_TEXTURE_0, SCHERK_RIM1_TEXTURE);
	      }
	    break;
	  }
	case 2:
	  {
	    lmbind(MATERIAL, SCHERK_RIM2_MATERIAL);
	    lmbind(BACKMATERIAL, SCHERK_RIM2_MATERIAL);
	    if (gstate.texmap)
	      {
		texbind(TX_TEXTURE_0, SCHERK_RIM2_TEXTURE);
	      }
	    break;
	  }
	case 3:
	  {
	    lmbind(MATERIAL, SCHERK_RIM3_MATERIAL);
	    lmbind(BACKMATERIAL, SCHERK_RIM3_MATERIAL);
	    if (gstate.texmap)
	      {
		texbind(TX_TEXTURE_0, SCHERK_RIM3_TEXTURE);
	      }
	    break;
	  }
	case 4:
	  {
	    lmbind(MATERIAL, SCHERK_RIM4_MATERIAL);
	    lmbind(BACKMATERIAL, SCHERK_RIM4_MATERIAL);
	    if (gstate.texmap)
	      {
		texbind(TX_TEXTURE_0, SCHERK_RIM4_TEXTURE);
	      }
	    break;
	  }
        }
    }
  else		/* face colors */
    {
      switch(numbr)
        {
	case 1:
	  {
	    lmbind(MATERIAL, SCHERK_FACE1_MATERIAL);
	    lmbind(BACKMATERIAL, SCHERK_FACE1_MATERIAL);
	    if (gstate.texmap)
	      {
		texbind(TX_TEXTURE_0, SCHERK_FACE1_TEXTURE);
	      }
	    break;
	  }
	case 2:
	  {
	    lmbind(MATERIAL, SCHERK_FACE2_MATERIAL);
	    lmbind(BACKMATERIAL, SCHERK_FACE2_MATERIAL);
	    if (gstate.texmap)
	      {
		texbind(TX_TEXTURE_0, SCHERK_FACE2_TEXTURE);
	      } break;
	  }
        }
    }
}			/*end loadmaterial ------------------------------------*/


/*========================================================
  CONSTRUCT ONE SCHERK/COLLINS SURFACE BRANCH
  __________________________________________________________
  For every instance of a thick branch of the sculpture,
  we start from the computed template basearray,
  apply the torroidal warp (if necessary -- for Collins),
  then adjust the initial surface normals into a new array,
  n[i][j][3], and then fill in the offset arrays of vertices.
  When the o[i][j] array has been suitably filled in,
  we read out all vertex info for a complete T-mesh
  from the various arrays.
  ---------------------------------------------------------*/

static int surfbranch(int flat,  int ring, int stor,  
		      int ntext, float height,  float flange,  float thick,
                      int branch, float azimb,  float twirl,  float trim,
		      /* used in Scherk */  float zbase,  float zoffset,
		      /* used in Collins */ float ringrad, float archrad, float thetab, float thwarp)
{
  int i, j, k, numbr;
  int do_upper, do_lower;
  float thetaj, trimbar, oddstor, step;
  float nlid[DMAX+DMAX+3][3];   /* normal directions in center-line of lids  */

  oddstor=(odd(stor));
  trimbar = (trim<1.0 ? 1.0-trim : 0.0 );

  if(stor<=scherk_storeys-1)
    do_upper=1;                                /* do top half */
  else
    do_upper=0;

  if(stor>0)
    do_lower=1;                              /* do bottom half */
  else
    do_lower=0;

  /*--------------------------------------------------------
    FILL IN THE ACTUAL ARRAY o[i][j] OF VERTEX INFO
    from which the T-mesh will be read out.
    First those operations common to Scherk and Collins:
    ---------------------------------------------------------*/
  for (j=0; j<=dd; j++) 
    {
      for (i=0; i<=DD; i++)
	{	
  	  o[i][j][X] = v[i][j][X];
  	  o[i][j][Y] = v[i][j][Y];
  	  o[i][j][Z] = 0.0;
	  arot( o[i][j], 'z', azimb );
	  /* additional base rotation for this surface branch */
	  n[i][j][X] = m[i][j][X];
  	  n[i][j][Y] = m[i][j][Y];
	  n[i][j][Z] = m[i][j][Z];
	  arot( n[i][j], 'z', azimb );
	  /* a simple rotation of the normals is sufficient */
	}	
    }
  /*--------------------------------------------------------
    Scherk-specific adjustments: (just a straight tower)
    ---------------------------------------------------------*/
  if (!ring)
    for (j=0; j<=dd; j++) 
      for (i=0; i<=DD; i++)
	{	
  	  o[i][j][Z] = v[i][j][Z] + zbase;
	  /* additional base offset for Scherk tower */
	  /* no further adjustment of the normals is necessary */
	}	
  /*--------------------------------------------------------
    Collins-specific adjustments:
    ---------------------------------------------------------*/
  else
    for (j=0; j<=dd; j++) 
      {
	thetaj = thwarp*0.5*(1 + h[j]);
        for (i=0; i<=DD; i++)
	  {	
  	    o[i][j][X] += archrad;

	    /* Now we need to prewarp the normals in the base array
	       for the expected stretching and squashing of the tower
	       outside/inside the median ring-radius when we bend it.
	       Compute inverse stretchig op in the z-direction:
	       stretch = x_coord /archrad; inverse = reciprocal */
	    
            n[i][j][Z] = n[i][j][Z]*archrad/o[i][j][X];
            normalize(n[i][j]);
	    arot( o[i][j], 'y', thetab+thetaj );
            /* If RING:  APPLY TORROIDAL BEND NOW:
               Z maps into angle theta along the toroidal ring. */

	    arot( n[i][j], 'y', thetab+thetaj );
	    /* also do the corresponding rotation of the normals */

            /* Move the sculpture back into central position;
               this depends on what fraction of a full toroid that it forms:
               -- Till 90 degrees: keep root at origin;
               -- Above 180 degrees: keep center of ring at origin;
               -- In between, linearly interpolate offset between above 2 values. */

  	    o[i][j][Z] +=  zoffset;
  	    if(scherk_warp<=360.0)
                o[i][j][X] += -archrad + ringrad*scherk_warp/360.0;
            else 
                o[i][j][X] += -archrad + ringrad*360.0/scherk_warp;

            /* old code
  	    if(scherk_warp<90.0)
                o[i][j][X] -= archrad;
            else if (scherk_warp<180.0)
                o[i][j][X] -= archrad * (180.0 - scherk_warp)/90.0;    */
            /* old code
                o[i][j][X] -= archrad - x_base_offset
                x_base_offset = archrad*0.25*(1.0 + cos(scherk_warp) + 2.0*cos(scherk_warp/2.0) )
                   {according to Laura} */
	  }	
      }

  /*--------------------------------------------------------
    Compute offset vertices for OUTER TMESH.
    ---------------------------------------------------------*/
  for (j=0; j<=dd; j++)
    for (i=1; i<DD; i++) 
      {
        /* outside mesh: move point WITH normal */
        o[i][j][X] += thick * n[i][j][X];
        o[i][j][Y] += thick * n[i][j][Y];
        o[i][j][Z] += thick * n[i][j][Z];
      } 
      /* rim points are not moved; they are already set in base array, depending on thick*trim  */

  /*--------------------------------------------------------
    DO OUTER FACE TMESH: read out Triangle-strips horizontally.
    ---------------------------------------------------------*/

  numbr = (oddstor ? 2 : 1);
  loadmaterial( 0, numbr);

  if(do_lower)
   {
    for (j=0; j<d; j++ )		 /* bottom half */
     {		/* do this in 4 quadrants, to get nicer junction at saddle point:
		   	        have 8 triangle come together to deal with warp. */
      bgntmesh();
      for (i=Dd; i>=D; i--)  /* right half plus one triangle */
	{
          if (gstate.envmap)
              EnvMapUV(n[i][j], cm);
          else
              t2f(t[i][j]);
	  n3f(n[i][j]);
	  v3f(o[i][j]);
	  if (gstate.envmap)
              EnvMapUV(n[i][j+1], cm);
          else
              t2f(t[i][j+1]);
	  n3f(n[i][j+1]);
	  v3f(o[i][j+1]);
	}
      
      i=D-1;
          if (gstate.envmap)
              EnvMapUV(n[i][j+1], cm);
          else
              t2f(t[i][j+1]);
          n3f(n[i][j+1]); 
          v3f(o[i][j+1]); 
      /* This extra triangle is needed to make things work out
	 with diagonal direction and face orientation */
      endtmesh();

      bgntmesh();
      for (i=D; i>=2; i--)  /* left half minus one triangle */
	{
	  if (gstate.envmap)
              EnvMapUV(n[i][j], cm);
          else
              t2f(t[i][j]);
	  n3f(n[i][j]);
	  v3f(o[i][j]);
	  if (gstate.envmap)
              EnvMapUV(n[i-1][j+1], cm);
          else
              t2f(t[i-1][j+1]);
	  n3f(n[i-1][j+1]);
	  v3f(o[i-1][j+1]);
	}
      /* i=last_i - 1 */
      i=1;
      if (gstate.envmap)
          EnvMapUV(n[i][j], cm);
      else
          t2f(t[i][j]);
      n3f(n[i][j]);
      v3f(o[i][j]);
      endtmesh();
    }

      if (out_data)
        {
          calc_bbox();
          output_surface(0, 0, 1+10*branch+100*stor);
        }
  }
  
  if(do_upper)
   {
    for (j=dd; j>d; j-- )
     {				/* second half from top down */
      bgntmesh();
      for (i=1; i<=D; i++)	/* left half plus one triangle */
	{
	  if (gstate.envmap)
              EnvMapUV(n[i][j], cm);
          else
              t2f(t[i][j]);
	  n3f(n[i][j]);
	  v3f(o[i][j]);
	  if (gstate.envmap)
              EnvMapUV(n[i][j-1], cm);
          else
              t2f(t[i][j-1]);
	  n3f(n[i][j-1]);
	  v3f(o[i][j-1]);
	}
      i=D+1;
      if (gstate.envmap)
          EnvMapUV(n[i][j-1], cm);
      else
          t2f(t[i][j-1]);
      n3f(n[i][j-1]);
      v3f(o[i][j-1]);
      endtmesh();

      bgntmesh();
      for (i=D; i<=dd; i++)	/* right half minus one triangle */
	{
	  if (gstate.envmap)
              EnvMapUV(n[i][j], cm);
          else
              t2f(t[i][j]);
	  n3f(n[i][j]);
	  v3f(o[i][j]);
	  if (gstate.envmap)
              EnvMapUV(n[i+1][j-1], cm);
          else
              t2f(t[i+1][j-1]);
	  n3f(n[i+1][j-1]);
	  v3f(o[i+1][j-1]);
	}
      i=dd+1;
      if (gstate.envmap)
          EnvMapUV(n[i][j], cm);
       else
          t2f(t[i][j]);
      n3f(n[i][j]);
      v3f(o[i][j]);
      endtmesh();
    }

      if (out_data)
        {
          calc_bbox();
          output_surface(0, 1, 2+10*branch+100*stor);
        }

  } /* END UPPER OUTER SURFACE */


/*=================== RIM SURFACES ====================*/
  

  /*---------------------------------------------------------
    Create points on SMOOTH RIM-BULGE with kmax points across
    --------------------------------------------------------*/
      /*
      numbr = branch+branch;
      numbr = (oddstor ? numbr+1 : numbr);
      numbr = numbr%vanes;
      */
      numbr = 1;
      loadmaterial( 1, numbr);

    /* make LEFT RIM points */
    i=1;
    for (j=0; j<=dd; j++) 
      for (k=0; k<=kmax; k++ ) 
        {
          lid[j][k][X]= o[1][j][X] + tbulge[k]*n[1][j][X] + wbulge[k]*n[0][j][X];
          lid[j][k][Y]= o[1][j][Y] + tbulge[k]*n[1][j][Y] + wbulge[k]*n[0][j][Y];
          lid[j][k][Z]= o[1][j][Z] + tbulge[k]*n[1][j][Z] + wbulge[k]*n[0][j][Z];
        
          /* normals */
          nrim[j][k][X]= trimnorm[k]*n[1][j][X] + wrimnorm[k]*n[0][j][X];
          nrim[j][k][Y]= trimnorm[k]*n[1][j][Y] + wrimnorm[k]*n[0][j][Y];
          nrim[j][k][Z]= trimnorm[k]*n[1][j][Z] + wrimnorm[k]*n[0][j][Z];
          normalize( nrim[j][k] );
        }


  if(do_upper)
    {
      for (k=1; k<=kmax; k++ )  /* make longitudinal t-strips */
        {
          bgntmesh();
          for (j=d; j<=dd; j++)	
            {
              if (gstate.envmap)
                  EnvMapUV(nrim[j][k], cm);
              else
                  t2f(rimt[j][k]);
	      n3f(nrim[j][k]);
	      v3f(lid[j][k]);
    
              if (gstate.envmap)
                  EnvMapUV(nrim[j][k-1], cm);
              else
                  t2f(rimt[j][k-1]);
	      n3f(nrim[j][k-1]);
	      v3f(lid[j][k-1]);
            }
          endtmesh();
        }
     
      if(ntext==0)
          for (k=0; k<=kmax; k++)
            for (j=d; j<=dd; j++) 
	      drawnormal(lid[j][k], nrim[j][k] );

      if(out_data)
        {
          calc_bbox();
          output_lid(d,0,dd,kmax,0,1);
        }
    }

  if(do_lower)
    {
      for (k=1; k<=kmax; k++ )  /* make longitudinal t-strips */
        {
          bgntmesh();
          for (j=0; j<=d; j++)	
            {
              if (gstate.envmap)
                  EnvMapUV(nrim[j][k], cm);
              else
                  t2f(rimt[j][k]);
	      n3f(nrim[j][k]);
	      v3f(lid[j][k]);
    
              if (gstate.envmap)
                  EnvMapUV(nrim[j][k-1], cm);
              else
                  t2f(rimt[j][k-1]);
	      n3f(nrim[j][k-1]);
	      v3f(lid[j][k-1]);
            }
          endtmesh();
        }

      if(ntext==0)
          for (k=0; k<=kmax; k++)
            for (j=0; j<=d; j++) 
	      drawnormal(lid[j][k], nrim[j][k] );

      if(out_data)
        {
          calc_bbox();
          output_lid(0,0,d,kmax,0,2);
        }
    }


   i=Dd;  /* RIGHT RIM */
     for (k=0; k<=kmax; k++ ) 
       for (j=0; j<=dd; j++) 
        {
          lid[j][k][X]= o[Dd][j][X] + tbulge[k]*n[Dd][j][X] + wbulge[k]*n[DD][j][X];
          lid[j][k][Y]= o[Dd][j][Y] + tbulge[k]*n[Dd][j][Y] + wbulge[k]*n[DD][j][Y];
          lid[j][k][Z]= o[Dd][j][Z] + tbulge[k]*n[Dd][j][Z] + wbulge[k]*n[DD][j][Z];

          /* normals */
          nrim[j][k][X]= trimnorm[k]*n[Dd][j][X] + wrimnorm[k]*n[DD][j][X];
          nrim[j][k][Y]= trimnorm[k]*n[Dd][j][Y] + wrimnorm[k]*n[DD][j][Y];
          nrim[j][k][Z]= trimnorm[k]*n[Dd][j][Z] + wrimnorm[k]*n[DD][j][Z];
          normalize( nrim[j][k] );
        }
   
 if(do_upper)
    {
      for (k=1; k<=kmax; k++ )  /* make longitudinal t-strips */
        {
          bgntmesh();
          for (j=d; j<=dd; j++)	
            {
              if (gstate.envmap)
                  EnvMapUV(nrim[j][k-1], cm);
              else
                  t2f(rimt[j][k-1]);
	      n3f(nrim[j][k-1]);
	      v3f(lid[j][k-1]);
    
              if (gstate.envmap)
                  EnvMapUV(nrim[j][k], cm);
              else
                  t2f(rimt[j][k]);
	      n3f(nrim[j][k]);
	      v3f(lid[j][k]);
            }
          endtmesh();
        }
     
      if(ntext==0)
          for (k=0; k<=kmax; k++)
            for (j=d; j<=dd; j++) 
	      drawnormal(lid[j][k], nrim[j][k] );

      if(out_data)
        {
          calc_bbox();
          output_lid(d,0,dd,kmax,1,3);
        }
    }

 if(do_lower)
    {
      for (k=1; k<=kmax; k++ )  /* make longitudinal t-strips */
        {
          bgntmesh();
          for (j=0; j<=d; j++)	
            {
              if (gstate.envmap)
                  EnvMapUV(nrim[j][k-1], cm);
              else
                  t2f(rimt[j][k-1]);
	      n3f(nrim[j][k-1]);
	      v3f(lid[j][k-1]);
    
              if (gstate.envmap)
                  EnvMapUV(nrim[j][k], cm);
              else
                  t2f(rimt[j][k]);
	      n3f(nrim[j][k]);
	      v3f(lid[j][k]);
            }
          endtmesh();
        }

      if(ntext==0)
          for (k=0; k<=kmax; k++)
            for (j=0; j<=d; j++) 
	      drawnormal(lid[j][k], nrim[j][k] );

      if(out_data)
        {
          calc_bbox();
          output_lid(0,0,d,kmax,1,4);
        }
    }



  /*--------------------------------------------------------
    SHOW FACE NORMALS ON OUTER SURFACE: HACK: when "0-texture"
  ---------------------------------------------------------*/
 if(ntext==0)
   {
     if(do_upper)
       for (j=d; j<=dd; j++)
         for (i=0; i<=DD; i++) 
	   drawnormal(o[i][j], n[i][j] );
  
     if(do_lower)
       for (j=0; j<=d; j++)
         for (i=0; i<=DD; i++) 
	   drawnormal(o[i][j], n[i][j] );
   }


  b_count++;


/*------- Make the extra LIDS at the cuts at the half-storey ends ----*/

  if ((scherk_warp != 360)&&(scherk_warp != 720)&&(scherk_warp != 1080)) /* not a ring */
   {
    if ( (stor==scherk_storeys) || (stor==0) )
     {
      /* Calculate extra vertices for the lid */
      for (k=0; k<=kmax; k++)
        {
          step = k*2.0*thick/kmax;
          for (i=2; i<Dd; i++)	/* all but rim points get intermediary */
            {
              lid[i][k][X] =  o[i][d][X] - step * n[i][d][X];
              lid[i][k][Y] =  o[i][d][Y] - step * n[i][d][Y];
              lid[i][k][Z] =  o[i][d][Z] - step * n[i][d][Z];
            }

          /* make points for endcaps */
          lid[1][k][X]= o[1][d][X] + tbulge[k]*n[1][d][X] + wbulge[k]*n[0][d][X];
          lid[1][k][Y]= o[1][d][Y] + tbulge[k]*n[1][d][Y] + wbulge[k]*n[0][d][Y];
          lid[1][k][Z]= o[1][d][Z] + tbulge[k]*n[1][d][Z] + wbulge[k]*n[0][d][Z];
	  
          lid[Dd][k][X]= o[Dd][d][X] + tbulge[k]*n[Dd][d][X] + wbulge[k]*n[DD][d][X];
          lid[Dd][k][Y]= o[Dd][d][Y] + tbulge[k]*n[Dd][d][Y] + wbulge[k]*n[DD][d][Y];
          lid[Dd][k][Z]= o[Dd][d][Z] + tbulge[k]*n[Dd][d][Z] + wbulge[k]*n[DD][d][Z];
        }

     /* Calculate approximate normal directions for the lid. ---- FIXED!*/
     if (stor==0)  /* downwards normals */
          for (i=1; i<=Dd; i++)	/* for all points including rim */
            {
              nlid[i][X] =  o[i][d][X] - o[i][d+1][X];
              nlid[i][Y] =  o[i][d][Y] - o[i][d+1][Y];
              nlid[i][Z] =  o[i][d][Z] - o[i][d+1][Z];
              normalize(nlid[i]);
            }
      else /* upwards normals */
          for (i=1; i<=Dd; i++)	/* for all points including rim */
            {
              nlid[i][X] =  o[i][d][X] - o[i][d-1][X];
              nlid[i][Y] =  o[i][d][Y] - o[i][d-1][Y];
              nlid[i][Z] =  o[i][d][Z] - o[i][d-1][Z];
              normalize(nlid[i]);
            }
    } /* if lid_points are needed because we are in an end stage */


    if (stor==scherk_storeys)
     {
      loadmaterial( 1, 2);
      /* make an UPWARD FACING LID at mid-storey height */
      for (k=1; k<=kmax; k++ )  /* make longitudinal t-strips */
        {
          bgntmesh();
          for (i=1; i<=Dd; i++)	
            {
              if (gstate.envmap)
                  EnvMapUV(nlid[i], cm);
              else
                  t2f(tlid[i][k]);
	      n3f(nlid[i]);
	      v3f(lid[i][k]);
    
              if (gstate.envmap)
                  EnvMapUV(nlid[i], cm);
              else
                  t2f(tlid[i][k-1]);
	      n3f(nlid[i]);
	      v3f(lid[i][k-1]);
            }
          endtmesh();
        }

      if(ntext==0)
          for (k=0; k<=kmax; k++)
            for (i=1; i<=Dd; i++) 
	      drawnormal(lid[i][k], nlid[i] );

      if(out_data)
        {
          calc_bbox();
          output_lid(1,0,DD-1,kmax,0,5);
        }
     }

    if (stor==0)
     {
      loadmaterial( 1, 3);
      /* make a DOWNWARD FACING LID at mid-storey height */
      for (k=1; k<=kmax; k++ )  /* make longitudinal t-strips */
        {
          bgntmesh();
          for (i=1; i<=Dd; i++)
            {
              if (gstate.envmap)
                  EnvMapUV(nlid[i], cm);
              else
                  t2f(tlid[i][k-1]);
	      n3f(nlid[i]);
	      v3f(lid[i][k-1]);
    
              if (gstate.envmap)
                  EnvMapUV(nlid[i], cm);
              else
                  t2f(tlid[i][k]);
	      n3f(nlid[i]);
	      v3f(lid[i][k]);
            }
          endtmesh();
        }

      if(ntext==0)
          for (k=0; k<=kmax; k++)
            for (i=1; i<=Dd; i++) 
	      drawnormal(lid[i][k], nlid[i] );


      if(out_data)   /* LID OUTPUT */
        {
          calc_bbox();
          output_lid(1,0,DD-1,kmax,1,6);
        }
     }

   } /* end-faces if not ring */
   


/*=================== BEGIN OF INNER SURFACE ====================*/

  /*--------------------------------------------------------
    Calculate the necessary changes for the INNER surface
    Flip normals and adjust offset in opposite direction.
  ---------------------------------------------------------*/

  for (j=0; j<=dd; j++)
    { 
      for (i=1; i<DD; i++)	/* all but rim normals */
        {			/* send consistent vertex information to tmesh */
          n[i][j][X] = -n[i][j][X];
          n[i][j][Y] = -n[i][j][Y];
          n[i][j][Z] = -n[i][j][Z];
        }

      for (i=1; i<DD; i++)	/* keep rim vertices in place */
        {    /* on second pass: inside mesh: 
		move point AGAINST OLD normal by TWICE the amount. */
          o[i][j][X] += 2.0* thick * n[i][j][X];
          o[i][j][Y] += 2.0* thick * n[i][j][Y];
          o[i][j][Z] += 2.0* thick * n[i][j][Z];
        }
    }


  /*--------------------------------------------------------
   DO INNER TMESH: do corresponding thing as above,
   but start from opposite end to get opposite orientation.
  ---------------------------------------------------------*/
  if (thick > 0.0)
    {
      /* (re)bind FACE material and texture */
      numbr = (oddstor ? 1 : 2);
      loadmaterial( 0, numbr);
    }

  /* Genereate T-MESH for inner surface */

  if(do_lower)
   {
    for (j=0; j<d; j++ )  /* bottom half */
     {       /* do this in 4 quadrants, to get nicer junction at saddle point:
	                     have 8 triangle come together to deal with warp. */

      bgntmesh();
      for (i=1; i<=D; i++)  /* left half plus one triangle */
	{
	  if (gstate.envmap)
            {
              EnvMapUV(n[i][j], cm);
            }
          else
            {
              t2f(mt[i][j]);
            }
	  n3f(n[i][j]);
	  v3f(o[i][j]);
	  if (gstate.envmap)
            {
              EnvMapUV(n[i][j+1], cm);
            }
          else
            {
              t2f(mt[i][j+1]);
            }
	  n3f(n[i][j+1]);
	  v3f(o[i][j+1]);
	}

       if (gstate.envmap)
        {
          EnvMapUV(n[i][j+1], cm);
        }
       else
        {
          t2f(mt[i][j+1]);
        }
      n3f(n[i][j+1]); 
      v3f(o[i][j+1]);
      endtmesh();

      bgntmesh();
      for (i=D; i<=dd; i++)  /* right half minus one triangle */
	{
	            if (gstate.envmap)
            {
              EnvMapUV(n[i][j], cm);
            }
          else
            {
              t2f(mt[i][j]);
            }
	  n3f(n[i][j]);
	  v3f(o[i][j]);
	            if (gstate.envmap)
            {
              EnvMapUV(n[i+1][j+1], cm);
            }
          else
            {
              t2f(mt[i+1][j+1]);
            }
	  n3f(n[i+1][j+1]);
	  v3f(o[i+1][j+1]);
	}
             if (gstate.envmap)
        {
          EnvMapUV(n[i][j], cm);
        }
       else
        {
          t2f(mt[i][j]);
        }
      n3f(n[i][j]);
      v3f(o[i][j]);
      endtmesh();

    }
    if (out_data)
        {
          calc_bbox();
          output_surface(1, 0, 3+10*branch+100*stor);
        }
  }

  if(do_upper)
   {
    for (j=dd; j>d; j-- )
     { /* upper half,  from top down */
      bgntmesh();
      for (i=Dd; i>=D; i--)  /* right half plus one triangle */
	{
	  if (gstate.envmap)
            {
              EnvMapUV(n[i][j], cm);
            }
          else
            {
              t2f(mt[i][j]);
            }
	  n3f(n[i][j]);
	  v3f(o[i][j]);
	            if (gstate.envmap)
            {
              EnvMapUV(n[i][j-1], cm);
            }
          else
            {
              t2f(mt[i][j-1]);
            }
	  n3f(n[i][j-1]);
	  v3f(o[i][j-1]);
	}
             if (gstate.envmap)
        {
          EnvMapUV(n[i][j-1], cm);
        }
       else
        {
          t2f(mt[i][j-1]);
        }
      n3f(n[i][j-1]);
      v3f(o[i][j-1]);
      endtmesh();

      bgntmesh();
      for (i=D; i>=2; i--)  /* left half minus one triangle */
	{
	  if (gstate.envmap)
            {
              EnvMapUV(n[i][j], cm);
            }
          else
            {
              t2f(mt[i][j]);
            }
	  n3f(n[i][j]);
	  v3f(o[i][j]);
	            if (gstate.envmap)
            {
              EnvMapUV(n[i-1][j-1], cm);
            }
          else
            {
              t2f(mt[i-1][j-1]);
            }
	  n3f(n[i-1][j-1]);
	  v3f(o[i-1][j-1]);
	}
      if (gstate.envmap)
        {
          EnvMapUV(n[i][j], cm);
        }
       else
        {
          t2f(mt[i][j]);
        }
      n3f(n[i][j]);
      v3f(o[i][j]);
      endtmesh();
    }

    if (out_data)
        {
          calc_bbox();
          output_surface(1, 1, 4+10*branch+100*stor);
        }
  }
  
  b_count++;

 /* Approximation of the number of polys in one patch */
 if (do_upper && do_lower)
     return (4*dd*dd + 4*dd*kmax);
 else if ((scherk_warp == 360)||(scherk_warp == 720)||(scherk_warp == 1080))
     return (2*dd*dd + 2*dd*kmax);
 else
     return (2*dd*dd + 4*dd*kmax);

} /*end: surfbranch */


/*=====================================================================
BUILD DISPLAY LIST
FULL processing of the geometry should only be done when cursor
is in the slider menu widow.
When cursor leaves, the geometry should be written to a display list.
When the cursor is in the display window, only the transormations
should be changed and the SAME display list can be reused
until the cursor returns into the slider window.
------------------------------------------------------------------------*/
         

/*========================================================================
  FULL SCHERK/COLLINS SCULPTURE
  __________________________________________________________
  This is the main loop calling upon the needed instances
  of branches to make the sculpture with all the storeys.
  ---------------------------------------------------------*/
int geometry(int flat)
{
  int stor, oddstor, branch, polys;
  float ringrad, archrad, thetab, thwarp, zbase, zoffset, azims, azimb, twirl, L_flange;

  d=scherk_detail;
  D=d+1;
  dd=d+d;
  Dd=D+d;
  DD=D+D;
  ring = (scherk_warp == 0.0 ? 0 : 1);
  vanes = 2*scherk_branches;
  L_flange = (scherk_flange<0.7 ? 0.7 : scherk_flange);
  thwarp= scherk_warp/(float)(scherk_storeys);      /* amount of bend per storey */
  twirl = scherk_twist/(float)(scherk_storeys);     /* amount of twist per storey */
  ringrad = (float)(scherk_storeys)*scherk_height/M_PI;     /* median radius of closed toroidal ring */
  archrad = 360.0*(float)(scherk_storeys)*scherk_height/scherk_warp/M_PI;     /* median radius of open arch segment */
  polys = 0;     /* Reset counter */


  /*>>> BUILD TEMPLATE ARRAY --------------------------------
    Load up the template array, and calculate those values
    that are worth saving from one patch instance to the next.
    For the Scherk, these are the coordinates, texture coordinates,
    ---------------------------------------------------------*/
basearray(scherk_branches, scherk_tex_tiles, scherk_height,
          L_flange, twirl, scherk_thickness, scherk_rim_bulge);

  /*--------------------------------------------------------
    With the template in place, make needed versioned copies:
    This outer-most loop produces the necessary number of storeys.
    We COPY the above Scherk-patch template array and update 
    the vertex and normal information to the degree needed 
    to make all the required instances,
    either offset in the z-direction for the Scherk tower,
    or offset in theta around the toroidal ring for Collins.
    >>> LOOP storeys TIMES; set base_Z, theta, and Azimuth.
    ---------------------------------------------------------*/
  for (stor=0; stor<=(scherk_storeys); stor++)               
    { /* calculate starting azimuth for this storey */
      oddstor=(odd(stor));
      azims = twirl*((float)(stor)-0.5) + scherk_azimuth;
      if(odd(stor))
	  azims += 180.0/(float)(scherk_branches);
      thetab= scherk_warp*((float)(stor)-0.5)/(float)(scherk_storeys);

      /* adjust so that foot rests at constant height: shift down 0.5 storeys */
      /*zbase = 2.0*scherk_height*(float)(stor) - scherk_height*(float)(scherk_storeys);*/
      zbase = 2.0*scherk_height*((float)(stor)-0.5) - scherk_height*(float)(scherk_storeys);
      if(scherk_warp<=360.0)
        zoffset = - (360.0 - scherk_warp)*scherk_height*(float)(scherk_storeys)/360.0;
      else
        zoffset = 0.0;   

    /* This loop computes the surface branches in the same storey.
      For the original Scherk/Collins surfaces we just make
      one branch copy: increase the azimuth by 180 degrees.
      This gives the rotation by 180 degrees around the z-axis.
      Loop branches times  {for Scherk: twice}.  */
      
      for (branch=0; branch<scherk_branches; branch++)        
	{
	  azimb = azims + 360.0*(float)(branch)/(float)(scherk_branches);
	  
	/*>>> GENERTATE ONE BRANCH OF ONE STOREY ------------------*/
          polys += surfbranch(flat, ring, stor,
		              scherk_tex_tiles,  scherk_height,  L_flange, scherk_thickness,
                              branch,  azimb,   twirl,  scherk_rim_bulge,
                              zbase, zoffset,  ringrad, archrad, thetab, thwarp);
	}
    }

  out_data = 0;
  total_trias = polys;
  return polys;
} /* end: geometry ------------------------------------------------*/


/* display list function for the scherk surfaces */
int geometry_display_list()
{
  return geometry(gstate.shading);
}

/*==================================================================*/