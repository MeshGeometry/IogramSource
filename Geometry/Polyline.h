#pragma once

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Graphics/Model.h>

Urho3D::Variant Polyline_Make(const Urho3D::VariantVector& vertexList);
//Urho3D::Variant Polyline_Make(const Urho3D::VariantVector& vertexList, const Urho3D::VariantVector& edgeList);

bool Polyline_Verify(const Urho3D::Variant& polyline);

Urho3D::VariantVector Polyline_GetVertexList(const Urho3D::Variant& polyline);
int Polyline_GetSequentialVertexNumber(const Urho3D::Variant& polyline);

Urho3D::VariantVector Polyline_ComputeSequentialVertexList(const Urho3D::Variant& polyline);
float Polyline_GetCurveLength(const Urho3D::Variant& polyline);

Urho3D::Variant Polyline_Clean(const Urho3D::Variant& polyline);

Urho3D::Vector<Urho3D::Vector3> Polyline_ComputePointCloud(const Urho3D::Variant& polyline);
Urho3D::Vector<Urho3D::Pair<int, int>> Polyline_ComputeEdges(const Urho3D::Variant& polyline);
bool Polyline_IsClosed(const Urho3D::Variant& polyline);

Urho3D::Variant Polyline_ApplyTransform(
	const Urho3D::Variant& polyline,
	const Urho3D::Matrix3x4& T
);

Urho3D::SharedPtr<Urho3D::Model> Polyline_GetRenderMesh(const Urho3D::Variant& triMesh, Urho3D::Context* context, Urho3D::VariantVector vCols, float thickness, bool split=false);