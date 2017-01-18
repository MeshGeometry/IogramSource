#include "Scene_Display.h"
#include "ColorDefs.h"
#include "TriMesh.h"
#include "NMesh.h"
#include "Polyline.h"
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Camera.h>

#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

String Scene_Display::iconTexture = "Textures/Icons/Scene_Display.png";


Scene_Display::Scene_Display(Urho3D::Context* context) : IoComponentBase(context, 4, 2)
{
	SetName("Display");
	SetFullName("Geometry Display");
	SetDescription("Displays geometry in the scene");

	inputSlots_[0]->SetName("Geometry");
	inputSlots_[0]->SetVariableName("G");
	inputSlots_[0]->SetDescription("Geometry to render");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Color");
	inputSlots_[1]->SetVariableName("C");
	inputSlots_[1]->SetDescription("Color to Display");
	inputSlots_[1]->SetVariantType(VariantType::VAR_COLOR);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Color(0.9f, 0.9f, 0.9f, 1.0f));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Mode");
	inputSlots_[2]->SetVariableName("M");
	inputSlots_[2]->SetDescription("Display Mode. 0 - Fill, 1 - Wireframe, 2 - Points.");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(0);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("Flat");
	inputSlots_[3]->SetVariableName("F");
	inputSlots_[3]->SetDescription("Flat shaded");
	inputSlots_[3]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->SetDefaultValue(true);
	inputSlots_[3]->DefaultSet();

	outputSlots_[0]->SetName("Node");
	outputSlots_[0]->SetVariableName("ID");
	outputSlots_[0]->SetDescription("Node ID of display");
	outputSlots_[0]->SetVariantType(VariantType::VAR_INT); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Model");
	outputSlots_[1]->SetVariableName("M");
	outputSlots_[1]->SetDescription("Pointer to model");
	outputSlots_[1]->SetVariantType(VariantType::VAR_PTR); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);

}

void Scene_Display::PreLocalSolve()
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

	pointCloud = NULL;
}

void Scene_Display::SolveInstance(
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


	Color col = inSolveInstance[1].GetColor();
	bool flat = inSolveInstance[3].GetBool();

	//load the default material
	Material* mat = GetSubsystem<ResourceCache>()->GetResource<Material>(normalMat);
	if (col.a_ < 0.99f)
	{
		mat = GetSubsystem<ResourceCache>()->GetResource<Material>(normalAlphaMat);
	}

	//if material path has be passed, load that material
	if (inSolveInstance[1].GetType() == VAR_STRING)
	{
		String matName = inSolveInstance[1].GetString();
		mat = GetSubsystem<ResourceCache>()->GetResource<Material>(matName);
	}


	VariantVector vCols;

	//first check that input 0 is a model already
	SharedPtr<Model> mdl((Model*)inSolveInstance[0].GetVoidPtr());
	if (mdl)
	{

	} //create the model
	else if (TriMesh_Verify(inSolveInstance[0]))
	{
		mdl = TriMesh_GetRenderMesh(inSolveInstance[0], GetContext(), vCols, flat);

	}
	else if (NMesh_Verify(inSolveInstance[0]))
	{
		mdl = NMesh_GetRenderMesh(inSolveInstance[0], GetContext(), vCols, flat);
	}
	else if (Polyline_Verify(inSolveInstance[0]))
	{
		mdl = Polyline_GetRenderMesh(inSolveInstance[0], GetContext(), vCols, 0.01f);
		mat = GetSubsystem<ResourceCache>()->GetResource<Material>(normalMatWires);
	}
	else if (inSolveInstance[0].GetType() == VAR_VECTOR3)
	{
		//check if billboard set is null
		if (!pointCloud)
		{
			Node* node = scene->CreateChild(ID + "_Preview");
			pointCloud = node->CreateComponent<BillboardSet>();
			pointCloud->SetNumBillboards(1);
			ResourceCache* cache = GetSubsystem<ResourceCache>();
			Material* pMat = cache->GetResource<Material>(pointMat);
			pointCloud->SetMaterial(pMat);
			pointCloud->SetSorted(true);
			trackedItems.Push(node->GetID());

		}

		//otherwise continue
		int numPoints = pointCloud->GetNumBillboards();
		if (numPoints > 16000)
		{
			URHO3D_LOGWARNING("Large vertex count detected.");
		}

		pointCloud->SetNumBillboards(numPoints + 1);
		Billboard* bb = pointCloud->GetBillboard(numPoints);

		bb->position_ = inSolveInstance[0].GetVector3();
		bb->size_ = Vector2(10.0f, 10.0f);
		bb->enabled_ = true;
		bb->color_ = inSolveInstance[1].GetColor();
		pointCloud->SetFixedScreenSize(true);
		pointCloud->Commit();

		//TODO: totally not ideal, but works for now
		outSolveInstance[0] = pointCloud->GetNode()->GetID();
		outSolveInstance[1] = pointCloud;

		return;
	}

	//have model, now actually put it in the scene
	if (mdl && mat)
	{
		Node* node = scene->CreateChild(ID + "_Preview");
		StaticModel* sm = node->CreateComponent<StaticModel>();

		SharedPtr<Material> cMat = mat->Clone();
		col = inSolveInstance[1].GetColor();
		int mode = inSolveInstance[2].GetInt();
		mode = Clamp(mode, 0, 2);

		cMat->SetShaderParameter("MatDiffColor", col);
		FillMode fm = static_cast<FillMode>(mode);
		cMat->SetFillMode(fm);

		sm->SetModel(mdl);
		sm->SetMaterial(cMat);
		sm->SetCastShadows(true);
	
		trackedItems.Push(node->GetID());
		outSolveInstance[0] = node->GetID();
		outSolveInstance[1] = mdl;
	}
	else {
		SetAllOutputsNull(outSolveInstance);
		return;
	}
}