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


#include "Graphics_Zone.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/TextureCube.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Material.h>

#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

String Graphics_Zone::iconTexture = "Textures/Icons/Graphics_Zone.png";


Graphics_Zone::Graphics_Zone(Urho3D::Context* context) : IoComponentBase(context, 4, 1)
{
	SetName("RenderSettings");
	SetFullName("Base Render Settings");
	SetDescription("Sets a few of the most important render settings.");

	inputSlots_[0]->SetName("Zone Size");
	inputSlots_[0]->SetVariableName("ZS");
	inputSlots_[0]->SetDescription("Vector defined size of the render zone");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDefaultValue(Vector3(-1000.0f, 1000.0f, 0.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("AmbientLight");
	inputSlots_[1]->SetVariableName("AL");
	inputSlots_[1]->SetDescription("Ambient Light. Alpha channel determines brightness.");
	inputSlots_[1]->SetVariantType(VariantType::VAR_COLOR);
	inputSlots_[1]->SetDefaultValue(Color(0.1f, 0.1f, 0.1f, 1.0f));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Fog Color");
	inputSlots_[2]->SetVariableName("FC");
	inputSlots_[2]->SetDescription("Color of Fog.");
	inputSlots_[2]->SetVariantType(VariantType::VAR_COLOR);	
	inputSlots_[2]->SetDefaultValue(Color(0.5f, 0.5f, 0.5f, 1.0f));
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("Fog extents");
	inputSlots_[3]->SetVariableName("FE");
	inputSlots_[3]->SetDescription("Fog extents. Vector 3 where x,y are start and end distances. Z is density.");
	inputSlots_[3]->SetVariantType(VariantType::VAR_VECTOR3);		
	inputSlots_[3]->SetDefaultValue(Vector3(300.0f, 600.0f, 0.0f));
	inputSlots_[3]->DefaultSet();

	outputSlots_[0]->SetName("Zone");
	outputSlots_[0]->SetVariableName("Z");
	outputSlots_[0]->SetDescription("Pointer to zone");
	outputSlots_[0]->SetVariantType(VariantType::VAR_PTR);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

}

void Graphics_Zone::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	//first, get the scene
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (!scene)
	{
		URHO3D_LOGERROR("Could not get scene");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	Node* zoneNode = scene->GetChild("Zone");
	
	if (zoneNode == NULL)
	{
		URHO3D_LOGERROR("Could not find sky or zone nodes...");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	Zone* zone = zoneNode->GetComponent<Zone>();

	if (zone == NULL)
	{
		URHO3D_LOGERROR("Could not find sky or zone components...");
		SetAllOutputsNull(outSolveInstance);
		return;
	}


	//zone size
	Vector3 zSize = inSolveInstance[0].GetVector3();
	zone->SetBoundingBox(BoundingBox(zSize.x_, zSize.y_));

	//ambient light
	Color aCol = inSolveInstance[1].GetColor();
	zone->SetAmbientColor(aCol);
	
	//set fog col
	Color fCol = inSolveInstance[2].GetColor();
	zone->SetFogColor(fCol);

	//set fog extents
	Vector3 fExt = inSolveInstance[3].GetVector3();
	zone->SetFogStart(fExt.x_);
	zone->SetFogEnd(fExt.y_);

	outSolveInstance[0] = zone;
}