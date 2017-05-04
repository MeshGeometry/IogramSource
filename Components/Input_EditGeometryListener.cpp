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


#include "Input_EditGeometryListener.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

#include "IoGraph.h"

using namespace Urho3D;

String Input_EditGeometryListener::iconTexture = "Textures/Icons/Input_EditGeometryListener.png";


Input_EditGeometryListener::Input_EditGeometryListener(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("EditGeometryListener");
	SetFullName("EditGeometryListener");
	SetDescription("Listens for updates to editable geometry");

	AddInputSlot(
		"NodeID",
		"ID",
		"Id of edit geometry to listen to.",
		VAR_INT,
		ITEM
		);

	AddOutputSlot(
		"Geometry",
		"G",
		"Edited geometry",
		VAR_VARIANTMAP,
		ITEM
		);

	//AddOutputSlot(
	//	"Displacements",
	//	"D",
	//	"Displacement vectors of original geometry to new.",
	//	VAR_VARIANTMAP,
	//	ITEM
	//	);

	SubscribeToEvent("EditGeometryUpdate", URHO3D_HANDLER(Input_EditGeometryListener, HandleEditGeometry));

}

void Input_EditGeometryListener::PreLocalSolve()
{
	//UnsubscribeFromAllEvents();
}

void Input_EditGeometryListener::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	//int nodeID = inSolveInstance[0].GetInt();

	//Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	//if (scene)
	//{
	//	Node* n = scene->GetChild(nodeID);
	//	if (n)
	//	{
	//		
	//	}
	//}

	outSolveInstance[0] = currentGeometry;
}

void Input_EditGeometryListener::HandleEditGeometry(StringHash eventType, VariantMap& eventData)
{
	Node* n = (Node*)eventData["NodeReference"].GetPtr();
	if (n)
	{
		currentGeometry = n->GetVar("ReferenceGeometry");
		solvedFlag_ = 0;
		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
	}
}