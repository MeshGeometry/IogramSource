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


#include "Scene_ModifyComponent.h"

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

String Scene_ModifyComponent::iconTexture = "Textures/Icons/Scene_ModifyComponent.png";

Scene_ModifyComponent::Scene_ModifyComponent(Context* context) : IoComponentBase(context, 0, 0)
{
	//set up component info
	SetName("ModifyComponent");
	SetFullName("Modify Component");
	SetDescription("Modifies the properties of a native component.");

	//set up the slots
	AddInputSlot(
		"Component Reference",
		"C",
		"Pointer reference to native component.",
		VAR_PTR,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Property",
		"P",
		"Name of Property to modify.",
		VAR_STRING,
		DataAccess::LIST
	);

	AddInputSlot(
		"Value",
		"V",
		"Value with which to modify property.",
		VAR_FLOAT,
		DataAccess::LIST
	);

	AddOutputSlot(
		"Reference",
		"Ptr",
		"Reference to modified component",
		VAR_PTR,
		DataAccess::ITEM
	);
}

//work function
void Scene_ModifyComponent::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	Component* comp = (Component*)inSolveInstance[0].GetPtr();
	if (comp == NULL)
	{
		
		comp = (Component*)inSolveInstance[0].GetVoidPtr();
		if (!comp)
		{
			URHO3D_LOGERROR("Could not cast input as a native Component!");
			outSolveInstance[0] = Variant();
			return;
		}

	}

	//iterate over properties and values and try to assign
	VariantVector propNames = inSolveInstance[1].GetVariantVector();
	VariantVector propVals = inSolveInstance[2].GetVariantVector();

	int count = Min(propNames.Size(), propVals.Size());

	for (int i = 0; i < count; i++)
	{
		//make sure that the supplied property actually exists
		String attributeName = propNames[i].GetString();
		Variant attriVar = comp->GetAttribute(attributeName);

		if (attriVar.GetType() != VAR_NONE)
		{
			if (attriVar.GetType() == propVals[i].GetType())
			{
				comp->SetAttribute(attributeName, propVals[i]);
			}
			else if (attriVar.GetType() == VAR_RESOURCEREFLIST)
			{

				Resource* resource = (Resource*)propVals[i].GetPtr();

				if (resource)
				{
					ResourceRefList oldRef = attriVar.GetResourceRefList();
					StringVector oldNames = oldRef.names_;

					for (int i = 0; i < oldNames.Size(); i++)
					{
						oldNames[i] = resource->GetName();
					}
					
					ResourceRefList newRef(oldRef.type_, oldNames);
					comp->SetAttribute(attributeName, newRef);
				}
			}
			else if (attriVar.GetType() == VAR_RESOURCEREF)
			{

				Resource* resource = (Resource*)propVals[i].GetPtr();

				if (resource)
				{
					ResourceRef oldRef = attriVar.GetResourceRef();
					oldRef.name_ = resource->GetName();
					comp->SetAttribute(attributeName, oldRef);

				}
			}
			else
			{
				URHO3D_LOGERROR("Mismatch in supplied attribuate type");
			}
		}
		else
		{
			URHO3D_LOGERROR("Could not find attribute: " + attributeName + " in component: " + comp->GetTypeNameStatic());
		}
	}

	outSolveInstance[0] = comp;
}