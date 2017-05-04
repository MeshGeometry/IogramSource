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

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Graphics/Model.h>

Urho3D::Variant Polyline_Make(const Urho3D::VariantVector& vertexList);
Urho3D::Variant Polyline_Make(const Urho3D::Vector<Urho3D::Vector3>& vertexList);
//Urho3D::Variant Polyline_Make(const Urho3D::VariantVector& vertexList, const Urho3D::VariantVector& edgeList);

bool Polyline_Verify(const Urho3D::Variant& polyline);
void Polyline_Close(Urho3D::Variant& polyline);

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