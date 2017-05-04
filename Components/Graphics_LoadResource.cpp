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


#include "Graphics_LoadResource.h"

#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

String Graphics_LoadResource::iconTexture = "Textures/Icons/Graphics_LoadResource.png";

Graphics_LoadResource::Graphics_LoadResource(Urho3D::Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("LoadResource");
	SetFullName("Load Resource");
	SetDescription("Loads a resource from a path");
	SetGroup(IoComponentGroup::DISPLAY);
	SetSubgroup("");

	inputSlots_[0]->SetName("Path");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Path to Resource file (e.g. \"Models/TeaPot.mdl\")");
	inputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	//inputSlots_[0]->SetDefaultValue("Models/Box.mdl");

	inputSlots_[1]->SetName("Type");
	inputSlots_[1]->SetVariableName("T");
	inputSlots_[1]->SetDescription("Type Name. Must be a registered class type. (e.g. \"model\")");
	inputSlots_[1]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	//inputSlots_[1]->SetDefaultValue("Model");

	outputSlots_[0]->SetName("ResourcePointer");
	outputSlots_[0]->SetVariableName("R");
	outputSlots_[0]->SetDescription("Void Pointer to Resource");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VOIDPTR); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Graphics_LoadResource::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	String resPath = inSolveInstance[0].GetString();
	URHO3D_LOGINFO("model path: " + resPath);
	String typeName = inSolveInstance[1].GetString();
	URHO3D_LOGINFO("type name: " + typeName);
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	StringHash typeHash(typeName);
	if (GetContext()->GetTypeName(typeHash).Empty())
	{
		URHO3D_LOGERROR("Type name not registered with Context: " + typeName);
		return;
	}

	Resource* resource = cache->GetResource(typeHash, resPath);
	
	if (resource)
	{
		outSolveInstance[0] = resource;
	}
	else
	{
		URHO3D_LOGERROR("Could not load resource: " + resPath);
	}
}
