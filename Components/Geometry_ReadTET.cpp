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

#include "Geometry_ReadTET.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "Geomlib_ReadOBJ.h"
#include "TriMesh.h"

using namespace Urho3D;

String Geometry_ReadTET::iconTexture = "Textures/Icons/Geometry_ReadTET.png";

Geometry_ReadTET::Geometry_ReadTET(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("ReadOBJ");
	SetFullName("ReadOBJ");
	SetDescription("Read TriMesh from OBJ file");

	inputSlots_[0]->SetName("File");
	inputSlots_[0]->SetVariableName("File");
	inputSlots_[0]->SetDescription("File");
	inputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("ToYUp");
	inputSlots_[1]->SetVariableName("ToYUp");
	inputSlots_[1]->SetDescription("Transform coords to Y Up");
	inputSlots_[1]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(false);
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Geometry_ReadTET::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	String filename = inSolveInstance[0].GetString();
	if (filename.Empty()) {
		URHO3D_LOGERROR("Geometry_ReadTET --- invalid File");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	SharedPtr<FileSystem> fs(GetSubsystem<FileSystem>());
	bool cached = false;
	SharedPtr<File> obj_file = GetSubsystem<ResourceCache>()->GetFile(filename);
	if (!obj_file) {
		obj_file = SharedPtr<File>(new File(GetContext(), filename));
	}

	if (!obj_file)
	{
		URHO3D_LOGERROR("Geometry_ReadTET --- invalid File");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	bool yup = inSolveInstance[1].GetBool();

	int numPts = 0;
	int numTets = 0;

	std::sscanf(obj_file->ReadLine().CString(), "%d %d", &numPts, &numTets);

	VariantVector pts;
	VariantVector tets;
	VariantVector faces;

	for (int i = 0; i < numPts; i++)
	{
		Vector3 pt;
		std::sscanf(obj_file->ReadLine().CString(), "%f %f %f", &pt.x_, &pt.y_, &pt.z_);
		if (yup)
		{
			float y = pt.y_;
			pt.y_ = pt.z_;
			pt.z_ = -y;
		}
		pts.Push(pt);
	}

	for (int i = 0; i < numTets; i++)
	{
		int a, b, c, d;
		std::sscanf(obj_file->ReadLine().CString(), "%d %d %d %d", &a, &b, &c, &d);
		tets.Push(a);
		tets.Push(b);
		tets.Push(c);
		tets.Push(d);
	}

	//TODO: somehow figure out the surface faces

	VariantMap vm;
	vm["type"] = "TriMesh";
	vm["vertices"] = pts;
	vm["faces"] = faces;
	vm["tetrahedra"] = tets;

	outSolveInstance[0] = vm;
}