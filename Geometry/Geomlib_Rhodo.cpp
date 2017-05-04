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


#include "Geomlib_Rhodo.h"

#include <Eigen/Core>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Vector3.h>

Geomlib::Rhodo::Rhodo(Rhodo& originalRhodo, int jumpIndex)
{
	Eigen::RowVector3i unitlessCoordinates = originalRhodo.ComputeUnitlessCoordinates()
		+ RhodoEigen::jumpsToNeighbors[jumpIndex];
	family_ = ComputeFamilyFromUnitlessCoordinates(unitlessCoordinates);
	xDiamJumps_ = (unitlessCoordinates(0) - RhodoEigen::iGeneratingRhodos[family_](0)) / 4;
	yDiamJumps_ = (unitlessCoordinates(1) - RhodoEigen::iGeneratingRhodos[family_](1)) / 4;
	zDiamJumps_ = (unitlessCoordinates(2) - RhodoEigen::iGeneratingRhodos[family_](2)) / 4;
}

Geomlib::Rhodo::Rhodo(int xUnitless, int yUnitless, int zUnitless)
{
	Eigen::RowVector3i unitlessCoords(xUnitless, yUnitless, zUnitless);
	family_ = ComputeFamilyFromUnitlessCoordinates(unitlessCoords);
	Eigen::RowVector3i generatingCenter = RhodoEigen::iGeneratingRhodos[family_];
	xDiamJumps_ = (unitlessCoords[0] - generatingCenter[0]) / 4;
	yDiamJumps_ = (unitlessCoords[1] - generatingCenter[1]) / 4;
	zDiamJumps_ = (unitlessCoords[2] - generatingCenter[2]) / 4;
}


Eigen::RowVector3f Geomlib::Rhodo::ComputeCoordinates(float s) const
{
	return s * (RhodoEigen::fGeneratingRhodos[family_] +
		(4 * (float)xDiamJumps_) * Eigen::RowVector3f(1.0f, 0.0f, 0.0f) +
		(4 * (float)yDiamJumps_) * Eigen::RowVector3f(0.0f, 1.0f, 0.0f) +
		(4 * (float)zDiamJumps_) * Eigen::RowVector3f(0.0f, 0.0f, 1.0f));
}

Eigen::RowVector3i Geomlib::Rhodo::ComputeUnitlessCoordinates() const
{
	return RhodoEigen::iGeneratingRhodos[family_] +
		(4 * xDiamJumps_) * Eigen::RowVector3i(1, 0, 0) +
		(4 * yDiamJumps_) * Eigen::RowVector3i(0, 1, 0) +
		(4 * zDiamJumps_) * Eigen::RowVector3i(0, 0, 1);
}

Urho3D::VariantVector Geomlib::Rhodo::ComputeVertices(float scale) const
{
	Urho3D::VariantVector tmp;

	Eigen::RowVector3i uCenter = ComputeUnitlessCoordinates();

	for (unsigned i = 0; i < 14; ++i) {
		Eigen::RowVector3i vert = uCenter + RhodoEigen::rhodoVertexCoords[i];
		Urho3D::Vector3 baseCoords(scale * vert(0), scale * vert(1), scale * vert(2));
		tmp.Push(baseCoords);
	}

	return tmp;
}

Urho3D::VariantVector Geomlib::Rhodo::ComputeRhombicPolylines(float scale) const
{
	Urho3D::VariantVector tmp;

	Eigen::RowVector3i uCenter = ComputeUnitlessCoordinates();
	Eigen::RowVector3f fCenter = scale * Eigen::RowVector3f((float)uCenter(0), (float)uCenter(1), (float)uCenter(2));

	// Loop across the 12 rhombic faces of the rhombic dodecahedron
	for (unsigned i = 0; i < 12; ++i) {
		// Get rhombic face i
		const Eigen::RowVector3f* rhombus = RhodoEigen::neighborRhombi[i];
		Urho3D::VariantMap rhombusPolyLine;
		Urho3D::VariantVector verts;
		// Loop over the 4 vertices of this rhombus
		for (unsigned j = 0; j < 4; ++j) {
			Eigen::RowVector3f v = scale * rhombus[j] + fCenter;

			Urho3D::Vector3 vert(v(0), v(1), v(2));
			verts.Push(vert);
		}
		// Close the rhombus polyline by adding the initial vertex again at the end
		Eigen::RowVector3f v = scale * rhombus[0] + fCenter;
		Urho3D::Vector3 vert(v(0), v(1), v(2));
		verts.Push(vert);

		rhombusPolyLine["vertices"] = verts;
		tmp.Push(rhombusPolyLine);
	}

	return tmp;
}

// hackish, depends on definition of generatingCenterDisplacements
int Geomlib::ComputeFamilyFromUnitlessCoordinates(const Eigen::RowVector3i& unitlessCoordinates)
{
	int a = unitlessCoordinates[0] % 4;
	int b = unitlessCoordinates[1] % 4;
	if (a == 0) {
		if (b == 0) {
			return 0;
		}
		else {
			return 1;
		}
	}
	else {
		if (b == 0) {
			return 2;
		}
		else {
			return 3;
		}
	}
}