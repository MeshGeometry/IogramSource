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


#include "Geomlib_TriMeshSaveOFF.h"
#include <Urho3D/Core/StringUtils.h>

using Urho3D::File;
using Urho3D::FileMode;
using Urho3D::String;
using Urho3D::VariantMap;
using Urho3D::VariantVector;
using Urho3D::Vector3;

bool Geomlib::TriMeshWriteOFF(
	const Urho3D::Variant& meshIn,
	File& destination)
{

	if (destination.GetMode() != FileMode::FILE_WRITE)
	{
		return false;
	}

	VariantMap meshMap = meshIn.GetVariantMap();
	if (meshMap.Keys().Contains("vertices") && meshMap.Keys().Contains("faces"))
	{
		VariantVector verts = meshMap["vertices"].GetVariantVector();
		VariantVector faces = meshMap["faces"].GetVariantVector();
		
		//write header
		destination.WriteLine("OFF");

		//write data info
		String info = "";
		info.AppendWithFormat("%u %u %u", verts.Size(), faces.Size()/3, 0);
		destination.WriteLine(info);

		//write the verts
		for (unsigned i = 0; i < verts.Size(); i++)
		{
			Vector3 vert = verts[i].GetVector3();
			String vertLine = "";
			vertLine.AppendWithFormat("%f %f %f", vert.x_, vert.y_, vert.z_);
			destination.WriteLine(vertLine);
		}

		//write the faces
		for (unsigned i = 0; i < faces.Size()/3; i++)
		{
			int a = faces[3 * i].GetInt();
			int b = faces[3 * i + 1].GetInt();
			int c = faces[3 * i + 2].GetInt();

			String vertLine = "";
			vertLine.AppendWithFormat("%u %u %u %u",3,a,b,c);
			destination.WriteLine(vertLine);
		}

	}
	return true;
}