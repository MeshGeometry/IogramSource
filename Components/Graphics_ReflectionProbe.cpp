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

#include "Graphics_ReflectionProbe.h"

#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Resource/Image.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Core/StringUtils.h>

#include "ReflectionProbe.h"

#include "Geomlib_ConstructTransform.h"


using namespace Urho3D;

String Graphics_ReflectionProbe::iconTexture = "Textures/Icons/Graphics_ReflectionProbe.png";

Graphics_ReflectionProbe::Graphics_ReflectionProbe(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("ReflectionProbe");
	SetFullName("Reflection Probe");
	SetDescription("Create a refleciton probe for environment maps.");

	////////////////////////////////

	AddInputSlot(
		"Transform",
		"T",
		"Transform",
		VAR_MATRIX3X4,
		ITEM,
		Matrix3x4::IDENTITY
	);

	AddInputSlot(
		"Resolution",
		"R",
		"Resolution",
		VAR_INT,
		ITEM,
		512
	);

	AddInputSlot(
		"Color",
		"C",
		"Color",
		VAR_COLOR,
		ITEM,
		Color::CYAN
	);

	////////////////////////////////

	AddOutputSlot(
		"NodeID",
		"ID",
		"Node ID",
		VAR_INT,
		ITEM
	);

	AddOutputSlot(
		"Probe",
		"RP",
		"ProbePtr",
		VAR_VOIDPTR,
		ITEM
	);


}

void Graphics_ReflectionProbe::PreLocalSolve()
{
	//delete old nodes
	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();
	if (scene)
	{
		for (int i = 0; i < trackedItems_.Size(); i++)
		{
			Node* oldNode = scene->GetNode(trackedItems_[i]);
			if (oldNode)
			{
				oldNode->Remove();
			}

		}

		trackedItems_.Clear();
	}

}

void Graphics_ReflectionProbe::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{

	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	if (scene == NULL)
	{
		URHO3D_LOGERROR("Null scene encountered.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//get the data
	Matrix3x4 xform = Geomlib::ConstructTransform(inSolveInstance[0]);
	int res = inSolveInstance[1].GetInt();

	res = Clamp(res, 4, 4096);
	Color col = inSolveInstance[2].GetColor();


	//create the light
	Node* node = scene->CreateChild("ReflectionProbe");

	//apply transform	
	node->SetPosition(xform.Translation());
	node->SetRotation(xform.Rotation());
	node->SetScale(xform.Scale());

	//create the probe
	ReflectionProbe* rp = node->CreateComponent<ReflectionProbe>();
	rp->SetResolution(res);
	rp->Initialize();
	rp->UpdateReflectionMap();

	trackedItems_.Push(node->GetID());

	outSolveInstance[0] = node->GetID();
	outSolveInstance[1] = rp;

}
