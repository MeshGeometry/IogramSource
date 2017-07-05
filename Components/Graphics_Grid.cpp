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

#include "Graphics_Grid.h"

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/TextureCube.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/Technique.h>

#include "Geomlib_ConstructTransform.h"


using namespace Urho3D;

String Graphics_Grid::iconTexture = "Textures/Icons/Graphics_Grid.png";

Graphics_Grid::Graphics_Grid(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("Sun");
	SetFullName("Sun");
	SetDescription("Create a directional Sun light");

	AddInputSlot(
		"Transform",
		"T",
		"Transform",
		VAR_MATRIX3X4,
		ITEM,
		Matrix3x4::IDENTITY
	);

	AddInputSlot(
		"Scale",
		"S",
		"Scale",
		VAR_VECTOR3,
		ITEM,
		Vector3(500, 1, 500)
	);

	AddInputSlot(
		"Density",
		"D",
		"Density",
		VAR_FLOAT,
		ITEM,
		1.0f
	);

	AddInputSlot(
		"Color",
		"C",
		"Color",
		VAR_COLOR,
		ITEM,
		0.5f * Color::WHITE
	);


	AddOutputSlot(
		"NodeID",
		"ID",
		"Node ID",
		VAR_INT,
		ITEM
	);

	AddOutputSlot(
		"Reference",
		"R",
		"Reference",
		VAR_PTR,
		ITEM
	);

	AddOutputSlot(
		"ModelName",
		"ModelName",
		"ModelName",
		VAR_STRING,
		ITEM
	);

}

void Graphics_Grid::PreLocalSolve()
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

void Graphics_Grid::SolveInstance(
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
	Vector3 scale = inSolveInstance[1].GetVector3();
	float density = inSolveInstance[2].GetFloat();
	Color col = inSolveInstance[3].GetColor();


	//create the grid
	Node* node = scene->CreateChild("__GRID__");
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	Material* mat = rc->GetResource<Material>("Materials/BasicGrid.xml");
	Model* mdl = rc->GetResource<Model>("Models/Plane.mdl");
	StaticModel* sm = node->CreateComponent<StaticModel>();

	if (mdl && mat) {

		mat->SetShaderParameter("MatDiffColor", col);
		mat->SetShaderParameter("GridScale", scale.x_);
		sm->SetModel(mdl);
		sm->SetMaterial(mat);
		sm->SetCastShadows(false);

	}



	//apply transform	
	node->SetPosition(xform.Translation());
	node->SetRotation(xform.Rotation());
	node->SetScale(scale);


	trackedItems_.Push(node->GetID());

	outSolveInstance[0] = node->GetID();
	outSolveInstance[1] = sm;
	outSolveInstance[2] = mdl->GetName();

}
