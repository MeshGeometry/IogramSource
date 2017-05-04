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


#include "Graphics_PointRenderer.h"
#include "ColorDefs.h"
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Texture2D.h>

#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

String Graphics_PointRenderer::iconTexture = "Textures/Icons/Graphics_PointRenderer.png";


Graphics_PointRenderer::Graphics_PointRenderer(Urho3D::Context* context) : IoComponentBase(context, 5, 2)
{
	SetName("Display");
	SetFullName("Geometry Display");
	SetDescription("Displays geometry in the scene");

	inputSlots_[0]->SetName("Points");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Points to render");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDataAccess(DataAccess::LIST);

	inputSlots_[1]->SetName("Color");
	inputSlots_[1]->SetVariableName("C");
	inputSlots_[1]->SetDescription("Color to Display");
	inputSlots_[1]->SetVariantType(VariantType::VAR_COLOR);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Color(0.9f, 0.6f, 0.9f, 1.0f));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Sprite");
	inputSlots_[2]->SetVariableName("S");
	inputSlots_[2]->SetDescription("Sprite to use for points");
	inputSlots_[2]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue("Textures/SpotWide.png");
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("Size");
	inputSlots_[3]->SetVariableName("D");
	inputSlots_[3]->SetDescription("Dimensions of point sprite");
	inputSlots_[3]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->SetDefaultValue(Vector3(0.05f, 0.05f, 0.0f));
	inputSlots_[3]->DefaultSet();

	inputSlots_[4]->SetName("FixedSize");
	inputSlots_[4]->SetVariableName("F");
	inputSlots_[4]->SetDescription("Use a fixed screen size");
	inputSlots_[4]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[4]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[4]->SetDefaultValue(false);
	inputSlots_[4]->DefaultSet();

	outputSlots_[0]->SetName("Node");
	outputSlots_[0]->SetVariableName("ID");
	outputSlots_[0]->SetDescription("Node ID of display");
	outputSlots_[0]->SetVariantType(VariantType::VAR_INT); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("BillboardSet");
	outputSlots_[1]->SetVariableName("B");
	outputSlots_[1]->SetDescription("Pointer to billboard set");
	outputSlots_[1]->SetVariantType(VariantType::VAR_PTR); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);

}

void Graphics_PointRenderer::PreLocalSolve()
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (scene)
	{
		for (int i = 0; i < trackedItems.Size(); i++)
		{
			Node* n = scene->GetNode(trackedItems[i]);
			if (n)
			{
				n->RemoveAllComponents();
				n->Remove();
			}
		}
	}

	trackedItems.Clear();
}

void Graphics_PointRenderer::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{

	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (!scene)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	ResourceCache* rc = GetSubsystem<ResourceCache>();
	String pointMat = "Materials/BasicPoints.xml";
	Material* baseMat = rc->GetResource<Material>(pointMat);

	if (!baseMat)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	VariantVector points = inSolveInstance[0].GetVariantVector();

	if (points.Empty() || points[0].GetType() == VAR_NONE)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}


	//get the other data
	String spritePath = inSolveInstance[2].GetString();
	Color mainColor = inSolveInstance[1].GetColor();
	Vector3 size = inSolveInstance[3].GetVector3();
	bool fixedSize = inSolveInstance[4].GetBool();

	//get texture if possible
	Texture2D* tex = rc->GetResource<Texture2D>(spritePath);

	//clone the material
	SharedPtr<Material> cloneMat = baseMat->Clone();
	cloneMat->SetTexture(TextureUnit::TU_DIFFUSE, tex);
	cloneMat->SetShaderParameter("MatDiffColor", mainColor);
	
	Node* node = scene->CreateChild(ID + "_Preview");
	BillboardSet* pointCloud = node->CreateComponent<BillboardSet>();
	pointCloud->SetNumBillboards(points.Size());
	pointCloud->SetMaterial(cloneMat);
	pointCloud->SetSorted(true);
	pointCloud->SetFixedScreenSize(fixedSize);

	//track
	trackedItems.Push(node->GetID());


	//add the points
	for (int i = 0; i < points.Size(); i++)
	{
		Billboard* bb = pointCloud->GetBillboard(i);

		bb->position_ = points[i].GetVector3();
		bb->size_ = Vector2(size.x_, size.y_);
		bb->enabled_ = true;
		bb->color_ = mainColor;
	}


	pointCloud->Commit();

	outSolveInstance[0] = node->GetID();
	outSolveInstance[1] = pointCloud;
}