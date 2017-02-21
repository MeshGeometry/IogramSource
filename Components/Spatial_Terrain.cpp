#include "Spatial_Terrain.h"

#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/Image.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include "IoGraph.h"
#include "Geomlib_ConstructTransform.h"

using namespace Urho3D;

String Spatial_Terrain::iconTexture = "Textures/Icons/Spatial_Terrain.png";

Spatial_Terrain::Spatial_Terrain(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("Terrain");
	SetFullName("Terrain Object");
	SetDescription("Terrain Object");

	AddInputSlot(
		"ImageFile",
		"I",
		"Image File",
		VAR_STRING,
		ITEM
	);

	AddInputSlot(
		"Transform",
		"T",
		"Transform",
		VAR_MATRIX3X4,
		ITEM,
		Matrix3x4::IDENTITY
	);

	AddOutputSlot(
		"NodeID",
		"ID",
		"Node ID",
		VAR_INT,
		ITEM
	);

	AddOutputSlot(
		"Terrain",
		"T",
		"Terrain",
		VAR_VOIDPTR,
		ITEM
	);

}

void Spatial_Terrain::PreLocalSolve()
{
	//delete old nodes
	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();
	if (scene)
	{
		for (int i = 0; i < trackedItems.Size(); i++)
		{
			Node* oldNode = scene->GetNode(trackedItems[i]);
			if (oldNode)
			{
				oldNode->Remove();
			}

		}

		trackedItems.Clear();
	}
}

void Spatial_Terrain::SolveInstance(
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

	// construct transform:
	Matrix3x4 xform = Geomlib::ConstructTransform(inSolveInstance[0]);

	Node* tNode = scene->CreateChild("TerrainNode");
	tNode->SetTransform(xform);

	Terrain* terrain = tNode->CreateComponent<Terrain>();
	terrain->SetPatchSize(64);
	terrain->SetSpacing(Vector3(2.0f, 0.1f, 2.0f)); // Spacing between vertices and vertical resolution of the height map
	terrain->SetSmoothing(true);
	terrain->SetHeightMap(cache->GetResource<Image>("Textures/HeightMap.png"));
	terrain->SetMaterial(cache->GetResource<Material>("Materials/Terrain.xml"));
	terrain->SetOccluder(true);

	trackedItems.Push(tNode->GetID());
	
	outSolveInstance[0] = tNode->GetID();
	outSolveInstance[1] = terrain;
}
