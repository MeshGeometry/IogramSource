#include "Geomlib_SunPosition.h"

// C++ program calculating the solar position for
// the current date and a set location (latitude, longitude)
// Jarmo Lammi 1999
//
// Code refreshed to work in the newer versions of C++
// Compiled and tested with
// Apple LLVM version 6.0 (clang-600.0.56) (based on LLVM 3.5svn)
// Target: x86_64-apple-darwin14.0.0
// Jarmo Lammi 4-Jan-2015

#include <iostream>
#include <math.h>
#include <time.h>

using std::cout;
using std::cin;
using std::ios;
using std::endl;


namespace
{
	// extern double pi;
	double pi = 3.14159265359;

	double tpi = 2 * pi;
	double degs = 180.0 / pi;
	double rads = pi / 180.0;

	double L, RA, g, daylen, delta, x, y, z;
	double SunDia = 0.53;     // Sunradius degrees

	double AirRefr = 34.0 / 60.0; // athmospheric refraction degrees //

								  //   Get the days to J2000
								  //   h is UT in decimal hours
								  //   FNday only works between 1901 to 2099 - see Meeus chapter 7

	double FNday(int y, int m, int d, float h) {
		int luku = -7 * (y + (m + 9) / 12) / 4 + 275 * m / 9 + d;
		// type casting necessary on PC DOS and TClite to avoid overflow
		luku += (long int)y * 367;
		return (double)luku - 730530.0 + h / 24.0;
	};

	//   the function below returns an angle in the range
	//   0 to 2*pi

	double FNrange(double x) {
		double b = x / tpi;
		double a = tpi * (b - (long)(b));
		if (a < 0) a = tpi + a;
		return a;
	};

	// Calculating the hourangle
	//
	double f0(double lat, double declin) {
		double fo, dfo;
		dfo = rads*(0.5*SunDia + AirRefr);
		if (lat < 0.0) dfo = -dfo;	// Southern hemisphere
		fo = tan(declin + dfo) * tan(lat*rads);
		if (fo > 0.99999) fo = 1.0; // to avoid overflow //
		fo = asin(fo) + pi / 2.0;
		return fo;
	};

	//   Find the ecliptic longitude of the Sun

	double FNsun(double d) {
		double w, M, v, r;
		//   mean longitude of the Sun
		w = 282.9404 + 4.70935E-5 * d;
		M = 356.047 + 0.9856002585 * d;
		// Sun's mean longitude
		L = FNrange(w * rads + M * rads);

		//   mean anomaly of the Sun

		g = FNrange(M * rads);

		// eccentricity
		double ecc = 0.016709 - 1.151E-9 * d;

		//   Obliquity of the ecliptic

		double obliq = 23.4393 * rads - 3.563E-7 * rads * d;
		double E = M + degs * ecc * sin(g) * (1.0 + ecc * cos(g));
		E = degs*FNrange(E*rads);
		x = cos(E*rads) - ecc;
		y = sin(E*rads) * sqrt(1.0 - ecc*ecc);
		r = sqrt(x*x + y*y);
		v = atan2(y, x)*degs;
		// longitude of sun
		double lonsun = v + w;
		lonsun -= 360.0*(lonsun > 360.0);

		// sun's ecliptic rectangular coordinates
		x = r * cos(lonsun*rads);
		y = r * sin(lonsun*rads);
		double yequat = y * cos(obliq);
		double zequat = y * sin(obliq);
		RA = atan2(yequat, x);
		delta = atan2(zequat, sqrt(x*x + yequat*yequat));
		RA *= degs;

		//   Ecliptic longitude of the Sun

		return FNrange(L + 1.915 * rads * sin(g) + .02 * rads * sin(2 * g));
	};

	// Display decimal hours in hours and minutes
	void showhrmn(double dhr) {
		int hr, mn;
		hr = (int)dhr;
		mn = (dhr - (double)hr) * 60;
		if (hr < 10) cout << '0';
		cout << hr << ':';
		if (mn < 10) cout << '0';
		cout << mn;
	};
};

Urho3D::Vector3 Geomlib::ComputeSunPosition(
	float year,
	float month,
	float day,
	float hour,
	int tzone,
	float latit,
	float longit
)
{
	// testi
	// year = 1990; m=4; day=19; h=11.99;	// local time
	double UT = hour - tzone;	// universal time
	double jd = FNday(year, month, day, UT);

	//   Use FNsun to find the ecliptic longitude of the
	//   Sun
	double lambda = FNsun(jd);
	//   Obliquity of the ecliptic

	double obliq = 23.4393 * rads - 3.563E-7 * rads * jd;

	// Sidereal time at Greenwich meridian
	double GMST0 = L*degs / 15.0 + 12.0;	// hours
	double SIDTIME = GMST0 + UT + longit / 15.0;
	// Hour Angle
	double ha = 15.0*SIDTIME - RA;	// degrees
	ha = FNrange(rads*ha);
	x = cos(ha) * cos(delta);
	y = sin(ha) * cos(delta);
	z = sin(delta);
	double xhor = x * sin(latit*rads) - z * cos(latit*rads);
	double yhor = y;
	double zhor = x * cos(latit*rads) + z * sin(latit*rads);
	double	azim = atan2(yhor, xhor) + pi;
	azim = FNrange(azim);
	double altit = asin(zhor) * degs;

	// cout << "L = " <<  L* degs << " degr\n";
	// cout << "RA = " << RA << " degr\n";
	cout << "GMST0 = " << GMST0 << " hours\n";
	cout << "SIDTIME = " << SIDTIME << " hours\n";
	// cout << "Hour Angle = " << ha*degs << " degr\n";

	// delta = asin(sin(obliq) * sin(lambda));
	double alpha = atan2(cos(obliq) * sin(lambda), cos(lambda));
	//   Find the Equation of Time in minutes
	double equation = 1440 - (L - alpha) * degs * 4;

	ha = f0(latit, delta);

	// Conversion of angle to hours and minutes //
	daylen = degs*ha / 7.5;
	if (daylen<0.0001) { daylen = 0.0; }
	// arctic winter     //

	double riset = 12.0 - 12.0 * ha / pi + tzone - longit / 15.0 + equation / 60.0;
	double settm = 12.0 + 12.0 * ha / pi + tzone - longit / 15.0 + equation / 60.0;
	double noont = riset + 12.0 * ha / pi;
	double altmax = 90.0 + delta*degs - latit;
	if (altmax > 90.0) altmax = 180.0 - altmax; //to express as degrees from the N horizon

	noont -= 24 * (noont>24);

	if (riset > 24.0) riset -= 24.0;
	if (settm > 24.0) settm -= 24.0;

	//cout << "\n Sunrise and set times\n";
	//cout << "===============\n";

	//cout.setf(ios::fixed);
	//cout.precision(0);
	//cout << "  year  : " << year << endl;
	//cout << "  month : " << month << endl;
	//cout << "  day   : " << day << "\n\n";
	//cout << "  time  : " << hour << endl;
	//cout << "Days since Y2K :  " << jd << '\n';
	//cout.precision(2);
	//cout << "Latitude :  " << latit << ", longitude:  " << longit << endl;
	//cout << "Timezone :  " << tzone << "\n\n";
	//cout << "Declination : " << delta * degs << '\n';
	//cout << "Daylength     : "; showhrmn(daylen); cout << " hours \n";
	//cout << "Sunrise       : ";
	//showhrmn(riset); cout << '\n';
	//cout << "Sun altitude at noontime ";

	//showhrmn(noont); cout << " = " << altmax << " degrees " << (latit >= delta * degs ? "(S)" : "(N)") << endl;
	//cout << "Sunset        : ";
	//showhrmn(settm); cout << '\n';
	//cout << endl << "SUNPOSITION AT THIS MOMENT" << endl;
	//cout << "Azimuth= " << azim*degs << " degr \n";
	//cout << "Altitude= " << altit << " degr \n";


	//return as vector3
	Urho3D::Vector3 sunPositionOut(altit, azim*degs, 0);

	return sunPositionOut;
}