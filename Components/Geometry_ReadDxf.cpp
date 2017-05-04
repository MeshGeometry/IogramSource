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


#include "Geometry_ReadDxf.h"
#include "DxfReader.h"
#include "TriMesh.h"
#include "Polyline.h"
#include "NMesh.h"

#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

String Geometry_ReadDXF::iconTexture = "Textures/Icons/Geometry_ReadDXF.png";


Geometry_ReadDXF::Geometry_ReadDXF(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("ReadDXF");
	SetFullName("Read DXF");
	SetDescription("Reads a DXF file. Supports Meshes, Polylines, and Points.");

	AddInputSlot(
		"Path",
		"P",
		"Path to DXF",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Y Up",
		"Y",
		"Force Y Up.",
		VAR_BOOL,
		DataAccess::ITEM,
		true
	);

	AddOutputSlot(
		"Meshes",
		"M",
		"Meshes",
		VAR_VARIANTMAP,
		DataAccess::LIST
	);

	AddOutputSlot(
		"Polylines",
		"PL",
		"Polylines",
		VAR_VARIANTMAP,
		DataAccess::LIST
	);

	AddOutputSlot(
		"Points",
		"PT",
		"Points",
		VAR_VECTOR3,
		DataAccess::LIST
	);

}

void Geometry_ReadDXF::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	//get the path
	String meshFile = inSolveInstance[0].GetString();
	bool forceYUp = inSolveInstance[1].GetBool();
	
	//construct a file using resource cache
	SharedPtr<File> rf = GetSubsystem<ResourceCache>()->GetFile(meshFile);

	if (!rf) {
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//create DXFREader
	DxfReader* dReader = new DxfReader(GetContext(), rf);

	//read the file
	dReader->Parse();

	//export results
	VariantVector meshes = dReader->GetMeshes();
	VariantVector polys = dReader->GetPolylines();
	VariantVector points = dReader->GetPoints();

	//get the meshes
	VariantVector meshesOut;
	for (int i = 0; i < meshes.Size(); i++)
	{
		VariantMap* mMap = meshes[i].GetVariantMapPtr();
		VariantVector* verts = (*mMap)["Vertices"].GetVariantVectorPtr();
		VariantVector* faces = (*mMap)["Faces"].GetVariantVectorPtr();

		meshesOut.Push(TriMesh_Make(*verts, *faces));
	}

	//get the polys
	VariantVector polysOut;
	for (int i = 0; i < polys.Size(); i++)
	{
		VariantMap* pMap = polys[i].GetVariantMapPtr();
		VariantVector* verts = (*pMap)["Vertices"].GetVariantVectorPtr();

		polysOut.Push(Polyline_Make(*verts));
	}

	//get the points
	VariantVector pointsOut;
	for (int i = 0; i < points.Size(); i++)
	{
		VariantMap* pMap = points[i].GetVariantMapPtr();
		Vector3 v = (*pMap)["Position"].GetVector3();

		pointsOut.Push(v);
	}

	outSolveInstance[0] = meshesOut;
	outSolveInstance[1] = polysOut;
	outSolveInstance[2] = pointsOut;

}
