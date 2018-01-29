// Voro++, a 3D cell-based Voronoi library
//
// Author   : Chris H. Rycroft (LBL / UC Berkeley)
// Email    : chr@alum.mit.edu
// Date     : August 30th 2011

/** \file wall.cc
 * \brief Function implementations for the derived wall classes. */

#include "wall.hh"

namespace voro {

/** Tests to see whether a point is inside the sphere wall object.
 * \param[in,out] (x,y,z) the vector to test.
 * \return True if the point is inside, false if the point is outside. */
bool wall_sphere::point_inside(double x,double y,double z) {
	return (x-xc)*(x-xc)+(y-yc)*(y-yc)+(z-zc)*(z-zc)<rc*rc;
}

/** Tests to see whether a point is inside the plane wall object.
 * \param[in] (x,y,z) the vector to test.
 * \return True if the point is inside, false if the point is outside. */
bool wall_plane::point_inside(double x,double y,double z) {
	return x*xc+y*yc+z*zc<ac;
}

/** Tests to see whether a point is inside the cylindrical wall object.
 * \param[in] (x,y,z) the vector to test.
 * \return True if the point is inside, false if the point is outside. */
bool wall_cylinder::point_inside(double x,double y,double z) {
	double xd=x-xc,yd=y-yc,zd=z-zc;
	double pa=(xd*xa+yd*ya+zd*za)*asi;
	xd-=xa*pa;yd-=ya*pa;zd-=za*pa;
	return xd*xd+yd*yd+zd*zd<rc*rc;
}

/** Tests to see whether a point is inside the cone wall object.
 * \param[in] (x,y,z) the vector to test.
 * \return True if the point is inside, false if the point is outside. */
bool wall_cone::point_inside(double x,double y,double z) {
	double xd=x-xc,yd=y-yc,zd=z-zc,pa=(xd*xa+yd*ya+zd*za)*asi;
	xd-=xa*pa;yd-=ya*pa;zd-=za*pa;
	pa*=gra;
	if (pa<0) return false;
	pa*=pa;
	return xd*xd+yd*yd+zd*zd<pa;
}

}
