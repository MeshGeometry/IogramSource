#include "Graphics_BaseSettings.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/TextureCube.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Material.h>

#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

String Graphics_BaseSettings::iconTexture = "Textures/Icons/Graphics_BaseSettings.png";


Graphics_BaseSettings::Graphics_BaseSettings(Urho3D::Context* context) : IoComponentBase(context, 6, 2)
{
	SetName("RenderSettings");
	SetFullName("Base Render Settings");
	SetDescription("Sets a few of the most important render settings.");

	inputSlots_[0]->SetName("Skybox");
	inputSlots_[0]->SetVariableName("SB");
	inputSlots_[0]->SetDescription("Set the skybox material");
	inputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[0]->SetDefaultValue("Materials/Skybox.xml");
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("ZoneTexture");
	inputSlots_[1]->SetVariableName("ZT");
	inputSlots_[1]->SetDescription("Zone Texture (for reflections)");
	inputSlots_[1]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[1]->SetDefaultValue("Textures/ZoneCubeMap.xml");
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Zone Size");
	inputSlots_[2]->SetVariableName("ZS");
	inputSlots_[2]->SetDescription("Vector defined size of the render zone");
	inputSlots_[2]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[2]->SetDefaultValue(Vector3(-1000.0f, 1000.0f, 0.0f));
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("AmbientLight");
	inputSlots_[3]->SetVariableName("AL");
	inputSlots_[3]->SetDescription("Ambient Light. Alpha channel determines brightness.");
	inputSlots_[3]->SetVariantType(VariantType::VAR_COLOR);
	inputSlots_[3]->SetDefaultValue(Color(0.1f, 0.1f, 0.1f, 1.0f));
	inputSlots_[3]->DefaultSet();

	inputSlots_[4]->SetName("Fog Color");
	inputSlots_[4]->SetVariableName("FC");
	inputSlots_[4]->SetDescription("Color of Fog.");
	inputSlots_[4]->SetVariantType(VariantType::VAR_COLOR);	
	inputSlots_[4]->SetDefaultValue(Color(0.5f, 0.5f, 0.5f, 1.0f));
	inputSlots_[4]->DefaultSet();

	inputSlots_[5]->SetName("Fog extents");
	inputSlots_[5]->SetVariableName("FE");
	inputSlots_[5]->SetDescription("Fog extents. Vector 3 where x,y are start and end distances. Z is density.");
	inputSlots_[5]->SetVariantType(VariantType::VAR_VECTOR3);		
	inputSlots_[5]->SetDefaultValue(Vector3(300.0f, 600.0f, 0.0f));
	inputSlots_[5]->DefaultSet();

	outputSlots_[0]->SetName("Zone");
	outputSlots_[0]->SetVariableName("Z");
	outputSlots_[0]->SetDescription("Pointer to zone");
	outputSlots_[0]->SetVariantType(VariantType::VAR_PTR);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Skybox");
	outputSlots_[1]->SetVariableName("S");
	outputSlots_[1]->SetDescription("Pointer to skybox");
	outputSlots_[1]->SetVariantType(VariantType::VAR_PTR);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}

void Graphics_BaseSettings::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	//first, get the scene
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (!scene)
	{
		URHO3D_LOGERROR("Could not get scene");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	Node* zoneNode = scene->GetChild("Zone");
	Node* skyNode = scene->GetChild("Sky");
	if (zoneNode == NULL || skyNode == NULL)
	{
		URHO3D_LOGERROR("Could not find sky or zone nodes...");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	Zone* zone = zoneNode->GetComponent<Zone>();
	Skybox* skybox = skyNode->GetComponent<Skybox>();

	if (zone == NULL || skybox == NULL)
	{
		URHO3D_LOGERROR("Could not find sky or zone components...");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//finally set the params
	ResourceCache* rc = GetSubsystem<ResourceCache>();

	//skybox mat
	Material* skyMat;
	if (inSolveInstance[0].GetType() == VAR_STRING)
	{
		String matPath = inSolveInstance[0].GetString();
		skyMat = rc->GetResource<Material>(matPath);
	}
	else if (inSolveInstance[0].GetType() == VAR_PTR)
	{
		skyMat = (Material*)inSolveInstance[0].GetPtr();
	}

	if (skyMat)
	{
		skybox->SetMaterial(skyMat);
	}

	//zone texture
	Texture* zTex;
	if (inSolveInstance[1].GetType() == VAR_STRING)
	{
		String texPath = inSolveInstance[1].GetString();
		zTex = rc->GetResource<TextureCube>(texPath);
	}
	else if (inSolveInstance[1].GetType() == VAR_PTR)
	{
		zTex = (TextureCube*)inSolveInstance[1].GetPtr();
	}

	if (zTex)
	{
		zone->SetZoneTexture(zTex);
	}

	//zone size
	Vector3 zSize = inSolveInstance[2].GetVector3();
	zone->SetBoundingBox(BoundingBox(zSize.x_, zSize.y_));

	//ambient light
	Color aCol = inSolveInstance[3].GetColor();
	zone->SetAmbientColor(aCol);
	
	//set fog col
	Color fCol = inSolveInstance[4].GetColor();
	zone->SetFogColor(fCol);

	//set fog extents
	Vector3 fExt = inSolveInstance[5].GetVector3();
	zone->SetFogStart(fExt.x_);
	zone->SetFogEnd(fExt.y_);

	outSolveInstance[0] = zone;
	outSolveInstance[1] = skybox;

}