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


#include "Geometry_WriteDxf.h"
#include "DxfWriter.h"
#include "TriMesh.h"
#include "Polyline.h"
#include "NMesh.h"

#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

String Geometry_WriteDXF::iconTexture = "Textures/Icons/Geometry_WriteDXF.png";


Geometry_WriteDXF::Geometry_WriteDXF(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("WriteDXF");
	SetFullName("Write DXF");
	SetDescription("Writes a DXF file. Supports Meshes, Polylines, and Points.");

	AddInputSlot(
		"Path",
		"P",
		"Path to DXF",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Z Up",
		"Z",
		"Force Z Up.",
		VAR_BOOL,
		DataAccess::ITEM,
		true
	);

	AddInputSlot(
		"Layer",
		"L",
		"Layer",
		VAR_STRING,
		DataAccess::ITEM,
		"Default"
	);

	AddInputSlot(
		"Meshes",
		"M",
		"Meshes",
		VAR_VARIANTMAP,
		DataAccess::LIST
	);

	AddInputSlot(
		"Polylines",
		"PL",
		"Polylines",
		VAR_VARIANTMAP,
		DataAccess::LIST
	);

	AddInputSlot(
		"Points",
		"PT",
		"Points",
		VAR_VECTOR3,
		DataAccess::LIST
	);

	AddOutputSlot(
		"Result",
		"Res",
		"Result",
		VAR_STRING,
		DataAccess::ITEM
	);

}

void Geometry_WriteDXF::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	//get the path
	String dxfPath = inSolveInstance[0].GetString();
	bool forceZUp = inSolveInstance[1].GetBool();
	String layer = inSolveInstance[2].GetString();

	if (dxfPath.Empty()) {
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	int len = dxfPath.Length();
	if (
		len < 4 ||
		dxfPath.Substring(len - 4) != String(".dxf")
		)
	{
		dxfPath += ".dxf";
	}

	layer = layer.Empty() ? "Default" : layer;

	//create the writer
	DxfWriter* dWriter = new DxfWriter(GetContext());

	//get the geometry
	VariantVector meshes = inSolveInstance[3].GetVariantVector();
	if (meshes.Size() == 1) {
		if (meshes[0].GetType() == VAR_NONE) {
			meshes = VariantVector();
		}
	}
	VariantVector polys = inSolveInstance[4].GetVariantVector();
	if (polys.Size() == 1) {
		if (polys[0].GetType() == VAR_NONE) {
			polys = VariantVector();
		}
	}
	VariantVector points = inSolveInstance[5].GetVariantVector();
	if (points.Size() == 1) {
		if (points[0].GetType() == VAR_NONE) {
			points = VariantVector();
		}
	}

	//set the meshes
	for (int i = 0; i < meshes.Size(); i++)
	{
		VariantMap* mMap = meshes[i].GetVariantMapPtr();

		//only support triangle meshes right now
		if (TriMesh_Verify((*mMap))) {
			dWriter->SetMesh((*mMap)["vertices"].GetVariantVector(), (*mMap)["faces"].GetVariantVector(), layer);
		}
		else if (NMesh_Verify((*mMap))) {
			Variant triMesh = NMesh_ConvertToTriMesh(*mMap);
			dWriter->SetMesh(TriMesh_GetVertexList(triMesh), TriMesh_GetFaceList(triMesh), layer);
		}

	}

	//set the polylines
	for (int i = 0; i < polys.Size(); i++) {

		VariantMap* p = polys[i].GetVariantMapPtr();
		if (Polyline_Verify(*p)) {
			VariantVector verts = Polyline_ComputeSequentialVertexList(*p);
			dWriter->SetPolyline(verts, layer);
		}
	}

	//set the points
	dWriter->SetPoints(points, layer);

	//save
	dWriter->Save(dxfPath);

	outSolveInstance[0] = dxfPath;

}
