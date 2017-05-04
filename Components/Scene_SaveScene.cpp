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


#include "Scene_SaveScene.h"

#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

String Scene_SaveScene::iconTexture = "Textures/Icons/Scene_SaveScene.png";

Scene_SaveScene::Scene_SaveScene(Urho3D::Context* context) :IoComponentBase(context, 0, 0)
{
	SetName("SaveScene");
	SetFullName("Save Scene");
	SetDescription("Saves current Scene resource file from XML or JSON");

	AddInputSlot(
		"ScenePath",
		"P",
		"Resource Path to scene file",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddInputSlot(
		"UseJson",
		"J",
		"If true, will write to JSON format. Otherwise XML.",
		VAR_BOOL,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Success",
		"R",
		"Returns true if file was written",
		VAR_BOOL,
		DataAccess::ITEM
	);

}

void Scene_SaveScene::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	String path = inSolveInstance[0].GetString();
	bool useJson = inSolveInstance[1].GetBool();

	if (path.Empty())
	{
		outSolveInstance[0] = Variant();
		return;
	}

	ResourceCache* c = GetSubsystem<ResourceCache>();
	Vector<String> dirs = c->GetResourceDirs();
	String prefDir = dirs[0];

	File* sceneFile = new File(GetContext());
	sceneFile->Open(prefDir + path, FILE_WRITE);

	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (!scene)
	{
		return;
	}

	if(!useJson)
		scene->SaveXML(*sceneFile);
	else
		scene->SaveJSON(*sceneFile);

	
	
	outSolveInstance[0] = true;


	sceneFile->Close();

	//check that resource exists


}

