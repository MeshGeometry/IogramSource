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


#include "Geomlib_MeshTetrahedralize.h"

#include <iostream>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>

#include "TriMesh.h"
#include "tetgen.h"

using Urho3D::Variant;
using Urho3D::VariantMap;
using Urho3D::VariantVector;
using Urho3D::Vector3;
using Urho3D::Vector;

bool Geomlib::MeshTetrahedralize(
	const Urho3D::Variant& meshIn,
	float maxVolume,
	Urho3D::Variant& meshOut
)
{
	if (!TriMesh_Verify(meshIn)) {
		std::cerr << "ERROR: Geomlib::Offset --- meshIn unverified\n";
		meshOut = Variant();
		return false;
	}

	bool success = false;

	// meshIn: verify and parse
	VariantMap meshMap = meshIn.GetVariantMap();
	const VariantVector vertexList = meshMap["vertices"].GetVariantVector();
	const VariantVector faceList = meshMap["faces"].GetVariantVector();

	Vector<double> vertDoubles = TriMesh_GetVerticesAsDoubles(meshIn);
	Vector<int> faceInts = TriMesh_GetFacesAsInts(meshIn);

	//set up io
	tetgenio in, out;

	//set up the points
	const int numPointsFlat = vertDoubles.Size();

	in.firstnumber = 0;
	in.numberofpoints = numPointsFlat / 3;
	in.pointlist = new REAL[numPointsFlat];
	
	for (int i = 0; i < numPointsFlat; i++)
	{
		in.pointlist[i] = (REAL)vertDoubles[i];
	}


	//set up the faces
	const int numFacesFlat = faceInts.Size()/3;
	in.numberoffacets = numFacesFlat;
	in.facetlist = new tetgenio::facet[in.numberoffacets];

	tetgenio::facet * facet;
	tetgenio::polygon * polygon;
	int * surface;
	for (int i = 0; i < faceInts.Size() / 3; i++)
	{
		facet = &in.facetlist[i];
		facet->numberofpolygons = 1;
		facet->numberofholes = 0;
		facet->holelist = NULL;
		facet->polygonlist = new tetgenio::polygon[1];
		polygon = &facet->polygonlist[0];
		polygon->numberofvertices = 3;
		polygon->vertexlist = new int[3];
		surface = &polygon->vertexlist[0];
		surface[0] = faceInts[3 * i + 0];
		surface[1] = faceInts[3 * i + 1];
		surface[2] = faceInts[3 * i + 2];
	}

	tetgenbehavior behaviour;
	//behaviour.zeroindex = 1;
	//behaviour.verbose = 0; // 0 is the default value, see Mesh.h
	behaviour.quiet = true;
	behaviour.neighout = 1;
	behaviour.plc = 1;
	behaviour.quality = 5;
	//behaviour.facet_overlap_ang_tol = 0.000001;
	behaviour.edgesout = 1;
	//behaviour.addsteiner_algo = 2;
	behaviour.insertaddpoints = 1;
	behaviour.fixedvolume = 1;
	//behaviour.refine = 1;
	behaviour.flipinsert = 1;

	if (maxVolume > 0)
		behaviour.maxvolume = maxVolume;

	//behaviour.meditview = 1;
	//behaviour.convex = 0;
	//behaviour.insertaddpoints = 1;

	try {
		tetrahedralize(&behaviour, &in, &out);
		success = true;
	}
	catch (...) {
		success = false;
	}

	if (success) {

		VariantVector newVerts;
		VariantVector faces;
		VariantVector tets;
		VariantVector edges;

		for (int i = 0; i < out.numberofpoints; i++)
		{
			Vector3 v(
				out.pointlist[3 * i],
				out.pointlist[3 * i + 1],
				out.pointlist[3 * i + 2]
			);

			newVerts.Push(v);
		}

		for (int i = 0; i < out.numberoftrifaces; ++i) {
			faces.Push(out.trifacelist[3 * i]);
			faces.Push(out.trifacelist[3 * i + 2]);
			faces.Push(out.trifacelist[3 * i + 1]);
		}

		for (int i = 0; i < out.numberoftetrahedra; i++)
		{
			tets.Push(out.tetrahedronlist[3 * i]);
			tets.Push(out.tetrahedronlist[3 * i + 1]);
			tets.Push(out.tetrahedronlist[3 * i + 2]);
			tets.Push(out.tetrahedronlist[3 * i + 3]);
		}

		for (int i = 0; i < out.numberofedges; i++)
		{
			edges.Push(out.edgelist[2 * i]);
			edges.Push(out.edgelist[2 * i + 1]);
		}

		meshOut = TriMesh_Make(newVerts, faces);
		
		//manually append the tets since we don't have a tet mesh class yet
		VariantMap map = meshOut.GetVariantMap();
		map["tetrahedra"] = tets;
		map["tet_edges"] = edges;
		
		//set it again
		meshOut = map;

	}
	else {
		std::cout << "Mesh::tetrahedralize() FAILED to perform tetrahedralization!" << std::endl;
	}


	return success;
}