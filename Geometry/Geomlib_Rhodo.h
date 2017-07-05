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


#pragma once

#include <vector>

#include <Eigen/Core>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Vector3.h>

namespace Geomlib {

	// Constants such as vertex coordinates, rhombus data, etc.,
	// for basic generating origin-centered rhombic dodecahedron,
	// expressed as Eigen data structures.
	namespace RhodoEigen {

		static Eigen::RowVector3i iGeneratingRhodos[4] = {
			Eigen::RowVector3i(0, 0, 0),
			Eigen::RowVector3i(0, 2, 2),
			Eigen::RowVector3i(2, 0, 2),
			Eigen::RowVector3i(2, 2, 0)
		};

		static Eigen::RowVector3f fGeneratingRhodos[4] = {
			Eigen::RowVector3f(0.0f, 0.0f, 0.0f),
			Eigen::RowVector3f(0.0f, 2.0f, 2.0f),
			Eigen::RowVector3f(2.0f, 0.0f, 2.0f),
			Eigen::RowVector3f(2.0f, 2.0f, 0.0f)
		};

		static const Eigen::RowVector3i jumpsToNeighbors[12] = {
			Eigen::RowVector3i(0, 2, 2),
			Eigen::RowVector3i(0, -2, 2),
			Eigen::RowVector3i(2, 0, 2),
			Eigen::RowVector3i(-2, 0, 2),
			Eigen::RowVector3i(0, 2, -2),
			Eigen::RowVector3i(0, -2, -2),
			Eigen::RowVector3i(2, 0, -2),
			Eigen::RowVector3i(-2, 0, -2),
			Eigen::RowVector3i(2, 2, 0),
			Eigen::RowVector3i(-2, 2, 0),
			Eigen::RowVector3i(-2, -2, 0),
			Eigen::RowVector3i(2, -2, 0)
		};

		static const Eigen::RowVector3i rhodoVertexCoords[14] = {
			Eigen::RowVector3i(1, 1, 1),
			Eigen::RowVector3i(1, 1, -1),
			Eigen::RowVector3i(1, -1, 1),
			Eigen::RowVector3i(1, -1, -1),
			Eigen::RowVector3i(-1, 1, 1),
			Eigen::RowVector3i(-1, 1, -1),
			Eigen::RowVector3i(-1, -1, 1),
			Eigen::RowVector3i(-1, -1, -1),
			Eigen::RowVector3i(2, 0, 0),
			Eigen::RowVector3i(-2, 0, 0),
			Eigen::RowVector3i(0, 2, 0),
			Eigen::RowVector3i(0, -2, 0),
			Eigen::RowVector3i(0, 0, 2),
			Eigen::RowVector3i(0, 0, -2)
		};

		static const Eigen::RowVector3f UP_NORTH_RHOMBUS[4] = {
			Eigen::RowVector3f(0.0, 0.0, 2.0),
			Eigen::RowVector3f(-1.0, 1.0, 1.0),
			Eigen::RowVector3f(0.0, 2.0, 0.0),
			Eigen::RowVector3f(1.0, 1.0, 1.0)
		};

		static const Eigen::RowVector3f UP_SOUTH_RHOMBUS[4] = {
			Eigen::RowVector3f(0.0, 0.0, 2.0),
			Eigen::RowVector3f(1.0, -1.0, 1.0),
			Eigen::RowVector3f(0.0, -2.0, 0.0),
			Eigen::RowVector3f(-1.0, -1.0, 1.0)
		};

		static const Eigen::RowVector3f UP_EAST_RHOMBUS[4] = {
			Eigen::RowVector3f(0.0, 0.0, 2.0),
			Eigen::RowVector3f(1.0, 1.0, 1.0),
			Eigen::RowVector3f(2.0, 0.0, 0.0),
			Eigen::RowVector3f(1.0, -1.0, 1.0)
		};

		static const Eigen::RowVector3f UP_WEST_RHOMBUS[4] = {
			Eigen::RowVector3f(0.0, 0.0, 2.0),
			Eigen::RowVector3f(-1.0, -1.0, 1.0),
			Eigen::RowVector3f(-2.0, 0.0, 0.0),
			Eigen::RowVector3f(-1.0, 1.0, 1.0)
		};

		static const Eigen::RowVector3f DOWN_NORTH_RHOMBUS[4] = {
			Eigen::RowVector3f(0.0, 0.0, -2.0),
			Eigen::RowVector3f(1.0, 1.0, -1.0),
			Eigen::RowVector3f(0.0, 2.0, 0.0),
			Eigen::RowVector3f(-1.0, 1.0, -1.0)
		};

		static const Eigen::RowVector3f DOWN_SOUTH_RHOMBUS[4] = {
			Eigen::RowVector3f(0.0, 0.0, -2.0),
			Eigen::RowVector3f(-1.0, -1.0, -1.0),
			Eigen::RowVector3f(0.0, -2.0, 0.0),
			Eigen::RowVector3f(1.0, -1.0, -1.0)
		};

		static const Eigen::RowVector3f DOWN_EAST_RHOMBUS[4] = {
			Eigen::RowVector3f(0.0, 0.0, -2.0),
			Eigen::RowVector3f(1.0, -1.0, -1.0),
			Eigen::RowVector3f(2.0, 0.0, 0.0),
			Eigen::RowVector3f(1.0, 1.0, -1.0)
		};

		static const Eigen::RowVector3f DOWN_WEST_RHOMBUS[4] = {
			Eigen::RowVector3f(0.0, 0.0, -2.0),
			Eigen::RowVector3f(-1.0, 1.0, -1.0),
			Eigen::RowVector3f(-2.0, 0.0, 0.0),
			Eigen::RowVector3f(-1.0, -1.0, -1.0)
		};

		static const Eigen::RowVector3f NORTH_EAST_RHOMBUS[4] = {
			Eigen::RowVector3f(0.0, 2.0, 0.0),
			Eigen::RowVector3f(1.0, 1.0, -1.0),
			Eigen::RowVector3f(2.0, 0.0, 0.0),
			Eigen::RowVector3f(1.0, 1.0, 1.0)
		};

		static const Eigen::RowVector3f NORTH_WEST_RHOMBUS[4] = {
			Eigen::RowVector3f(-2.0, 0.0, 0.0),
			Eigen::RowVector3f(-1.0, 1.0, -1.0),
			Eigen::RowVector3f(0.0, 2.0, 0.0),
			Eigen::RowVector3f(-1.0, 1.0, 1.0)
		};

		static const Eigen::RowVector3f SOUTH_WEST_RHOMBUS[4] = {
			Eigen::RowVector3f(0.0, -2.0, 0.0),
			Eigen::RowVector3f(-1.0, -1.0, -1.0),
			Eigen::RowVector3f(-2.0, 0.0, 0.0),
			Eigen::RowVector3f(-1.0, -1.0, 1.0)
		};

		static const Eigen::RowVector3f SOUTH_EAST_RHOMBUS[4] = {
			Eigen::RowVector3f(2.0, 0.0, 0.0),
			Eigen::RowVector3f(1.0, -1.0, -1.0),
			Eigen::RowVector3f(0.0, -2.0, 0.0),
			Eigen::RowVector3f(1.0, -1.0, 1.0)
		};

		static const Eigen::RowVector3f* neighborRhombi[12] = {
			UP_NORTH_RHOMBUS,
			UP_SOUTH_RHOMBUS,
			UP_EAST_RHOMBUS,
			UP_WEST_RHOMBUS,
			DOWN_NORTH_RHOMBUS,
			DOWN_SOUTH_RHOMBUS,
			DOWN_EAST_RHOMBUS,
			DOWN_WEST_RHOMBUS,
			NORTH_EAST_RHOMBUS,
			NORTH_WEST_RHOMBUS,
			SOUTH_WEST_RHOMBUS,
			SOUTH_EAST_RHOMBUS
		};
	}

	// Constants such as vertex coordinates, rhombus data, etc.,
	// for basic generating origin-centered rhombic dodecahedron,
	// expressed as native Urho3D data structures.
	namespace RhodoUrho3D {

		/*
		Having second thoughts about making the transition here from
		Eigen to Urho3D, as I like the i/f int/float enforced distinction....
		*/

		static Urho3D::Vector3 iGeneratingRhodos[4] = {
			Urho3D::Vector3(0, 0, 0),
			Urho3D::Vector3(0, 2, 2),
			Urho3D::Vector3(2, 0, 2),
			Urho3D::Vector3(2, 2, 0)
		};

		static Urho3D::Vector3 fGeneratingRhodos[4] = {
			Urho3D::Vector3(0.0f, 0.0f, 0.0f),
			Urho3D::Vector3(0.0f, 2.0f, 2.0f),
			Urho3D::Vector3(2.0f, 0.0f, 2.0f),
			Urho3D::Vector3(2.0f, 2.0f, 0.0f)
		};

		static const Urho3D::Vector3 jumpsToNeighbors[12] = {
			Urho3D::Vector3(0, 2, 2),
			Urho3D::Vector3(0, -2, 2),
			Urho3D::Vector3(2, 0, 2),
			Urho3D::Vector3(-2, 0, 2),
			Urho3D::Vector3(0, 2, -2),
			Urho3D::Vector3(0, -2, -2),
			Urho3D::Vector3(2, 0, -2),
			Urho3D::Vector3(-2, 0, -2),
			Urho3D::Vector3(2, 2, 0),
			Urho3D::Vector3(-2, 2, 0),
			Urho3D::Vector3(-2, -2, 0),
			Urho3D::Vector3(2, -2, 0)
		};

		static const Urho3D::Vector3 rhodoVertexCoords[14] = {
			Urho3D::Vector3(1, 1, 1),
			Urho3D::Vector3(1, 1, -1),
			Urho3D::Vector3(1, -1, 1),
			Urho3D::Vector3(1, -1, -1),
			Urho3D::Vector3(-1, 1, 1),
			Urho3D::Vector3(-1, 1, -1),
			Urho3D::Vector3(-1, -1, 1),
			Urho3D::Vector3(-1, -1, -1),
			Urho3D::Vector3(2, 0, 0),
			Urho3D::Vector3(-2, 0, 0),
			Urho3D::Vector3(0, 2, 0),
			Urho3D::Vector3(0, -2, 0),
			Urho3D::Vector3(0, 0, 2),
			Urho3D::Vector3(0, 0, -2)
		};

		static const Urho3D::Vector3 UP_NORTH_RHOMBUS[4] = {
			Urho3D::Vector3(0.0, 0.0, 2.0),
			Urho3D::Vector3(-1.0, 1.0, 1.0),
			Urho3D::Vector3(0.0, 2.0, 0.0),
			Urho3D::Vector3(1.0, 1.0, 1.0)
		};

		static const Urho3D::Vector3 UP_SOUTH_RHOMBUS[4] = {
			Urho3D::Vector3(0.0, 0.0, 2.0),
			Urho3D::Vector3(1.0, -1.0, 1.0),
			Urho3D::Vector3(0.0, -2.0, 0.0),
			Urho3D::Vector3(-1.0, -1.0, 1.0)
		};

		static const Urho3D::Vector3 UP_EAST_RHOMBUS[4] = {
			Urho3D::Vector3(0.0, 0.0, 2.0),
			Urho3D::Vector3(1.0, 1.0, 1.0),
			Urho3D::Vector3(2.0, 0.0, 0.0),
			Urho3D::Vector3(1.0, -1.0, 1.0)
		};

		static const Urho3D::Vector3 UP_WEST_RHOMBUS[4] = {
			Urho3D::Vector3(0.0, 0.0, 2.0),
			Urho3D::Vector3(-1.0, -1.0, 1.0),
			Urho3D::Vector3(-2.0, 0.0, 0.0),
			Urho3D::Vector3(-1.0, 1.0, 1.0)
		};

		static const Urho3D::Vector3 DOWN_NORTH_RHOMBUS[4] = {
			Urho3D::Vector3(0.0, 0.0, -2.0),
			Urho3D::Vector3(1.0, 1.0, -1.0),
			Urho3D::Vector3(0.0, 2.0, 0.0),
			Urho3D::Vector3(-1.0, 1.0, -1.0)
		};

		static const Urho3D::Vector3 DOWN_SOUTH_RHOMBUS[4] = {
			Urho3D::Vector3(0.0, 0.0, -2.0),
			Urho3D::Vector3(-1.0, -1.0, -1.0),
			Urho3D::Vector3(0.0, -2.0, 0.0),
			Urho3D::Vector3(1.0, -1.0, -1.0)
		};

		static const Urho3D::Vector3 DOWN_EAST_RHOMBUS[4] = {
			Urho3D::Vector3(0.0, 0.0, -2.0),
			Urho3D::Vector3(1.0, -1.0, -1.0),
			Urho3D::Vector3(2.0, 0.0, 0.0),
			Urho3D::Vector3(1.0, 1.0, -1.0)
		};

		static const Urho3D::Vector3 DOWN_WEST_RHOMBUS[4] = {
			Urho3D::Vector3(0.0, 0.0, -2.0),
			Urho3D::Vector3(-1.0, 1.0, -1.0),
			Urho3D::Vector3(-2.0, 0.0, 0.0),
			Urho3D::Vector3(-1.0, -1.0, -1.0)
		};

		static const Urho3D::Vector3 NORTH_EAST_RHOMBUS[4] = {
			Urho3D::Vector3(0.0, 2.0, 0.0),
			Urho3D::Vector3(1.0, 1.0, -1.0),
			Urho3D::Vector3(2.0, 0.0, 0.0),
			Urho3D::Vector3(1.0, 1.0, 1.0)
		};

		static const Urho3D::Vector3 NORTH_WEST_RHOMBUS[4] = {
			Urho3D::Vector3(-2.0, 0.0, 0.0),
			Urho3D::Vector3(-1.0, 1.0, -1.0),
			Urho3D::Vector3(0.0, 2.0, 0.0),
			Urho3D::Vector3(-1.0, 1.0, 1.0)
		};

		static const Urho3D::Vector3 SOUTH_WEST_RHOMBUS[4] = {
			Urho3D::Vector3(0.0, -2.0, 0.0),
			Urho3D::Vector3(-1.0, -1.0, -1.0),
			Urho3D::Vector3(-2.0, 0.0, 0.0),
			Urho3D::Vector3(-1.0, -1.0, 1.0)
		};

		static const Urho3D::Vector3 SOUTH_EAST_RHOMBUS[4] = {
			Urho3D::Vector3(2.0, 0.0, 0.0),
			Urho3D::Vector3(1.0, -1.0, -1.0),
			Urho3D::Vector3(0.0, -2.0, 0.0),
			Urho3D::Vector3(1.0, -1.0, 1.0)
		};

		static const Urho3D::Vector3* neighborRhombi[12] = {
			UP_NORTH_RHOMBUS,
			UP_SOUTH_RHOMBUS,
			UP_EAST_RHOMBUS,
			UP_WEST_RHOMBUS,
			DOWN_NORTH_RHOMBUS,
			DOWN_SOUTH_RHOMBUS,
			DOWN_EAST_RHOMBUS,
			DOWN_WEST_RHOMBUS,
			NORTH_EAST_RHOMBUS,
			NORTH_WEST_RHOMBUS,
			SOUTH_WEST_RHOMBUS,
			SOUTH_EAST_RHOMBUS
		};
	}

	class Rhodo {
	public:
		Rhodo(int family, int xDiamJumps, int yDiamJumps, int zDiamJumps)
			: family_(family), xDiamJumps_(xDiamJumps), yDiamJumps_(yDiamJumps), zDiamJumps_(zDiamJumps) { }
		Rhodo(Rhodo& originalRhodo, int jumpIndex);
		Rhodo(int xUnitless, int yUnitless, int zUnitless);

		int family_;
		int xDiamJumps_;
		int yDiamJumps_;
		int zDiamJumps_;

	public:
		Eigen::RowVector3i ComputeUnitlessCoordinates() const;
		Eigen::RowVector3f ComputeCoordinates(float s) const;

		Urho3D::VariantVector ComputeVertices(float scale) const;
		Urho3D::VariantVector ComputeRhombicPolylines(float scale) const;
	};

	struct RhodoCompare {
		bool operator() (const Rhodo& lhs, const Rhodo& rhs) const
		{
			if (lhs.family_ < rhs.family_) {
				return true;
			}
			else if (lhs.family_ == rhs.family_) {
				if (lhs.xDiamJumps_ < rhs.xDiamJumps_) {
					return true;
				}
				else if (lhs.xDiamJumps_ == rhs.xDiamJumps_) {
					if (lhs.yDiamJumps_ < rhs.yDiamJumps_) {
						return true;
					}
					else if (lhs.yDiamJumps_ == rhs.yDiamJumps_) {
						if (lhs.zDiamJumps_ < rhs.zDiamJumps_) {
							return true;
						}
					}
				}
			}
			return false;
		}
	};

	int ComputeFamilyFromUnitlessCoordinates(const Eigen::RowVector3i& unitlessCoordinates);
}