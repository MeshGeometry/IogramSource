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
