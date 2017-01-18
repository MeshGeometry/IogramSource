#pragma once
// All Urho3D classes reside in namespace Urho3D
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Graphics/Camera.h>

enum CameraViews
{
	TOP,
	LEFT,
	RIGHT,
	BOTTOM,
	FRONT,
	BACK
};

/// Custom logic component for rotating a scene node.
class OrbitCamera : public Urho3D::LogicComponent
{
    URHO3D_OBJECT(OrbitCamera, Urho3D::LogicComponent)
    
public:
    /// Construct.
    OrbitCamera(Urho3D::Context* context);

    /// Handle scene update. Called by LogicComponent base class.
    virtual void Update(float timeStep);
    virtual void Start();
	Urho3D::Camera* camera_;

	///manipulations
	void SetView(CameraViews view);
	void ZoomExtents(Urho3D::Vector3 min, Urho3D::Vector3 max);
	void ZoomExtents();
private:
	Urho3D::Vector3 view_center_;
	Urho3D::SharedPtr<Urho3D::Node> cam_node_;
	Urho3D::SharedPtr<Urho3D::Node> light_node_;
	Urho3D::SharedPtr<Urho3D::Node> second_light_;
};