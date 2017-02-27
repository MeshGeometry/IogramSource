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


#include "Geomlib_PolylineExtrude.h"
#include "Polyline.h"
#include "TriMesh.h"

using Urho3D::Variant;
using Urho3D::Vector3;
using Urho3D::VariantVector;

namespace Geomlib {

	bool PolylineExtrude(
		const Urho3D::Variant& polyIn,
		Urho3D::Variant& meshOut,
		Urho3D::Vector3 dir
		)
	{
		if (!Polyline_Verify(polyIn))
		{
			return false;
		}

		VariantVector verts = Polyline_ComputeSequentialVertexList(polyIn);
		VariantVector faces;
		int vertOffset = verts.Size();
		float endPointDistance = (verts[0].GetVector3() - verts[vertOffset - 1].GetVector3()).Length();
		bool isClosed = (endPointDistance < 1.0f / Urho3D::M_LARGE_VALUE) ? true : false;

		//itr num
		int itrNum = vertOffset - 1;

		//if closed, delete the extra point at end of polyline
		if (isClosed)
		{
			verts.Erase(vertOffset - 1, 1);
			vertOffset = verts.Size();
			itrNum = vertOffset;
		}

		//push new vertices
		for (int i = 0; i < vertOffset; i++)
		{
			verts.Push(verts[i].GetVector3() + dir);
		}

		//create the faces
		for (int i = 0; i < itrNum; i++)
		{
			int a, b, c;

			//first triangle
			a = i;
			b = i + vertOffset;
			c = (i + 1) % vertOffset;

			faces.Push(a); faces.Push(b); faces.Push(c);

			//second triangle
			a = (i + 1) % vertOffset;
			b = i + vertOffset;
			c = (i + 1) % vertOffset + vertOffset;

			faces.Push(a); faces.Push(b); faces.Push(c);
		}

		meshOut = TriMesh_Make(verts, faces);
		return true;
	}

}
