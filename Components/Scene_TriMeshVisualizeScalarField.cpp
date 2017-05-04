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


#include "Scene_TriMeshVisualizeScalarField.h"

#pragma warning(push, 0)
#include <igl/jet.h>
#pragma warning(pop)

#include <Eigen/Core>

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>

#include "ColorDefs.h"
#include "TriMesh.h"
#include "Polyline.h"
#include "ConversionUtilities.h"

using namespace Urho3D;

String Scene_TriMeshVisualizeScalarField::iconTexture = "";

Scene_TriMeshVisualizeScalarField::Scene_TriMeshVisualizeScalarField(Urho3D::Context* context) : IoComponentBase(context, 2, 2)
{
	SetName("TriMeshVisualizeScalarField");
	SetFullName("TriMesh Visualize Scalar Field");
	SetDescription("Visualize scalar field on a TriMesh");

	inputSlots_[0]->SetName("TriMesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("TriMesh input");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("ScalarField");
	inputSlots_[1]->SetVariableName("SF");
	inputSlots_[1]->SetDescription("Scalar field on the input TriMesh");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::LIST);
	inputSlots_[1]->SetDefaultValue(0.0f);
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Node");
	outputSlots_[0]->SetVariableName("ID");
	outputSlots_[0]->SetDescription("Node ID of displayed TriMesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_INT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Model");
	outputSlots_[1]->SetVariableName("M");
	outputSlots_[1]->SetDescription("Pointer to model");
	outputSlots_[1]->SetVariantType(VariantType::VAR_PTR);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}

void Scene_TriMeshVisualizeScalarField::PreLocalSolve()
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (scene) {
		for (int i = 0; i < tracked_items.Size(); ++i) {
			Node* n = scene->GetNode(tracked_items[i]);
			if (n) {
				n->RemoveAllComponents();
				n->Remove();
			}
		}
	}
	tracked_items.Clear();
}

void Scene_TriMeshVisualizeScalarField::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (!scene) {
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	Material* mat = GetSubsystem<ResourceCache>()->GetResource<Material>(normalMat);

	Variant tri_mesh = inSolveInstance[0];
	if (!TriMesh_Verify(tri_mesh)) {
		URHO3D_LOGWARNING("TriMeshVisualizeScalarField --- M must be a valid TriMesh");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	if (inSolveInstance[1].GetType() != VariantType::VAR_VARIANTVECTOR) {
		URHO3D_LOGWARNING("TriMeshVisualizeScalarField --- could not read scalar field values");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	VariantVector scalar_field_in = inSolveInstance[1].GetVariantVector();
	if (scalar_field_in.Size() == 0) {
		URHO3D_LOGWARNING("TriMeshVisualizeScalarField --- no scalar field given");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	VariantVector vertex_list = TriMesh_GetVertexList(tri_mesh);
	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	IglMeshToMatrices(tri_mesh, V, F);
	Eigen::VectorXf Z(V.rows());
	for (int i = 0; i < Z.rows(); ++i) {
		int index = i % scalar_field_in.Size();
		if (scalar_field_in[i].GetType() == VariantType::VAR_FLOAT) {
			Z(i) = scalar_field_in[i].GetFloat();
		}
		else {
			Z(i) = 0.0f;
		}
	}
	Eigen::MatrixXd C;
	igl::jet(Z, true, C);
	VariantVector vertex_colors;
	for (int i = 0; i < C.rows(); ++i) {
		vertex_colors.Push(Urho3D::Color(C(i, 0), C(i, 1), C(i, 2)));
	}
	assert(vertex_colors.Size() == vertex_list.Size());

	Model* model = TriMesh_GetRenderMesh(tri_mesh, GetContext(), vertex_colors, true);

	if (model && mat) {
		Node* node = scene->CreateChild(ID + "_Preview");
		StaticModel* sm = node->CreateComponent<StaticModel>();

		SharedPtr<Material> cMat = mat->Clone();
		//Urho3D::Color col = Urho3D::Color(0.9f, 0.9f, 0.9f, 1.0f);
		int mode = 0;
		mode = Clamp(mode, 0, 2);

		//cMat->SetShaderParameter("MatDiffColor", col);
		FillMode fm = static_cast<FillMode>(mode);
		cMat->SetFillMode(fm);

		sm->SetModel(model);
		sm->SetMaterial(cMat);
		sm->SetCastShadows(true);

		tracked_items.Push(node->GetID());
		outSolveInstance[0] = node->GetID();
		outSolveInstance[1] = model;
	}
	else {
		URHO3D_LOGWARNING("TriMeshVisualizeScalarField --- failed to create either model or material");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
}