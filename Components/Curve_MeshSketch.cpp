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


#include "Curve_MeshSketch.h"
#include "MeshSketcher.h"
#include "Polyline.h"
#include "IoGraph.h"
#include <assert.h>


using namespace Urho3D;

String Curve_MeshSketch::iconTexture = "Textures/Icons/Curve_MeshSketchPolyline.png";

Curve_MeshSketch::Curve_MeshSketch(Context* context) :
	IoComponentBase(context, 0, 0)
{
	SetName("MeshSketch");
	SetFullName("Mesh Sketch");
	SetDescription("Sketch on a Mesh");

	
	AddInputSlot(
		"Surface",
		"S",
		"Surface to sketch on",
		VAR_INT,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Reset",
		"R",
		"Clears the curves",
		VAR_BOOL,
		DataAccess::ITEM,
		false
	);
	

	AddOutputSlot(
		"Curves",
		"C",
		"The curves.",
		VAR_VARIANTMAP,
		DataAccess::LIST
	);

	AddOutputSlot(
		"Last",
		"L",
		"Last modified curve",
		VAR_VARIANTMAP,
		DataAccess::ITEM
	);

	SubscribeToEvent("GraphNodeDelete", URHO3D_HANDLER(Curve_MeshSketch, HandleNodeDeleted));
}

void Curve_MeshSketch::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();

	if (scene == NULL)
	{
		URHO3D_LOGERROR("Null scene encountered.");
		return;
	}


	int nodeId = inSolveInstance[0].GetInt();
	Node* node = scene->GetNode(nodeId);
	if (!node)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	StaticModel* sm = node->GetComponent<StaticModel>();
	if (!sm)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	bool reset = inSolveInstance[1].GetBool();
	if (reset)
	{
		surfacesCurves_.Clear();
	}

	if (node->GetComponent<MeshSketcher>())
	{
		SetAllOutputsNull(outSolveInstance);
		return;

	}
	MeshSketcher* ms = node->CreateComponent<MeshSketcher>();
	ms->SetSketchSurface(sm);

	SubscribeToEvent(ms, "MeshSketchChanged", URHO3D_HANDLER(Curve_MeshSketch, HandleSketchChanged));

	outSolveInstance[0] = Variant();
}


void Curve_MeshSketch::HandleSketchChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	VariantVector lastCurve = eventData["LastCurve"].GetVariantVector();
	Variant polyline = Polyline_Make(lastCurve);
	surfacesCurves_.Push(polyline);


	outputSlots_[0]->SetIoDataTree(IoDataTree(GetContext(), surfacesCurves_));
	outputSlots_[1]->SetIoDataTree(IoDataTree(GetContext(), polyline));


	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();

}

void Curve_MeshSketch::HandleNodeDeleted(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	IoComponentBase* comp = (IoComponentBase*)eventData["GraphNode"].GetPtr();
	if (comp == this)
	{
		
		surfacesCurves_.Clear();

		UnsubscribeFromAllEvents();
	}
}