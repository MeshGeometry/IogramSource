#include "Spatial_Terrain.h"

#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/Image.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Graphics/Texture2D.h>
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
		ITEM,
		"Textures/HeightMap.png"
	);

	AddInputSlot(
		"Material",
		"M",
		"Material Path",
		VAR_STRING,
		ITEM,
		"Materials/BasicWeb.xml"
		);

	AddInputSlot(
		"Transform",
		"T",
		"Transform",
		VAR_MATRIX3X4,
		ITEM,
		Matrix3x4::IDENTITY
	);

	AddInputSlot(
		"Spacing",
		"S",
		"Grid Spacing",
		VAR_VECTOR3,
		ITEM,
		Vector3(1, 0.2, 1.0)
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
	Matrix3x4 xform = Geomlib::ConstructTransform(inSolveInstance[2]);
	String imagePath = inSolveInstance[0].GetString();
	String matPath = inSolveInstance[1].GetString();
	Vector3 spacing = inSolveInstance[3].GetVector3();

	Node* tNode = scene->CreateChild("TerrainNode");

	SharedPtr<Image> terrainImage(cache->GetResource<Image>(imagePath));
	bool exists = cache->Exists(imagePath);

	if (!terrainImage)
	{
		Texture2D* tex = cache->GetResource<Texture2D>(imagePath);
		if (!tex)
		{
			SetAllOutputsNull(outSolveInstance);
			return;
		}

		terrainImage = tex->GetImage();

	}


	Terrain* terrain = tNode->CreateComponent<Terrain>();
	terrain->SetPatchSize(64);
	terrain->SetSpacing(spacing); // Spacing between vertices and vertical resolution of the height map
	terrain->SetSmoothing(false);
	terrain->SetHeightMap(terrainImage);
	terrain->SetMaterial(cache->GetResource<Material>(matPath));
	terrain->SetOccluder(true);

	tNode->SetTransform(xform);

	trackedItems.Push(tNode->GetID());
	
	outSolveInstance[0] = tNode->GetID();
	outSolveInstance[1] = terrain;
}
