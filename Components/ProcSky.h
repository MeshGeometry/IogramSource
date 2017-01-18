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

#define PROCSKY_UI
//#define PROCSKY_TEXTURE_DUMPING

#pragma once
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Math/Matrix3.h>
#include <Urho3D/Math/Vector3.h>

namespace Urho3D {
class Skybox;
class StringHash;
#if defined(PROCSKY_UI)
class UIElement;
#endif
#if defined(PROCSKY_TEXTURE_DUMPING)
class Texture2D;
class TextureCube;
#endif
}

class ProcSky: public Urho3D::Component {
  URHO3D_OBJECT(ProcSky, Urho3D::Component);

public:
  ProcSky(Urho3D::Context* context);
  virtual ~ProcSky();
  static void RegisterObject(Urho3D::Context* context);
  void OnNodeSet(Urho3D::Node* node);

  /// Automatic update renders according to update interval. If Manual, user calls Update() to render.
  void SetUpdateAuto(bool updateAuto);
  bool GetUpdateAuto() const { return updateAuto_; }
  /// Set the rendering interval (default 0).
  void SetUpdateInterval(float interval) { updateInterval_ = interval; }
  float GetUpdateInterval() const { return updateInterval_; }
  float GetUpdateWait() const { return updateWait_; }
  /// Set size of Skybox TextureCube.
  bool SetRenderSize(unsigned size);
  unsigned GetRenderSize() const { return renderSize_; }
  /// Initialize objects and subscribe to update events.
  bool Initialize();
  /// Queue render of next frame.
  void Update();
  void SetCamera(Urho3D::Camera* camIn) { cam_ = camIn; };

protected:
  void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
  /// Set rendering of next frame active/inactive.
  void SetRenderQueued(bool enable);
  void HandlePostRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

#if defined(PROCSKY_UI)
  void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
  void ToggleUI();
  void CreateSlider(Urho3D::UIElement* parent, const Urho3D::String& label, float* target, float range);
  void HandleSliderChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
#endif

#if defined(PROCSKY_TEXTURE_DUMPING)
  void DumpTexCubeImages(Urho3D::TextureCube* texCube, const Urho3D::String& filePathPrefix);
  void DumpTexture(Urho3D::Texture2D* texture, const Urho3D::String& filePath);
#endif

protected:
  /// Camera used for face projections.
  Urho3D::Camera* cam_;
  /// Urho3D Skybox with geometry and main TextureCube.
  Urho3D::SharedPtr<Urho3D::Skybox> skybox_;
  /// Node used for light direction.
  Urho3D::WeakPtr<Urho3D::Node> lightNode_;
  Urho3D::SharedPtr<Urho3D::RenderPath> rPath_;
  /// Render size of each face.
  unsigned renderSize_;
  /// FOV used to initialize the default camera. Can adjust for Skybox seams.
  float renderFOV_;
  /// Fixed rotations for each cube face.
  Urho3D::Matrix3 faceRotations_[Urho3D::MAX_CUBEMAP_FACES];

  bool updateAuto_;
  float updateInterval_;
  float updateWait_;
  bool renderQueued_;
#if defined(PROCSKY_UI)
  Urho3D::Vector3 angVel_;
#endif
  /// Atmospheric parameters.
  Urho3D::Vector3 Kr_; // Absorption profile of air.
  float rayleighBrightness_;
  float mieBrightness_;
  float spotBrightness_;
  float scatterStrength_;
  float rayleighStrength_;
  float mieStrength_;
  float rayleighCollectionPower_;
  float mieCollectionPower_;
  float mieDistribution_;
};