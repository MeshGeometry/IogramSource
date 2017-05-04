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


#pragma once

#include "Urho3D/Graphics/Drawable.h"
#include "Urho3D/Core/Context.h"
#include "Urho3D/Graphics/VertexBuffer.h"
#include "Urho3D/Graphics/IndexBuffer.h"
#include "Urho3D/Graphics/Camera.h"
#include "Urho3D/Graphics/Material.h"
#include "Urho3D/Graphics/OctreeQuery.h"
#include "Urho3D/Graphics/Geometry.h"
#include "Urho3D/Scene/Scene.h"
#include "Urho3D/Scene/SceneEvents.h"
#include "Urho3D/Resource/ResourceCache.h"
#include "Urho3D/IO/Log.h"

/// Drawable component that creates a tail.
class URHO3D_API CurveRenderer : public Urho3D::Drawable
{
	URHO3D_OBJECT(CurveRenderer, Urho3D::Drawable);

public:
	/// Construct.
	CurveRenderer(Urho3D::Context* context);
	/// Destruct.
	virtual ~CurveRenderer();
	/// Register object factory.
	static void RegisterObject(Urho3D::Context* context);
	/// Process octree raycast. May be called from a worker thread.
	virtual void ProcessRayQuery(const Urho3D::RayOctreeQuery& query, Urho3D::PODVector<Urho3D::RayQueryResult>& results);
	/// Handle enabled/disabled state change.
	virtual void OnSetEnabled();
	/// Update before octree reinsertion. Is called from a main thread.
	virtual void Update(const Urho3D::FrameInfo &frame);
	/// Calculate distance and prepare batches for rendering. May be called from worker thread(s), possibly re-entrantly.
	virtual void UpdateBatches(const Urho3D::FrameInfo& frame);
	/// Prepare geometry for rendering. Called from a worker thread if possible (no GPU update.)
	virtual void UpdateGeometry(const Urho3D::FrameInfo& frame);
	/// Return whether a geometry update is necessary, and if it can happen in a worker thread.
	virtual Urho3D::UpdateGeometryType GetUpdateGeometryType();
	/// update vertex buffer
	void UpdateVertexBuffer(const Urho3D::FrameInfo& frame);

	///set the points
	void SetVertices(Urho3D::Vector<Urho3D::Vector3> verts, bool isClosed);
	/// Set material.
	void SetMaterial(Urho3D::Material* material);
	/// Set material attribute.
	void SetMaterialAttr(const Urho3D::ResourceRef& value);
	/// Set distance between points.
	void SetVertexDistance(float length);
	/// Set width of the tail. Only works for face camera trail type.
	void SetWidth(float width);
	/// Set vertex blended color for start of trail.
	void SetStartColor(const Urho3D::Color& color);
	/// Set vertex blended scale for end of trail.
	void SetEndColor(const Urho3D::Color& color);
	/// Set vertex blended color for start of trail.
	void SetStartScale(float startScale);
	/// Set vertex blended scale for end of trail.
	void SetEndScale(float endScale);
	/// Set whether tails are sorted by distance. Default false.
	void SetSorted(bool enable);
	/// Set whether to update when trail emiiter are not visible.
	void SetUpdateInvisible(bool enable);
	/// Mark for bounding box and vertex buffer update. Call after modifying the trails.
	void Commit();

	/// Return material.
	Urho3D::Material* GetMaterial() const;
	/// Return material attribute.
	Urho3D::ResourceRef GetMaterialAttr() const;
	/// Get distance between points.
	float GetVertexDistance() const { return vertexDistance_; }
	/// Get width of the trail.
	float GetWidth() const { return width_; }
	/// Get vertex blended color for start of trail.
	const Urho3D::Color& GetStartColor() const { return startColor_; }
	/// Get vertex blended color for end of trail.
	const Urho3D::Color& GetEndColor() const { return endColor_; }
	/// Get vertex blended scale for start of trail.
	float GetStartScale() const { return startScale_; }
	/// Get vertex blended scale for end of trail.
	float GetEndScale() const { return endScale_; }
	/// Return whether tails are sorted.
	bool IsSorted() const { return sorted_; }
	/// Return whether to update when trail emitter are not visible.
	bool GetUpdateInvisible() const { return updateInvisible_; }

protected:
	/// Handle node being assigned.
	virtual void OnSceneSet(Urho3D::Scene* scene);
	/// Recalculate the world-space bounding box.
	virtual void OnWorldBoundingBoxUpdate();
	/// Mark vertex buffer to need an update.
	void MarkPositionsDirty();
	/// Tails.
	Urho3D::PODVector<Urho3D::Vector3> points_;
	/// Tails sorted flag.
	bool sorted_;
private:
	/// Geometry.
	Urho3D::SharedPtr<Urho3D::Geometry> geometry_;
	/// Vertex buffer.
	Urho3D::SharedPtr<Urho3D::VertexBuffer> vertexBuffer_;
	/// Index buffer.
	Urho3D::SharedPtr<Urho3D::IndexBuffer> indexBuffer_;
	/// Transform matrices for position and orientation.
	Urho3D::Matrix3x4 transforms_;
	/// Distance between points. Basically is tail length.
	float vertexDistance_;
	/// Width of trail.
	float width_;
	/// Number of points.
	unsigned numPoints_;
	/// Color for start of trails.
	Urho3D::Color startColor_;
	/// Color for end of trails.
	Urho3D::Color endColor_;
	/// Scale for start of trails.
	float startScale_;
	/// End for start of trails.
	float endScale_;
	/// Need update flag.
	bool needUpdate_;
	/// Trail pointers for sorting.
	Urho3D::Vector<Urho3D::Vector3> sortedPoints_;
	/// Force update flag (ignore animation LOD momentarily.)
	bool forceUpdate_;
	/// Update when invisible flag.
	bool updateInvisible_;
};