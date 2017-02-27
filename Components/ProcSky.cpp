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


/**
  @class ProcSky
  @brief Procedural Sky component for Urho3D
  @author carnalis <carnalis.j@gmail.com>
  @license MIT License
  @copyright
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "ProcSky.h"
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/GraphicsDefs.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/Technique.h>
#include <Urho3D/Graphics/Texture.h>
#include <Urho3D/Graphics/TextureCube.h>
#include <Urho3D/Graphics/View.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Math/Matrix4.h>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Math/Vector4.h>
#include <Urho3D/Resource/ResourceCache.h>
#if defined(PROCSKY_UI)
#include <Urho3D/Resource/XMLFile.h>
#endif
#include <Urho3D/IO/Log.h>
#include <Urho3D/Scene/Scene.h>

#if defined(PROCSKY_UI)
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Slider.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#endif

#if defined(PROCSKY_TEXTURE_DUMPING)
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/Image.h>
#endif

using namespace Urho3D;

ProcSky::ProcSky(Context* context):
  Component(context)
  , renderSize_(256)
  , renderFOV_(89.5)
  , updateAuto_(true)
  , updateInterval_(0.0f)
  , updateWait_(0)
  , renderQueued_(true)
  , cam_(NULL)
  , Kr_(Vector3(0.18867780436772762, 0.4978442963618773, 0.6616065586417131))
  , rayleighBrightness_(3.3f)
  , mieBrightness_(0.1f)
  , spotBrightness_(50.0f)
  , scatterStrength_(0.028f)
  , rayleighStrength_(0.139f)
  , mieStrength_(0.264f)
  , rayleighCollectionPower_(0.81f)
  , mieCollectionPower_(0.39f)
  , mieDistribution_(0.63f)
{
  faceRotations_[FACE_POSITIVE_X] = Matrix3(0,0,1,  0,1,0, -1,0,0);
  faceRotations_[FACE_NEGATIVE_X] = Matrix3(0,0,-1, 0,1,0,  1,0,0);
  faceRotations_[FACE_POSITIVE_Y] = Matrix3(1,0,0,  0,0,1,  0,-1,0);
  faceRotations_[FACE_NEGATIVE_Y] = Matrix3(1,0,0,  0,0,-1, 0,1,0);
  faceRotations_[FACE_POSITIVE_Z] = Matrix3(1,0,0,  0,1,0,  0,0,1);
  faceRotations_[FACE_NEGATIVE_Z] = Matrix3(-1,0,0, 0,1,0,  0,0,-1);
}
ProcSky::~ProcSky() {}

void ProcSky::RegisterObject(Context* context) {
  context->RegisterFactory<ProcSky>();
}

void ProcSky::OnNodeSet(Node* node) {
  if (!node) return;
}

bool ProcSky::Initialize() {
  URHO3D_LOGDEBUG("ProcSky::Initialize()");
  ResourceCache* cache(GetSubsystem<ResourceCache>());
  Renderer* renderer(GetSubsystem<Renderer>());
  rPath_ = renderer->GetViewport(0)->GetRenderPath();

  // If Camera has not been set, create one in this node.
  if (!cam_) {
    cam_ = node_->CreateComponent<Camera>();
    cam_->SetFov(renderFOV_);
    cam_->SetAspectRatio(1.0f);
    cam_->SetNearClip(1.0f);
    cam_->SetFarClip(100.0f);
  }

  // Use first child as light node if it exists; otherwise, create it.
  if (!lightNode_) {
    const Vector<SharedPtr<Node> >& children = node_->GetChildren();
    if (children.Size()) {
      lightNode_ = children[0];
    }
    if (lightNode_) {
      URHO3D_LOGDEBUG("ProcSky::Initialize: Creating node 'ProcSkyLight' with directional light.");
      lightNode_ = node_->CreateChild("ProcSkyLight");
      Light* light(lightNode_->CreateComponent<Light>());
      light->SetLightType(LIGHT_DIRECTIONAL);
      Color lightColor;
      lightColor.FromHSV(57.0f, 9.9f, 75.3f);
      light->SetColor(lightColor);
    }
  }

  // Create a Skybox to draw to. Set its Material, Technique, and render size.
  skybox_ = node_->CreateComponent<Skybox>();
  Model* model(cache->GetResource<Model>("Models/Box.mdl"));
  skybox_->SetModel(model);
  SharedPtr<Material> skyboxMat(new Material(context_));
  skyboxMat->SetTechnique(0, cache->GetResource<Technique>("Techniques/DiffProcSkybox.xml"));
  skyboxMat->SetCullMode(CULL_NONE);
  skybox_->SetMaterial(skyboxMat);
  SetRenderSize(renderSize_);

  // Shove some of the shader parameters into a VariantMap.
  VariantMap atmoParams;
  atmoParams["Kr"] = Kr_;
  atmoParams["RayleighBrightness"] = rayleighBrightness_;
  atmoParams["MieBrightness"] = mieBrightness_;
  atmoParams["SpotBrightness"] = spotBrightness_;
  atmoParams["ScatterStrength"] = scatterStrength_;
  atmoParams["RayleighStrength"] = rayleighStrength_;
  atmoParams["MieStrength"] = mieStrength_;
  atmoParams["RayleighCollectionPower"] = rayleighCollectionPower_;
  atmoParams["MieCollectionPower"] = mieCollectionPower_;
  atmoParams["MieDistribution"] = mieDistribution_;
  atmoParams["LightDir"] = Vector3::DOWN;
  atmoParams["InvProj"] = cam_->GetProjection().Inverse();

  // Add custom quad commands to render path.
  for (unsigned i = 0; i < MAX_CUBEMAP_FACES; ++i) {
    RenderPathCommand cmd;
    cmd.tag_ = "ProcSky";
    cmd.type_ = CMD_QUAD;
    cmd.sortMode_ = SORT_BACKTOFRONT;
    cmd.pass_ = "base";
    cmd.outputs_.Push(MakePair(String("DiffProcSky"), (CubeMapFace)i));
    cmd.textureNames_[0] = "";
    cmd.vertexShaderName_ = "ProcSky";
    cmd.vertexShaderDefines_ = "";
    cmd.pixelShaderName_ = "ProcSky";
    cmd.pixelShaderDefines_ = "";
    cmd.shaderParameters_ = atmoParams;
    cmd.shaderParameters_["InvViewRot"] = faceRotations_[i];
    cmd.enabled_ = true;
    rPath_->AddCommand(cmd);
  }

  // Perform at least one render to avoid empty sky.
  Update();

  SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(ProcSky, HandleUpdate));
#if defined(PROCSKY_UI)
  SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(ProcSky, HandleKeyDown));
  ToggleUI(); // Initially display UI.
#endif
  return true;
}

void ProcSky::HandleUpdate(StringHash eventType, VariantMap& eventData) {
#if defined(PROCSKY_UI)
  if (!GetSubsystem<UI>()->GetFocusElement()) {
    Input* input(GetSubsystem<Input>());
    const float factor(0.5f);
    if (input->GetKeyDown(KEY_UP)) {
      float dt(eventData[Update::P_TIMESTEP].GetFloat());
      angVel_.x_ += dt * factor;
      SetRenderQueued(true);
    }
    if (input->GetKeyDown(KEY_DOWN)) {
      float dt(eventData[Update::P_TIMESTEP].GetFloat());
      angVel_.x_ -= dt * factor;
      SetRenderQueued(true);
    }
    if (input->GetKeyDown(KEY_LEFT)) {
      float dt(eventData[Update::P_TIMESTEP].GetFloat());
      angVel_.y_ -= dt * factor;
      SetRenderQueued(true);
    }
    if (input->GetKeyDown(KEY_RIGHT)) {
      float dt(eventData[Update::P_TIMESTEP].GetFloat());
      angVel_.y_ += dt * factor;
      SetRenderQueued(true);
    }
  }
#endif // defined(PROCSKY_UI)

  if (updateAuto_) {
    float dt(eventData[Update::P_TIMESTEP].GetFloat());
    // If using an interval, queue update when done waiting.
    if (updateInterval_ > 0) {
      updateWait_ -= dt;
      if (updateWait_ <= 0) {
        updateWait_ = updateInterval_;
        Update();
      }
    } else { // No interval; just update.
      Update();
    }
  }
#if defined(PROCSKY_UI)
  lightNode_->Rotate(Quaternion(angVel_.x_, angVel_.y_, angVel_.z_));
#endif
}

// Update shader parameters and queue a render.
void ProcSky::Update() {
  if (lightNode_) {
    // In the shader code, LightDir is the direction TO the object casting light, not the direction OF the light, so invert the direction.
    Vector3 lightDir(-lightNode_->GetWorldDirection());
    rPath_->SetShaderParameter("LightDir", lightDir);
  }
  ///@TODO Need only send changed parameters.
  rPath_->SetShaderParameter("Kr", Kr_);
  rPath_->SetShaderParameter("RayleighBrightness", rayleighBrightness_);
  rPath_->SetShaderParameter("MieBrightness", mieBrightness_);
  rPath_->SetShaderParameter("SpotBrightness", spotBrightness_);
  rPath_->SetShaderParameter("ScatterStrength", scatterStrength_);
  rPath_->SetShaderParameter("RayleighStrength", rayleighStrength_);
  rPath_->SetShaderParameter("MieStrength", mieStrength_);
  rPath_->SetShaderParameter("RayleighCollectionPower", rayleighCollectionPower_);
  rPath_->SetShaderParameter("MieCollectionPower", mieCollectionPower_);
  rPath_->SetShaderParameter("MieDistribution", mieDistribution_);
  rPath_->SetShaderParameter("InvProj", cam_->GetProjection().Inverse());
  SetRenderQueued(true);
}

bool ProcSky::SetRenderSize(unsigned size) {
  if (size >= 1) {
    // Create a TextureCube and assign to the ProcSky material.
    SharedPtr<TextureCube> skyboxTexCube(new TextureCube(context_));
    skyboxTexCube->SetName("DiffProcSky");
    skyboxTexCube->SetSize(size, Graphics::GetRGBAFormat(), TEXTURE_RENDERTARGET);
    skyboxTexCube->SetFilterMode(FILTER_BILINEAR);
    skyboxTexCube->SetAddressMode(COORD_U, ADDRESS_CLAMP);
    skyboxTexCube->SetAddressMode(COORD_V, ADDRESS_CLAMP);
    skyboxTexCube->SetAddressMode(COORD_W, ADDRESS_CLAMP);
    GetSubsystem<ResourceCache>()->AddManualResource(skyboxTexCube);

    skybox_->GetMaterial()->SetTexture(TU_DIFFUSE, skyboxTexCube);
    renderSize_ = size;
    return true;
  } else {
    URHO3D_LOGWARNING("ProcSky::SetSize (" + String(size) + ") ignored; requires size >= 1.");
  }
  return false;
}

void ProcSky::SetUpdateAuto(bool updateAuto) {
  if (updateAuto_ == updateAuto) return;
  updateAuto_ = updateAuto;
}

void ProcSky::SetRenderQueued(bool queued) {
  if (renderQueued_ == queued) return;
  // When using manual update, be notified after rendering.
  if (!updateAuto_)
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(ProcSky, HandlePostRenderUpdate));
  rPath_->SetEnabled("ProcSky", queued);
  renderQueued_ = queued;
}

void ProcSky::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData) {
  if (!updateAuto_)
    SetRenderQueued(false);
}

#if defined(PROCSKY_UI)

void ProcSky::ToggleUI() {
  UI* ui(GetSubsystem<UI>());
  UIElement* uiRoot(ui->GetRoot());
  // If window exists, remove it and return.
  Window* win(static_cast<Window*>(uiRoot->GetChild("ProcSkyWindow", true)));
  if (win) {
    win->Remove();
    return;
  }
  XMLFile* style(GetSubsystem<ResourceCache>()->GetResource<XMLFile>("UI/DefaultStyle.xml"));
  uiRoot->SetDefaultStyle(style);

  win = new Window(context_);
  uiRoot->AddChild(win);
  win->SetName("ProcSkyWindow");
  win->SetStyleAuto();
  win->SetMovable(true);
  win->SetResizable(true);
  win->SetLayout(LM_VERTICAL, 2, IntRect(2,2,2,2));
  win->SetAlignment(HA_LEFT, VA_TOP);
  win->SetOpacity(0.8f);

  // Create Window 'titlebar' container
  UIElement* titleBar(new UIElement(context_));
  titleBar->SetMinSize(0, 16);
  titleBar->SetVerticalAlignment(VA_TOP);
  titleBar->SetLayoutMode(LM_HORIZONTAL);
  Text* windowTitle(new Text(context_));
  windowTitle->SetText("ProcSky Parameters");
  titleBar->AddChild(windowTitle);
  win->AddChild(titleBar);
  windowTitle->SetStyleAuto();

  CreateSlider(win, "rayleighBrightness", &rayleighBrightness_, 100.0f);
  CreateSlider(win, "mieBrightness", &mieBrightness_, 10.0f);
  CreateSlider(win, "spotBrightness", &spotBrightness_, 200.0f);
  CreateSlider(win, "scatterStrength", &scatterStrength_, 1.0f);
  CreateSlider(win, "rayleighStrength", &rayleighStrength_, 5.0f);
  CreateSlider(win, "mieStrength", &mieStrength_, 2.0f);
  CreateSlider(win, "rayleighCollectionPower", &rayleighCollectionPower_, 10.0f);
  CreateSlider(win, "mieCollectionPower", &mieCollectionPower_, 10.0f);
  CreateSlider(win, "mieDistribution", &mieDistribution_, 10.0f);
  CreateSlider(win, "Kr_red", &Kr_.x_, 1.0f);
  CreateSlider(win, "Kr_green", &Kr_.y_, 1.0f);
  CreateSlider(win, "Kr_blue", &Kr_.z_, 1.0f);
  CreateSlider(win, "updateInterval", &updateInterval_, 10.0f);

  Graphics* graphics(GetSubsystem<Graphics>());
  IntVector2 scrSize(graphics->GetWidth(), graphics->GetHeight());
  IntVector2 winSize(scrSize);
  winSize.x_ = (int)(0.3f * winSize.x_); winSize.y_ = (int)(0.5f * winSize.y_);
  IntVector2 uiPos(uiRoot->GetPosition());
  win->SetSize(winSize);
  win->SetPosition(0, (scrSize.y_-winSize.y_)/2);
}

void ProcSky::CreateSlider(UIElement* parent, const String& label, float* target, float range) {
  UIElement* textContainer(new UIElement(context_));
  parent->AddChild(textContainer);
  textContainer->SetStyleAuto();
  textContainer->SetLayoutMode(LM_HORIZONTAL);
  textContainer->SetMaxSize(2147483647, 16);

  Text* text(new Text(context_));
  textContainer->AddChild(text);
  text->SetStyleAuto();
  text->SetAlignment(HA_LEFT, VA_TOP);
  text->SetText(label);
  text->SetMaxSize(2147483647, 16);

  Text* valueText(new Text(context_));
  textContainer->AddChild(valueText);
  valueText->SetStyleAuto();
  valueText->SetAlignment(HA_RIGHT, VA_TOP);
  valueText->SetText(String(*target));
  valueText->SetMaxSize(2147483647, 16);

  Slider* slider(new Slider(context_));
  parent->AddChild(slider);
  slider->SetStyleAuto();
  slider->SetAlignment(HA_LEFT, VA_TOP);
  slider->SetName(label);
  slider->SetRange(range);
  slider->SetValue(*target);

  slider->SetMaxSize(2147483647, 16);
  // Store target for handler to use.
  slider->SetVar(label, target);
  // Store value label for handler to use.
  slider->SetVar(label+"_value", valueText);
  SubscribeToEvent(slider, E_SLIDERCHANGED, URHO3D_HANDLER(ProcSky, HandleSliderChanged));
}

void ProcSky::HandleSliderChanged(StringHash eventType, VariantMap& eventData) {
  Slider* slider(static_cast<Slider*>(eventData[SliderChanged::P_ELEMENT].GetPtr()));
  assert(slider != NULL);

  float value(eventData[SliderChanged::P_VALUE].GetFloat());
  String sliderName(slider->GetName());
  // Get target member from node var and update it.
  float* target(static_cast<float*>(slider->GetVar(sliderName).GetVoidPtr()));
  *target = value;
  // Get stored value Text label and update it.
  Text* valueText(static_cast<Text*>(slider->GetVar(sliderName+"_value").GetVoidPtr()));
  valueText->SetText(String(value));
}

void ProcSky::HandleKeyDown(StringHash eventType, VariantMap& eventData) {
  if (GetSubsystem<UI>()->GetFocusElement()) { return; }
  int key(eventData[KeyDown::P_KEY].GetInt());
  int qual(eventData[KeyDown::P_QUALIFIERS].GetInt());

  if (key == KEY_U) {
    updateAuto_ = !updateAuto_;
  }
  else if (key == KEY_SPACE) {
    ToggleUI();
  }
  else if (key == '-') {
    angVel_ *= 0.9f;
  }
  else if (key == '=') {
    angVel_ *= 1.1f;
  }
  else if (key == KEY_0 || key == KEY_KP_0) {
    angVel_ = Vector3::ZERO;
  }

  if (key == KEY_KP_6) {
    lightNode_->SetRotation(Quaternion(faceRotations_[FACE_POSITIVE_X]));
  }
  else if (key == KEY_KP_4) {
    lightNode_->SetRotation(Quaternion(faceRotations_[FACE_NEGATIVE_X]));
  }
  else if (key == KEY_KP_7) {
    lightNode_->SetRotation(Quaternion(faceRotations_[FACE_POSITIVE_Y]));
  }
  else if (key == KEY_KP_1) {
    lightNode_->SetRotation(Quaternion(faceRotations_[FACE_NEGATIVE_Y]));
  }
  else if (key == KEY_KP_8) {
    lightNode_->SetRotation(Quaternion(faceRotations_[FACE_POSITIVE_Z]));
  }
  else if (key == KEY_KP_2) {
    lightNode_->SetRotation(Quaternion(faceRotations_[FACE_NEGATIVE_Z]));
  }

#if defined(PROCSKY_TEXTURE_DUMPING)
  else if (key == KEY_KP_9) {
    TextureCube* skyboxTexCube(static_cast<TextureCube*>(skybox_->GetMaterial()->GetTexture(TU_DIFFUSE)));
    String pathName(GetSubsystem<FileSystem>()->GetProgramDir());
    DumpTexCubeImages(skyboxTexCube, pathName + "DiffProcSky_");
  }
#endif
}
#endif

#if defined(PROCSKY_TEXTURE_DUMPING)

void ProcSky::DumpTexCubeImages(TextureCube* texCube, const String& filePathPrefix) {
  URHO3D_LOGINFO("Save TextureCube: " + filePathPrefix + "[0-5].png");
  for (unsigned j = 0; j < MAX_CUBEMAP_FACES; ++j) {
    Texture2D* faceTex(static_cast<Texture2D*>(texCube->GetRenderSurface((CubeMapFace)j)->GetParentTexture()));
    SharedPtr<Image> faceImage(new Image(context_));
    faceImage->SetSize(faceTex->GetWidth(), faceTex->GetHeight(), faceTex->GetComponents());
    String filePath(filePathPrefix + String(j) + ".png");
    if (!texCube->GetData((CubeMapFace)j, 0, faceImage->GetData())) {
      URHO3D_LOGERROR("...failed GetData() for face " + filePath);
    } else {
      faceImage->SavePNG(filePath);
    }
  }
}

void ProcSky::DumpTexture(Texture2D* texture, const String& filePath) {
  URHO3D_LOGINFO("Save texture: " + filePath);
  SharedPtr<Image> image(new Image(context_));
  image->SetSize(texture->GetWidth(), texture->GetHeight(), texture->GetComponents());

  if (!texture->GetData(0, image->GetData())) {
    URHO3D_LOGERROR("...failed GetData().");
  } else {
    image->SavePNG(filePath);
  }
}

#endif
