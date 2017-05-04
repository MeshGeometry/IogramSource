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

#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Core/Variant.h>

namespace Geomlib {
	/*
	Triangulates a closed polyline.
	If the polyline is non planar, a best fit plane will be found and the triangulation attempted.
	In this case, it is unlikely that a good triangulation will result.
	*/

	bool TriangulatePolygon(const Urho3D::Variant& polyIn, Urho3D::Variant& polyOut);
	bool TriangulatePolygon(const Urho3D::Variant& polyIn, 
							const Urho3D::VariantVector& holes, 
							Urho3D::Variant& polyOut);

	// Some methods for NMesh --> TriMesh conversion
	// Triangulates a specified face of an NMesh. 
	bool TriangulateFace(const Urho3D::Variant& NMeshIn,
		int faceIndex,
		Urho3D::VariantVector& tri_face_list
		);

	bool TriangulatePolygonB(const Urho3D::Variant& polyIn,
							const Urho3D::VariantVector& holes,
							Urho3D::Variant& polyOut);
};
