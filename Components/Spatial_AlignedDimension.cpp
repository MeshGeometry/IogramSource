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


#include "Spatial_AlignedDimension.h"
#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/UI/Text3D.h>
#include <Urho3D/UI/Font.h>

#include "Polyline.h"


using namespace Urho3D;

String Spatial_AlignedDimension::iconTexture = "Textures/Icons/Spatial_AlignedDimension.png";

Spatial_AlignedDimension::Spatial_AlignedDimension(Context* context) : IoComponentBase(context, 0, 0)
{
	//set up component info
	SetName("AddComponent");
	SetFullName("Add Component");
	SetDescription("Adds a native component to a scene node.");

	//set up the slots
	AddInputSlot(
		"Start",
		"A",
		"First dimension point.",
		VAR_VECTOR3,
		DataAccess::ITEM
	);

	AddInputSlot(
		"End",
		"B",
		"Second dimension point.",
		VAR_VECTOR3,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Offset",
		"O",
		"Offset",
		VAR_FLOAT,
		DataAccess::ITEM,
		1.0f
	);

	AddInputSlot(
		"Scale",
		"S",
		"Scale",
		VAR_FLOAT,
		DataAccess::ITEM,
		1.0f
	);

	AddInputSlot(
		"Up",
		"U",
		"Up direction hint.",
		VAR_VECTOR3,
		DataAccess::ITEM,
		Vector3::UP
	);

	AddInputSlot(
		"Color",
		"C",
		"Color.",
		VAR_COLOR,
		DataAccess::ITEM,
		0.7f * Color::BLACK
	);

	AddOutputSlot(
		"NodeID",
		"ID",
		"Node ID",
		VAR_INT,
		DataAccess::ITEM
	);

}

void Spatial_AlignedDimension::PreLocalSolve()
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (scene)
	{
		for (int i = 0; i < trackedItems.Size(); i++)
		{
			Node* n = scene->GetNode(trackedItems[i]);
			if (n != NULL)
			{
				n->Remove();
			}
		}
	}

	trackedItems.Clear();
}

//work function
void Spatial_AlignedDimension::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();
	if (scene == NULL)
	{
		URHO3D_LOGERROR("Null scene encountered!");
		outSolveInstance[0] = Variant();
		return;
	}
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	//get the data
	Vector3 startPt = inSolveInstance[0].GetVector3();
	Vector3 endPt = inSolveInstance[1].GetVector3();
	float offset = inSolveInstance[2].GetFloat();
	float scale = Abs(inSolveInstance[3].GetFloat());
	Vector3 upDir = inSolveInstance[4].GetVector3();
	Color color = inSolveInstance[5].GetColor();

	Vector3 edgeVec = endPt - startPt;
	Vector3 midPt = startPt + 0.5f * edgeVec;

	//possible rotate upDir
	if (Abs(upDir.DotProduct(edgeVec)) > 0.95f) {
		upDir = Vector3(-upDir.y_, upDir.x_, upDir.z_);
	}

	//get the perp dir
	Vector3 crossVec = edgeVec.CrossProduct(upDir);
	crossVec.Normalize();

	//create the root node
	Node* dimNode = scene->CreateChild("DimNode");
	Material* mat = NULL;
	mat = cache->GetResource<Material>("Materials/BasicCurve.xml");
	if (!mat) {
		cache->GetResource<Material>("Materials/BasicWebAlpha.xml");
	}

	BillboardSet* curveDisplay = dimNode->CreateComponent<BillboardSet>();
	SharedPtr<Material> firstMat = mat->Clone();
	curveDisplay->SetNumBillboards(3);
	curveDisplay->SetMaterial(firstMat);
	curveDisplay->SetSorted(true);
	curveDisplay->SetFaceCameraMode(FaceCameraMode::FC_DIRECTION);

	//create aligned stroke
	float crossExtend = 0.1f * scale;
	Billboard* bb = curveDisplay->GetBillboard(0);
	bb->position_ = midPt + offset * crossVec;
	bb->size_ = Vector2(0.01f * scale, 0.5f * (edgeVec.Length()) + crossExtend);
	bb->direction_ = edgeVec;
	bb->enabled_ = true;
	bb->color_ = color;

	//create first cross stroke
	bb = curveDisplay->GetBillboard(1);
	bb->position_ = startPt +  0.5f * crossVec * (offset + crossExtend);
	bb->size_ = Vector2(0.01f * scale, 0.5f * (Abs(offset) * crossVec.Length()) + Sign(offset) * 0.5f * crossExtend);
	bb->direction_ = crossVec;
	bb->enabled_ = true;
	bb->color_ = color;

	//create second cross stroke
	bb = curveDisplay->GetBillboard(2);
	bb->position_ = endPt + 0.5f * crossVec * (offset + crossExtend);
	bb->size_ = Vector2(0.01f * scale, 0.5f * (Abs(offset) * crossVec.Length()) + Sign(offset) * 0.5f * crossExtend);
	bb->direction_ = crossVec;
	bb->enabled_ = true;
	bb->color_ = color;

	//create the label
	String displayText = String(edgeVec.Length());
	Vector3 pos = midPt + (offset + crossExtend) * crossVec;
	String fontName = "Fonts/Anonymous Pro.sdf";

	Node* textNode = dimNode->CreateChild("3DText");
	Text3D* text = textNode->CreateComponent<Text3D>();
	Font* f = GetSubsystem<ResourceCache>()->GetResource<Font>(fontName);
	if (f) {
		text->SetFont(f);
	}
	text->SetText(displayText);
	text->SetColor(color);
	Quaternion rot = Quaternion(edgeVec.Normalized(), crossVec, edgeVec.CrossProduct(crossVec).Normalized());
	textNode->SetRotation(rot);
	textNode->SetPosition(pos);
	textNode->SetScale(0.75f * Vector3(scale, scale, scale));
	text->SetAlignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER);
	text->SetFaceCameraMode(FaceCameraMode::FC_DIRECTION);

	trackedItems.Push(dimNode->GetID());
	outSolveInstance[0] = dimNode->GetID();

}