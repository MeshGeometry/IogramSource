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


#include "Spatial_Sun.h"

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
#include "Geomlib_SunPosition.h"

using namespace Urho3D;

String Spatial_Sun::iconTexture = "Textures/Icons/Spatial_Sun.png";

Spatial_Sun::Spatial_Sun(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("Sun");
	SetFullName("Sun");
	SetDescription("Create a directional Sun light");

	AddInputSlot(
		"Latitude",
		"LA",
		"Latitude",
		VAR_FLOAT,
		ITEM,
		43.6525f
	);

	AddInputSlot(
		"Longitude",
		"LO",
		"Longitude",
		VAR_FLOAT,
		ITEM,
		-79.381667f
	);

	AddInputSlot(
		"Month",
		"M",
		"Month",
		VAR_INT,
		ITEM,
		6
	);

	AddInputSlot(
		"Day",
		"D",
		"Day",
		VAR_INT,
		ITEM,
		15
	);

	AddInputSlot(
		"Hour",
		"H",
		"Hour",
		VAR_FLOAT,
		ITEM,
		11.5f
	);

	AddInputSlot(
		"Color",
		"C",
		"Color",
		VAR_COLOR,
		ITEM,
		Color::GRAY
	);

	AddOutputSlot(
		"NodeID",
		"ID",
		"Node ID",
		VAR_INT,
		ITEM
	);

	AddOutputSlot(
		"Sun",
		"S",
		"SunPtr",
		VAR_VOIDPTR,
		ITEM
	);

}

void Spatial_Sun::PreLocalSolve()
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

void Spatial_Sun::SolveInstance(
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

	//turn off other lights??

	//get all the data
	float lat = inSolveInstance[0].GetFloat();
	float lon = inSolveInstance[1].GetFloat();
	float year = 2017;
	float month = inSolveInstance[2].GetFloat();
	float day = inSolveInstance[3].GetFloat();
	float hour = inSolveInstance[4].GetFloat();
	int tzone = -4;
	Color col = inSolveInstance[5].GetColor();

	//clamp ranges
	lat = Clamp(lat, -180.f, 180.f);
	lon = Clamp(lon, -180.f, 180.f);
	month = Clamp(month, 0.f, 11.f);
	day = Clamp(day, 0.f, 30.f);
	hour = Clamp(hour, 0.f, 24.f);

	//calc sun pos
	Vector3 sunPos = Geomlib::ComputeSunPosition(
		year,
		month,
		day,
		hour,
		tzone,
		lat,
		lon
	);

	//get or create the sun node
	//NOTE: this creates a singleton like behaviour. Not sure if desired.
	//Node* node = scene->GetChild("SunLight", true);
	//if (!node)
	//{
	Node* node = scene->CreateChild("SunLight");
	node->SetWorldPosition(Vector3(0, 100, 0));
	//}

	//get or create the light component
	Light* light = node->GetComponent <Light>();
	if (!light)
	{
		Light* light = node->CreateComponent<Light>();
		light->SetColor(col);
		light->SetBrightness(col.a_);
		light->SetLightType(LIGHT_DIRECTIONAL);
		light->SetCastShadows(true);
		light->SetRange(1000.0f);
		light->SetShadowIntensity(0.1f);
	}


	//convert to sun direction
	//TODO: this needs some verification. Currently just using spherical coords
	double theta = sunPos.x_;
	double phi = 90 - sunPos.y_;
	double x = Cos(theta)*Sin(phi);
	double z = Sin(theta)*Sin(phi);
	double y = Cos(phi);
	Vector3 sun_direction_(-x, -y, -z);

	node->SetWorldDirection(sun_direction_);
	trackedItems_.Push(node->GetID());

	outSolveInstance[0] = node->GetID();
	outSolveInstance[1] = light;



}
