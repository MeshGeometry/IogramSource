//
// Copyright (c) 2016 - 2017 Mesh Consultants Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#include "Geomlib_GeoConversions.h"

#define GD_semiMajorAxis 6378137.000000
#define GD_TranMercB     6356752.314245
#define GD_geocentF      0.003352810664

using Urho3D::Vector3;

namespace Geomlib {

	Urho3D::Vector3 GeoToXYZ(
		double refLat,
		double refLon,
		double lat,
		double lon
		)
	{
		double a = GD_semiMajorAxis;
		double b = GD_TranMercB;
		double f = GD_geocentF;

		double L = lon - refLon;
		double U1 = atan((1 - f) * tan(refLat));
		double U2 = atan((1 - f) * tan(lat));
		double sinU1 = sin(U1);
		double cosU1 = cos(U1);
		double sinU2 = sin(U2);
		double cosU2 = cos(U2);

		double lambda = L;
		double lambdaP;
		double sinSigma;
		double sigma;
		double cosSigma;
		double cosSqAlpha;
		double cos2SigmaM;
		double sinLambda;
		double cosLambda;
		double sinAlpha;

		double xOffset, yOffset;

		int iterLimit = 100;
		do {
			sinLambda = sin(lambda);
			cosLambda = cos(lambda);
			sinSigma = sqrt((cosU2*sinLambda) * (cosU2*sinLambda) +
				(cosU1*sinU2 - sinU1*cosU2*cosLambda) *
				(cosU1*sinU2 - sinU1*cosU2*cosLambda));
			if (sinSigma == 0)
			{
				xOffset = 0.0;
				yOffset = 0.0;
				return Vector3::ZERO;  // co-incident points
			}
			cosSigma = sinU1*sinU2 + cosU1*cosU2*cosLambda;
			sigma = atan2(sinSigma, cosSigma);
			sinAlpha = cosU1 * cosU2 * sinLambda / sinSigma;
			cosSqAlpha = 1 - sinAlpha*sinAlpha;
			cos2SigmaM = cosSigma - 2 * sinU1*sinU2 / cosSqAlpha;
			if (cos2SigmaM != cos2SigmaM) //isNaN
			{
				cos2SigmaM = 0;  // equatorial line: cosSqAlpha=0 (§6)
			}
			double C = f / 16 * cosSqAlpha*(4 + f*(4 - 3 * cosSqAlpha));
			lambdaP = lambda;
			lambda = L + (1 - C) * f * sinAlpha *
				(sigma + C*sinSigma*(cos2SigmaM + C*cosSigma*(-1 + 2 * cos2SigmaM*cos2SigmaM)));
		} while (fabs(lambda - lambdaP) > 1e-12 && --iterLimit>0);

		if (iterLimit == 0)
		{
			xOffset = 0.0;
			yOffset = 0.0;
			return Vector3::ZERO;  // formula failed to converge
		}

		double uSq = cosSqAlpha * (a*a - b*b) / (b*b);
		double A = 1 + uSq / 16384 * (4096 + uSq*(-768 + uSq*(320 - 175 * uSq)));
		double B = uSq / 1024 * (256 + uSq*(-128 + uSq*(74 - 47 * uSq)));
		double deltaSigma = B*sinSigma*(cos2SigmaM + B / 4 * (cosSigma*(-1 + 2 * cos2SigmaM*cos2SigmaM) -
			B / 6 * cos2SigmaM*(-3 + 4 * sinSigma*sinSigma)*(-3 + 4 * cos2SigmaM*cos2SigmaM)));
		double s = b*A*(sigma - deltaSigma);

		double bearing = atan2(cosU2*sinLambda, cosU1*sinU2 - sinU1*cosU2*cosLambda);
		xOffset = sin(bearing)*s;
		yOffset = cos(bearing)*s;


		return Vector3(xOffset,0.0f, yOffset);
	}
}