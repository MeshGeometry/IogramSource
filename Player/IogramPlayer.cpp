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


#include "IogramPlayer.h"

#include "OrbitCamera.h"
#include "IoGraph.h"
#include "IoSerialization.h"
#include "ComponentRegistration.h"
#include "RegisterCoreComponents.h"
#include "PersistentData.h"
#include "PluginAPI.h"

#include <Urho3D/ThirdParty/SDL/SDL.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/Graphics/GraphicsImpl.h>
#include <Urho3D/IO/PackageFile.h>
#include <Urho3D/AngelScript/Script.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/UI/Cursor.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/TextureCube.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include "Urho3D/UI/Text.h"
#include <Urho3D/UI/ScrollView.h>
#include <Urho3D/UI/Slider.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/View3D.h>
#include <Urho3D/UI/UIEvents.h>

#include "IoScriptInstance.h"
#include "IoGeometryAPI.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <iostream>

using namespace Urho3D;

IogramPlayer* IogramPlayer::instance_;

URHO3D_DEFINE_APPLICATION_MAIN(IogramPlayer);

IoGraph* graph;

IogramPlayer::IogramPlayer(Context* context) :
	Application(context)
{
	context->RegisterFactory<OrbitCamera>();
	context->RegisterSubsystem(new Script(context));
	context->RegisterFactory<IoScriptInstance>();

	IoScriptInstance::RegisterScriptObject(context);


	//register iogram systems
	context->RegisterSubsystem(new IoGraph(context));
	context->RegisterSubsystem(new PluginAPI(context));
	context->RegisterSubsystem(new PersistentData(context));
	//context->RegisterSubsystem(new Log(context));
	instance_ = this;
}

void IogramPlayer::Setup()
{
	int width = 1200;
	int height = 800;
	int fullscreen = 0;

#ifdef __EMSCRIPTEN__
	emscripten_get_canvas_size(&width, &height, &fullscreen);
	std::cout << "Canvas width: " << width << std::endl;
	std::cout << "Canvas height: " << height << std::endl;
#endif

	width = Clamp(width, 600, 4096);
	height = Clamp(height, 400, 4096);


	// Modify engine startup parameters
	engineParameters_["WindowTitle"] = GetTypeName();
	engineParameters_["LogName"] = GetSubsystem<FileSystem>()->GetProgramDir() + GetTypeName() + ".log";
	engineParameters_["FullScreen"] = false;
	engineParameters_["Headless"] = false;
	engineParameters_["WindowHeight"] = height;
	engineParameters_["WindowWidth"] = width;
	engineParameters_["WindowResizable"] = true;
	engineParameters_["HighDPI"] = true;
	engineParameters_[EP_RESOURCE_PREFIX_PATHS] = ";./CoreData;./Data";
	Urho3D::OpenConsoleWindow();
}

void IogramPlayer::Start()
{
	//set up the log
	//GetSubsystem<Log>()->SetLevel(1);

	//init the pref dir
#ifndef WEB
	PersistentData* pd = GetSubsystem<PersistentData>();
	pd->RegisterAppData("MyOrganization", "MyFirstApp");
#endif

	//register core components
	RegisterCoreComponents(context_);

	//register geometry api
	RegisterGeometryFunctions(context_);

	//load plugins
#ifndef WEB
	LoadPlugins();
#endif

	GetSubsystem<Input>()->SetMouseVisible(true);

	CreateScene();

	SetupViewport();

	//push scene to script
	GetSubsystem<Script>()->SetDefaultScene(scene_);

	LoadGraph();

	//subscribe
	SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(IogramPlayer, HandleUpdate));
}


void IogramPlayer::Stop()
{
}

void IogramPlayer::LoadGraph()
{
	//scan the graphs directory and load the first one
	FileSystem* fs = GetSubsystem<FileSystem>();
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	String graphDir = fs->GetProgramDir() + "/Data/Graphs";

	bool init = false;

	//first check command like arguments
	Vector<String> args = GetArguments();
	if (args.Size() > 0)
	{
		graphDir = args[0];
	}

	if (fs->DirExists(graphDir))
	{
		URHO3D_LOGINFO("IogramPlayer::Start --- loading main.graph from " + graphDir);

		//load the first graph we find
		IoGraph* graph = GetSubsystem<IoGraph>();
		IoSerialization::LoadGraph(*graph, graphDir + "/main.graph");
		graph->scene = scene_;
		//solve it
		graph->TopoSolveGraph();

		init = true;
	}

	if (!init)
	{
		auto pFiles = cache->GetPackageFiles();
		for (int i = 0; i < pFiles.Size(); i++)
		{
			URHO3D_LOGINFO("package file: " + pFiles[i]->GetName());
			if (pFiles[i]->GetName().Contains("Data.pak"))
			{
				//get the entires
				StringVector names = pFiles[i]->GetEntryNames();

				for (int j = 0; j < names.Size(); j++)
				{
					if (names[j].Contains("main.graph"))
					{
						URHO3D_LOGINFO("IogramPlayer::Start --- found main.graph in Data.pak");

						//load the first graph we find
						File* graphFile = new File(context_, pFiles[i], names[j]);
						IoGraph* graph = GetSubsystem<IoGraph>();
						IoSerialization::LoadGraph(*graph, graphFile);
						graph->scene = scene_;

						////solve it
						graph->TopoSolveGraph();

						init = true;
						goto hasfile;
					}
				}
			}
		}
	}

hasfile:


	if (!init)
	{
		URHO3D_LOGERROR("Could not find a graph file to run.");
	}
}

void IogramPlayer::LoadPlugins()
{
	//FileSystem* fs = GetSubsystem<FileSystem>();
	//PluginAPI* pl = GetSubsystem<PluginAPI>();
	//String configPath = fs->GetProgramDir() + "Plugins/PluginConfiguration.json";
	//pl->LoadPluginsFromFile(configPath);

}


void IogramPlayer::CreateScene()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	scene_ = new Scene(context_);
	SetGlobalVar("Scene", scene_);


	// Create the Octree component to the scene so that drawable objects can be rendered. Use default volume
	// (-1000, -1000, -1000) to (1000, 1000, 1000)
	scene_->CreateComponent<Octree>();

	// Create a Zone for ambient light & fog control
	Node* zoneNode = scene_->CreateChild("Zone");
	Zone* zone = zoneNode->CreateComponent<Zone>();
	zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
	zone->SetFogStart(300.0f);
	zone->SetFogEnd(600.0f);
	zone->SetAmbientColor(Color(0.3, 0.3, 0.3, 0.9));
	TextureCube* cubeTex = cache->GetResource<TextureCube>("Textures/GreySkyGradient.xml");
	zone->SetZoneTexture(cubeTex);

	//create the background
	Skybox* skybox = scene_->CreateComponent<Skybox>();
	skybox->SetModel(cache->GetResource<Model>("Models/Sphere.mdl"));
	Material* skyMat = cache->GetResource<Material>("Materials/GradientSkydome.xml");
	skybox->SetMaterial(skyMat);

	// Create the camera
	cameraNode_ = scene_->CreateChild("OrbitCamera");
	cameraNode_->CreateComponent<OrbitCamera>();


	//GetSubsystem<Script>()->SetDefaultScene(scene_);

	//calculate the best fit ui scale
	SetUIScale();

	//initialize the ui style sheet
	UI* ui = GetSubsystem<UI>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/IogramDefaultStyle.xml");

	ui->GetRoot()->SetDefaultStyle(style);
	ui->SetUseSystemClipboard(true);
	ui->SetUseScreenKeyboard(true);

	//register root as active ui region
	SetGlobalVar("activeUIRegion", ui->GetRoot());

	//Graphics* g = GetSubsystem<Graphics>();
	Graphics* g = GetSubsystem<Graphics>();
	g->Maximize();



}

void IogramPlayer::SetUIScale()
{
	float a, b, c;
	SDL_GetDisplayDPI(0, &a, &b, &c);;
	//URHO3D_LOGINFO("diagonal dots per inch (ddpi) = " + String(a));

	//get current window size
	Graphics* g = GetSubsystem<Graphics>();

	// check to see if we should scale based on windowsize : drawable size ratio
	// only relevant for OSX Retina displays?
	SDL_Window* curwindow = g->GetWindow();
	int gl_w, gl_h;
	SDL_GL_GetDrawableSize(curwindow, &gl_w, &gl_h);

	int sdl_w, sdl_h;
	SDL_GetWindowSize(curwindow, &sdl_w, &sdl_h);

	float multiplier = 1.0f;
	if (sdl_w != 0)
		multiplier = gl_w / sdl_w;

	int realDPI = multiplier * a;
	float scale = Max(0.33f * (realDPI / 48.0f), 1.0f);

	UI* ui = GetSubsystem<UI>();
	float curScale = ui->GetScale();
	if (Abs(scale - curScale) > 0.001f)
		ui->SetScale(scale);
}


void IogramPlayer::SetupViewport()
{
	Renderer* renderer = GetSubsystem<Renderer>();
	renderer->SetDrawShadows(true);
	renderer->SetShadowQuality(SHADOWQUALITY_PCF_24BIT);
	renderer->SetMaterialQuality(QUALITY_HIGH);
	renderer->SetHDRRendering(true);

	// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
	viewport_ = new Viewport(context_, scene_, cameraNode_->GetComponent<OrbitCamera>()->camera_);
	
	SetGlobalVar("activeViewport", viewport_);
	VariantVector vpList;
	vpList.Push(viewport_);
	SetGlobalVar("ViewportVector", vpList);
	renderer->SetViewport(0, viewport_);

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	RenderPath* render_path = new RenderPath();

	render_path->Load(cache->GetResource<XMLFile>("RenderPaths/ForwardDepth.xml"));
	render_path->Append(cache->GetResource<XMLFile>("PostProcess/Vibrance.xml"));
	viewport_->SetRenderPath(render_path);

	Graphics* graphics = GetSubsystem<Graphics>();
	graphics->SetWindowTitle("Iogram: The Pre-Game Engine");
	Image* icon = cache->GetResource<Image>("Textures/Io_Icon_Small.png");
	graphics->SetWindowIcon(icon);

	/***********************************************************

	HERE BE DRAGONS:

	When accessing the scene via script, somehow "ownership" of the scene got transferred to script system (I think).
	This then caused the Player to render only a black screen (presumably because the Viewport pointer to the scene was NULL).

	In the Editor, this didn't happen. The difference between the Editor and the Player is (wrt this problem) is that Editor
	uses View3D to render to scene. So, I checked out that code and found the following code that somehow prevents this issue.

	It works, but I don't really know why.

	************************************************************/

	RefCount* refCount = scene_->RefCountPtr();
	++refCount->refs_;

}


void IogramPlayer::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	//adjust ui scale if necessary
	//SetUIScale();

	//adjust window size to canvas size on web
#ifdef __EMSCRIPTEN__

	// Graphics* g = GetSubsystem<Graphics>();
	// int width, height, fullscreen = 0;
	// int currWidth, currHeight;
	// emscripten_get_canvas_size(&width, &height, &fullscreen);
 //    //emscripten_set_canvas_size(1600, 1200);

	// currWidth = g->GetWidth();
	// currHeight = g->GetHeight();

	// if (width != currWidth || height != currHeight) {
	// 	g->SetMode(width, height);
	// }

#endif

	Input* input = GetSubsystem<Input>();
	if (input->GetKeyDown(KEY_ALT) && input->GetKeyPress(KEY_G))
	{
		GetSubsystem<IoGraph>()->TopoSolveGraph();
	}
}


