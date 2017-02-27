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


#include "CurveRenderer.h"

using namespace Urho3D;

struct VertexData
{
	Vector3 position; // size of 12 bytes +
	Vector3 normal; // size of 12 bytes +
	unsigned color;
};

CurveRenderer::CurveRenderer(Context* context) :
	Drawable(context, DRAWABLE_GEOMETRY),
	geometry_(new Geometry(context_)),
	vertexBuffer_(new VertexBuffer(context_)),
	indexBuffer_(new IndexBuffer(context_)),
	numPoints_(0),
	vertexDistance_(0.1f),
	width_(0.2f),
	startScale_(1.0f),
	endScale_(1.0f),
	endColor_(Color(1.0f, 1.0f, 1.0f, 0.0f)),
	startColor_(Color(1.0f, 1.0f, 1.0f, 1.0f)),
	needUpdate_(false),
	sorted_(false),
	forceUpdate_(false),
	updateInvisible_(false)
{
	geometry_->SetVertexBuffer(0, vertexBuffer_);
	geometry_->SetIndexBuffer(indexBuffer_);

	transforms_ = Matrix3x4::IDENTITY;

	batches_.Resize(1);
	batches_[0].geometry_ = geometry_;
	batches_[0].geometryType_ = GEOM_STATIC;
	batches_[0].worldTransform_ = &transforms_;
	batches_[0].numWorldTransforms_ = 1;

	Material* mat = GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/FlatFace.xml");
	batches_[0].material_ = mat;
}

CurveRenderer::~CurveRenderer()
{
}

void CurveRenderer::RegisterObject(Context* context)
{
	context->RegisterFactory<CurveRenderer>();

	URHO3D_ACCESSOR_ATTRIBUTE("Is Enabled", IsEnabled, SetEnabled, bool, true, AM_DEFAULT);
	URHO3D_COPY_BASE_ATTRIBUTES(Drawable);
	URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Material", GetMaterialAttr, SetMaterialAttr, ResourceRef, ResourceRef(Material::GetTypeStatic()), AM_DEFAULT);
	//URHO3D_ACCESSOR_ATTRIBUTE("Emitting", IsEmitting, SetEmitting, bool, true, AM_DEFAULT);
	URHO3D_ACCESSOR_ATTRIBUTE("Update Invisible", GetUpdateInvisible, SetUpdateInvisible, bool, false, AM_DEFAULT);
	//URHO3D_ENUM_ACCESSOR_ATTRIBUTE("Trail Type", GetOrientationType, SetOrientationType, CurveOrientationType, trailTypeNames, CO_FACE_CAMERA, AM_DEFAULT);
	//URHO3D_ACCESSOR_ATTRIBUTE("Tail Lifetime", GetLifetime, SetLifetime, float, 1.0f, AM_DEFAULT);
	//URHO3D_ACCESSOR_ATTRIBUTE("Tail Column", GetTailColumn, SetTailColumn, unsigned, 0, AM_DEFAULT);
	URHO3D_ACCESSOR_ATTRIBUTE("Vertex Distance", GetVertexDistance, SetVertexDistance, float, 0.1f, AM_DEFAULT);
	URHO3D_ACCESSOR_ATTRIBUTE("Width", GetWidth, SetWidth, float, 0.2f, AM_DEFAULT);
	URHO3D_ACCESSOR_ATTRIBUTE("Start Scale", GetStartScale, SetStartScale, float, 1.0f, AM_DEFAULT);
	URHO3D_ACCESSOR_ATTRIBUTE("End Scale", GetEndScale, SetEndScale, float, 1.0f, AM_DEFAULT);
	URHO3D_ACCESSOR_ATTRIBUTE("Start Color", GetStartColor, SetStartColor, Color, Color::WHITE, AM_DEFAULT);
	URHO3D_ACCESSOR_ATTRIBUTE("End Color", GetEndColor, SetEndColor, Color, Color(1.0f, 1.0f, 1.0f, 0.0f), AM_DEFAULT);
	//URHO3D_ACCESSOR_ATTRIBUTE("Animation LOD Bias", GetAnimationLodBias, SetAnimationLodBias, float, 1.0f, AM_DEFAULT);
	URHO3D_ACCESSOR_ATTRIBUTE("Sort By Distance", IsSorted, SetSorted, bool, false, AM_DEFAULT);
}

void CurveRenderer::ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results)
{
	// If no trail-level testing, use the Drawable test
	if (query.level_ < RAY_TRIANGLE)
	{
		Drawable::ProcessRayQuery(query, results);
		return;
	}

	// Check ray hit distance to AABB before proceeding with trail-level tests
	if (query.ray_.HitDistance(GetWorldBoundingBox()) >= query.maxDistance_)
		return;

	// Approximate the tails as spheres for raycasting
	for (unsigned i = 0; i < points_.Size() - 1; ++i)
	{
		Vector3 center = (points_[i] + points_[i + 1]) * 0.5f;
		Vector3 scale = width_ * Vector3::ONE;
		// Tail should be represented in cylinder shape, but we don't have this yet on Urho,
		// so this implementation will use bounding box instead (hopefully only temporarily)
		float distance = query.ray_.HitDistance(BoundingBox(center - scale, center + scale));
		if (distance < query.maxDistance_)
		{
			// If the code reaches here then we have a hit
			RayQueryResult result;
			result.position_ = query.ray_.origin_ + distance * query.ray_.direction_;
			result.normal_ = -query.ray_.direction_;
			result.distance_ = distance;
			result.drawable_ = this;
			result.node_ = node_;
			result.subObject_ = i;
			results.Push(result);
		}
	}
}

void CurveRenderer::OnSetEnabled()
{
	Drawable::OnSetEnabled();

}


void CurveRenderer::Update(const FrameInfo &frame)
{
	Drawable::Update(frame);

}


///set the points
void CurveRenderer::SetVertices(Vector<Vector3> verts, bool isClosed)
{
	numPoints_ = verts.Size();

	points_.Clear();
	points_.Resize(verts.Size());

	if (numPoints_ < 2)
		return;
	
	//clears the vertex and index buffers
	int numIndices = isClosed ? numPoints_ * 6 : (numPoints_ - 1) * 6;
	indexBuffer_->SetSize(numIndices, true, false);

	int numVerts = numPoints_* 2;
	//TODO: use UVs and Tangents?
	vertexBuffer_->SetSize(numVerts, MASK_POSITION | MASK_COLOR | MASK_NORMAL);
	
	//add the new indices like this:
	Vector<unsigned int> indices;
	indices.Resize(numIndices);
	indexBuffer_->SetData(&indices[0]);

	//hack...
	for (unsigned i = 0; i < numPoints_; i++)
	{
		points_[i] = verts[i];
	}

	unsigned int* indexDest = (unsigned int*)indexBuffer_->Lock(0, (numPoints_ - 1) * 6, true);
	if (!indexDest)
		return;

	for (unsigned i = 0; i < numPoints_ - 1; ++i)
	{
		indexDest[0] = 2 * i;
		indexDest[1] = 2 * (i + 1) + 1;
		indexDest[2] = 2 * i + 1;

		indexDest[3] = 2 * i;
		indexDest[4] = 2 * (i + 1);
		indexDest[5] = 2 * (i + 1) + 1;

		indexDest += 6;
	}

	indexBuffer_->Unlock();
	indexBuffer_->ClearDataLost();

	
	//update vertex buffer
	needUpdate_ = true;
}

void CurveRenderer::UpdateVertexBuffer(const FrameInfo& frame)
{

	float* dest = (float*)vertexBuffer_->Lock(0, numPoints_ * 2, true);
	if (!dest)
		return;

	Vector3 camDir = frame.camera_->GetNode()->GetWorldDirection();
	unsigned int col = Color::RED.ToUInt();

	for (int i = 0; i < numPoints_; i++)
	{
		Vector3 bisector = Vector3::UP;

		Matrix3x4 localFrame;
		Vector3 xAxis = Vector3::RIGHT;
		Vector3 yAxis = Vector3::UP;
		Vector3 zAxis = Vector3::FORWARD;
		//localFrame.SetScale(Vector3(1.0f, 1.0f, width_));

		Vector3 camDir = frame.camera_->GetNode()->GetWorldDirection();
		Vector3 camPos = frame.camera_->GetNode()->GetWorldPosition();
		Vector3 camVec = camPos - points_[i];

		if (i > 0 && i < numPoints_ - 1)
		{
			Vector3 leftHandDir = (points_[i - 1] - points_[i]).Normalized();
			Vector3 rightHandDir = (points_[i + 1] - points_[i]).Normalized();
			float angle = leftHandDir.Angle(rightHandDir);

			//rotate the left dir to find the bisector
			Quaternion rot;
			Vector3 cross = leftHandDir.CrossProduct(rightHandDir);		
			rot.FromAngleAxis(0.5f * angle, cross);
			bisector = rot.RotationMatrix() * leftHandDir;

			//rotate the bisector to be perpendicular to the view dir
			//Quaternion camRot;
			float sign = Sign(bisector.DotProduct(camVec));
			//camRot.FromRotationTo(cross, -1.0f * sign * camVec);
			//bisector = camRot*bisector;
			Vector3 nCross = bisector.CrossProduct(cross);
			Quaternion nRot;
			nRot.FromLookRotation(sign * camVec, nCross);
			bisector = nRot * bisector;
		}
		if (i == 0)
		{

		}
		
		localFrame.SetTranslation(points_[i]);
		
		//first point
		Vector3 posA = points_[i] + width_*bisector;// localFrame * Vector3(1.0f, 0.0f, 0.0f);
		//Vector3 posA = localFrame * Vector3(0.0f, 0.0f, width_);
		dest[0] = posA.x_;
		dest[1] = posA.y_;
		dest[2] = posA.z_;
		((unsigned&)dest[3]) = col;
		dest[4] = 0.f;
		dest[5] = 0.f;
		dest[6] = 0.f;

		dest += 7;

		Vector3 posB = points_[i] - width_*bisector; //localFrame * Vector3(-1.0f, 0.0f, 0.0f);
		//Vector3 posB = localFrame * Vector3(0.0f, 0.0f, -width_);
		dest[0] = posB.x_;
		dest[1] = posB.y_;
		dest[2] = posB.z_;
		((unsigned&)dest[3]) = col;
		dest[4] = 0.f;
		dest[5] = 0.f;
		dest[6] = 0.f;

		dest += 7;

	}

	vertexBuffer_->Unlock();
	vertexBuffer_->ClearDataLost();


	//batches_[0].geometry_->SetVertexBuffer(0, vertexBuffer_);
	//batches_[0].geometry_->SetIndexBuffer(indexBuffer_);
	batches_[0].geometry_->SetDrawRange(TRIANGLE_LIST, 0, (numPoints_ - 1) * 6, false);

}

void CurveRenderer::SetEndScale(float endScale)
{
	endScale_ = endScale;
	Commit();
}

void CurveRenderer::SetStartScale(float startScale)
{
	startScale_ = startScale;
	Commit();
}


void CurveRenderer::UpdateBatches(const FrameInfo& frame)
{
	// Update information for renderer about this drawable
	distance_ = frame.camera_->GetDistance(GetWorldBoundingBox().Center());
	batches_[0].distance_ = distance_;

	// Calculate scaled distance for animation LOD
	float scale = GetWorldBoundingBox().Size().DotProduct(DOT_SCALE);
	// If there are no trail, the size becomes zero, and LOD'ed updates no longer happen. Disable LOD in that case
	if (scale > M_EPSILON)
		lodDistance_ = frame.camera_->GetLodDistance(distance_, scale, lodBias_);
	else
		lodDistance_ = 0.0f;

	Vector3 worldPos = node_->GetWorldPosition();
	Vector3 offset = (worldPos - frame.camera_->GetNode()->GetWorldPosition());

}

void CurveRenderer::UpdateGeometry(const FrameInfo& frame)
{
	if (needUpdate_)
	{
		UpdateVertexBuffer(frame);
	}
}

/// Return whether a geometry update is necessary, and if it can happen in a worker thread.
UpdateGeometryType CurveRenderer::GetUpdateGeometryType()
{
	return UPDATE_MAIN_THREAD;
}

void CurveRenderer::SetMaterial(Material* material)
{
	batches_[0].material_ = material;
	MarkNetworkUpdate();
}

void CurveRenderer::OnSceneSet(Scene* scene)
{
	Drawable::OnSceneSet(scene);

}

void CurveRenderer::OnWorldBoundingBoxUpdate()
{
	BoundingBox worldBox;

	for (unsigned i = 0; i < points_.Size(); ++i)
	{
		Vector3 &p = points_[i];
		Vector3 scale = width_ * Vector3::ONE;
		worldBox.Merge(BoundingBox(p - scale, p + scale));
	}

	worldBoundingBox_ = worldBox;
}


void CurveRenderer::SetVertexDistance(float length)
{
	vertexDistance_ = length;
	Commit();
}

void CurveRenderer::SetEndColor(const Color& color)
{
	endColor_ = color;
	Commit();
}

void CurveRenderer::SetStartColor(const Color& color)
{
	startColor_ = color;
	Commit();
}

void CurveRenderer::SetSorted(bool enable)
{
	sorted_ = enable;
	Commit();
}

void CurveRenderer::SetMaterialAttr(const ResourceRef& value)
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	SetMaterial(cache->GetResource<Material>(value.name_));
	Commit();
}

void CurveRenderer::SetWidth(float width)
{
	width_ = width;
	Commit();
}

void CurveRenderer::SetUpdateInvisible(bool enable)
{
	updateInvisible_ = enable;
	MarkNetworkUpdate();
}

void CurveRenderer::Commit()
{
	MarkPositionsDirty();
	MarkNetworkUpdate();
}

void CurveRenderer::MarkPositionsDirty()
{
	Drawable::OnMarkedDirty(node_);
}

Material* CurveRenderer::GetMaterial() const
{
	return batches_[0].material_;
}

ResourceRef CurveRenderer::GetMaterialAttr() const
{
	return GetResourceRef(batches_[0].material_, Material::GetTypeStatic());
}


