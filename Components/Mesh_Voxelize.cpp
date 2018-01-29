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


#include "Mesh_Voxelize.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Plane.h>

#include "TriMesh.h"
#include "Polyline.h"
#include "Geomlib_ConstructTransform.h"
#include "igl/ray_mesh_intersect.h"
#include "igl/voxel_grid.h"
#include "igl/signed_distance.h"
#include "igl/AABB.h"
#include "igl/copyleft/marching_cubes.h"

using namespace Urho3D;
using namespace Eigen;
String Mesh_Voxelize::iconTexture = "Textures/Icons/Mesh_Voxelize.png";



Mesh_Voxelize::Mesh_Voxelize(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("ProjectOnto");
	SetFullName("ProjectOnto");
	SetDescription("Projects geometry on to other geometry");

	AddInputSlot(
		"Source",
		"S",
		"Source geometry",
		VAR_VARIANTMAP,
		ITEM
	);

	AddInputSlot(
		"CellCount",
		"C",
		"CellCount",
		VAR_FLOAT,
		ITEM,
		1.0f
	);

	AddInputSlot(
		"Base",
		"B",
		"Base transform",
		VAR_MATRIX3X4,
		ITEM,
		Matrix3x4::IDENTITY
	);

	AddOutputSlot(
		"In",
		"In",
		"Voxel Centers",
		VAR_VECTOR3,
		LIST
	);

	AddOutputSlot(
		"Out",
		"Out",
		"Voxel Centers",
		VAR_VECTOR3,
		LIST
	);

	AddOutputSlot(
		"On",
		"On",
		"Voxel Centers",
		VAR_VECTOR3,
		LIST
	);

	AddOutputSlot(
		"Flags",
		"Flags",
		"Voxel Flags",
		VAR_INT,
		LIST
	);



}

void Mesh_Voxelize::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{

	//check that we have the right types
	if (!TriMesh_Verify(inSolveInstance[0]))
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//num cells on longest side
	float cellSize = inSolveInstance[1].GetFloat();

	//find bounding box
	Variant box_mesh = TriMesh_BoundingBox(inSolveInstance[0]);
	VariantVector vertexList = TriMesh_GetVertexList(box_mesh);
	Vector3 min = vertexList[0].GetVector3();
	Vector3 max = vertexList[0].GetVector3();
	for (int i = 0; i < vertexList.Size(); ++i) {
		Vector3 curVert = vertexList[i].GetVector3();
		if (curVert.x_ <= min.x_ && curVert.y_ <= min.y_ && curVert.z_ <= min.z_)
			min = curVert;
		if (curVert.x_ >= max.x_ && curVert.y_ >= max.y_ && curVert.z_ >= max.z_)
			max = curVert;
	}

	Vector3 centre = (max + min) / 2;
	float diag = (max - min).Length();

	//figure out num cells from max length
	float maxLength = Max(max.x_ - min.x_, max.y_ - min.y_);
	maxLength = Max(maxLength, max.z_ - min.z_);
	int numCells = CeilToInt(maxLength / cellSize);
	numCells = Clamp(numCells, 6, 1000);

	//create voxel grid
	Eigen::MatrixXd vg;
	Eigen::RowVector3i res;
	Eigen::AlignedBox3d bb(Eigen::RowVector3d(min.x_, min.y_, min.z_), Eigen::RowVector3d(max.x_, max.y_, max.z_));
	igl::voxel_grid(bb, numCells + 1, 1, vg, res);

	//create sdf
	Eigen::MatrixXd V;
	Eigen::MatrixXi F;

	TriMeshToDoubleMatrices(inSolveInstance[0], V, F);

	Eigen::VectorXd S;
	Eigen::VectorXi I;
	Eigen::MatrixXd C;
	Eigen::MatrixXd N;

	igl::signed_distance(vg, V, F, igl::SignedDistanceType::SIGNED_DISTANCE_TYPE_DEFAULT, S, I, C, N);

	VariantVector in;
	VariantVector out;
	VariantVector srf;
	VariantVector flags;

	int counter = 0;
	for (int i = 0; i < res.z(); i++)
	{
		for (int j = 0; j < res.y(); j++)
		{
			for (int k = 0; k < res.x(); k++)
			{
				int index = i * res.x() * res.y() + j * res.x() + k;
				Eigen::RowVector3d vp = vg.row(index);
				
				//get neighbours
				bool isIn = S[index] <= 0 ? true : false;
				bool onSrf = false;
				int nIndex = index;
				int flag = 0;
				int bit = 0;
				for (int n = 0; n < 6; n++)
				{
					switch (n)
					{
					case 0:
						nIndex = Clamp(i - 1, 0, res.z() - 1);
						nIndex = nIndex * res.x() * res.y() + j * res.x() + k;
						break;
					case 1:
						nIndex = Clamp(i + 1, 0, res.z() - 1);
						nIndex = nIndex * res.x() * res.y() + j * res.x() + k;
						break;
					case 2:
						nIndex = Clamp(j - 1, 0, res.y() - 1);
						nIndex = i * res.x() * res.y() + nIndex * res.x() + k;
						break;
					case 3:
						nIndex = Clamp(j + 1, 0, res.y() - 1);
						nIndex = i * res.x() * res.y() + nIndex * res.x() + k;
						break;
					case 4:
						nIndex = Clamp(k - 1, 0, res.x() - 1);
						nIndex = i * res.x() * res.y() + j * res.x() + nIndex;
						break;
					case 5:
						nIndex = Clamp(k + 1, 0, res.x() - 1);
						nIndex = i * res.x() * res.y() + j * res.x() + nIndex;
						break;
					}

					//check if neighbour matches center. If not, it is on boundary
					bool nIsIn = S[nIndex] <= 0 ? true : false;
					if (nIsIn != isIn)
					{
						onSrf = true;
		
					}
					else //track positive neighbours
					{
						bit = Pow(2, n);
						flag |= bit;
					}
				}
				
				// push to right list
				Vector3 cp(vp.x(), vp.y(), vp.z());
				if (onSrf && isIn)
				{
					srf.Push(cp);
					flags.Push(flag);
				}
				else if (isIn)
				{
					in.Push(cp);
				}
				else if (!isIn)
				{
					out.Push(cp);
				}
			}
		}
	}

	outSolveInstance[0] = in;
	outSolveInstance[1] = out;
	outSolveInstance[2] = srf;
	outSolveInstance[3] = flags;

}
