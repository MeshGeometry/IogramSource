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
#include "Polyline.h"
#include "tetgen.h"
#include "Geomlib_Incenter.h"


using Urho3D::Variant;
using Urho3D::VariantMap;
using Urho3D::VariantVector;
using Urho3D::Vector3;
using Urho3D::Vector;

Vector<double> VariantVectorToDoubles(VariantVector verts)
{
    Vector<double> vertsOut;
    int numVerts = verts.Size();
    vertsOut.Resize(numVerts * 3);
    
    for (int i = 0; i < numVerts; i++)
    {
        Vector3 v = verts[i].GetVector3();
        vertsOut[3 * i] = (double)v.x_;
        vertsOut[3 * i + 1] = (double)v.y_;
        vertsOut[3 * i + 2] = (double)v.z_;
    }
    
    return vertsOut;
}


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
    //behaviour.quality = 0;
	//behaviour.facet_overlap_ang_tol = 0.000001;
	behaviour.edgesout = 1;
	//behaviour.addsteiner_algo = 2;
	//behaviour.insertaddpoints = 1;
	//behaviour.fixedvolume = 1;
	////behaviour.refine = 1;
	//behaviour.flipinsert = 1;
 //   
 //   behaviour.mindihedral = 1.0;
 //   behaviour.optmaxdihedral = 179.0;
 //   behaviour.weighted = 1;
	//behaviour.verbose = 1;

	if (maxVolume > 0)
	{
		behaviour.maxvolume = maxVolume;
		behaviour.fixedvolume = 1;
	}


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
			tets.Push(out.tetrahedronlist[4 * i]);
			tets.Push(out.tetrahedronlist[4 * i + 1]);
			tets.Push(out.tetrahedronlist[4 * i + 2]);
			tets.Push(out.tetrahedronlist[4 * i + 3]);
		}

		for (int i = 0; i < out.numberofedges; i++)
		{
			edges.Push(out.edgelist[2 * i]);
			edges.Push(out.edgelist[2 * i + 1]);
		}

		

		VariantVector tets_out;

		for (int i = 0; i < tets.Size() / 4; ++i) {
			VariantVector V;
			VariantVector F;
			int a = tets[4 * i].GetInt();
			int b = tets[4 * i + 1].GetInt();
			int c = tets[4 * i + 2].GetInt();
			int d = tets[4 * i + 3].GetInt();

			V.Push(Variant(newVerts[a].GetVector3()));
			V.Push(Variant(newVerts[b].GetVector3()));
			V.Push(Variant(newVerts[c].GetVector3()));
			V.Push(Variant(newVerts[d].GetVector3()));

			F.Push(0);
			F.Push(1);
			F.Push(2);

			F.Push(0);
			F.Push(2);
			F.Push(3);

			F.Push(3);
			F.Push(2);
			F.Push(1);

			F.Push(1);
			F.Push(0);
			F.Push(3);

			Variant T = TriMesh_Make(V, F);
			tets_out.Push(T);

		}

		meshOut = TriMesh_Make(newVerts, faces);
		
		//manually append the tets since we don't have a tet mesh class yet
		VariantMap map = meshOut.GetVariantMap();
		map["tetrahedra"] = tets;
		map["tet_edges"] = edges;
		map["tet_meshes"] = tets_out;
		
		//set it again
		meshOut = map;

	}
	else {
		std::cout << "Mesh::tetrahedralize() FAILED to perform tetrahedralization!" << std::endl;
	}


	return success;
}

bool Geomlib::MeshTetrahedralizeFromPoints(
      const Urho3D::Variant& meshIn,
      const Urho3D::VariantVector& pointsIn,
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
    Vector<double> otherPtsDoubles = VariantVectorToDoubles(pointsIn);
    Vector<int> faceInts = TriMesh_GetFacesAsInts(meshIn);
    
    //set up io
    tetgenio in, out, addin;
    
    //set up the points
    const int numPointsFlat = vertDoubles.Size();
    
    in.firstnumber = 0;
    in.numberofpoints = numPointsFlat / 3;
    in.pointlist = new REAL[numPointsFlat];
    
    for (int i = 0; i < vertDoubles.Size(); i++)
    {
        in.pointlist[i] = (REAL)vertDoubles[i];
    }
    
    // addin point
    const int numAddInPts = otherPtsDoubles.Size();
    
    addin.firstnumber = 0;
    addin.numberofpoints = numAddInPts / 3;
    addin.pointlist = new REAL[numAddInPts];
    
    for (int i = 0; i < otherPtsDoubles.Size(); i++)
    {
        addin.pointlist[i] = (REAL)otherPtsDoubles[i];
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
        tetrahedralize(&behaviour, &in, &out, &addin);
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
            tets.Push(out.tetrahedronlist[4 * i]);
            tets.Push(out.tetrahedronlist[4 * i + 1]);
            tets.Push(out.tetrahedronlist[4 * i + 2]);
            tets.Push(out.tetrahedronlist[4 * i + 3]);
        }
        
        for (int i = 0; i < out.numberofedges; i++)
        {
            edges.Push(out.edgelist[2 * i]);
            edges.Push(out.edgelist[2 * i + 1]);
        }
        
        
        
        VariantVector tets_out;
        
        for (int i = 0; i < tets.Size() / 4; ++i) {
            VariantVector V;
            VariantVector F;
            int a = tets[4 * i].GetInt();
            int b = tets[4 * i + 1].GetInt();
            int c = tets[4 * i + 2].GetInt();
            int d = tets[4 * i + 3].GetInt();
            
            V.Push(Variant(newVerts[a].GetVector3()));
            V.Push(Variant(newVerts[b].GetVector3()));
            V.Push(Variant(newVerts[c].GetVector3()));
            V.Push(Variant(newVerts[d].GetVector3()));
            
            F.Push(0);
            F.Push(1);
            F.Push(2);
            
            F.Push(0);
            F.Push(2);
            F.Push(3);
            
            F.Push(3);
            F.Push(2);
            F.Push(1);
            
            F.Push(1);
            F.Push(0);
            F.Push(3);
            
            Variant T = TriMesh_Make(V, F);
            tets_out.Push(T);
            
        }
        
        meshOut = TriMesh_Make(newVerts, faces);
        
        //manually append the tets since we don't have a tet mesh class yet
        VariantMap map = meshOut.GetVariantMap();
        map["tetrahedra"] = tets;
        map["tet_edges"] = edges;
        map["tet_meshes"] = tets_out;
        
        //set it again
        meshOut = map;
        
    }
    else {
        std::cout << "Mesh::tetrahedralize() FAILED to perform tetrahedralization!" << std::endl;
    }
    
    
    return success;
}

bool Geomlib::EdgeLatticeFromTetrahedra(const Urho3D::Variant & meshIn, Urho3D::VariantVector & latticeMeshes, float t)
{
	// make sure the input mesh has tetrahedra
	if (!TriMesh_Verify(meshIn)) {
		std::cerr << "ERROR: Geomlib:: meshIn unverified\n";
		latticeMeshes = VariantVector();
		return false;
	}
	VariantMap mesh = meshIn.GetVariantMap();
	Variant tet_var = mesh["tetrahedra"];
	if (tet_var.GetType() != Urho3D::VAR_VARIANTVECTOR) {
		latticeMeshes = VariantVector();
		return false;
	}

	VariantVector tets = tet_var.GetVariantVector();
	VariantVector verts = TriMesh_GetVertexList(mesh);

	// we are going to frame each face of each tetrahedron, then connect the frames. 
	// each tet will have 12 vertices, none of which are the original. 
	// each tet will have 16 faces
	for (int i = 0; i < tets.Size(); i += 4) {
		// get the original verts
		Vector3 A = verts[tets[i].GetInt()].GetVector3();
		Vector3 B = verts[tets[i+1].GetInt()].GetVector3();
		Vector3 C = verts[tets[i+2].GetInt()].GetVector3();
		Vector3 D = verts[tets[i+3].GetInt()].GetVector3();

		VariantVector vlist, flist;

		// create the new verts
		// incenters
		Vector3 I_ABC = Geomlib::Incenter(A, B, C);
		Vector3 I_ACD = Geomlib::Incenter(A, C, D);
		Vector3 I_DCB = Geomlib::Incenter(D, C, B);
		Vector3 I_BAD = Geomlib::Incenter(B, A, D);

		Vector3 A_ABC = A + t * (I_ABC - A);
		Vector3 A_ACD = A + t * (I_ACD - A);
		Vector3 A_BAD = A + t * (I_BAD - A);

		Vector3 B_ABC = B + t * (I_ABC - B);
		Vector3 B_DCB = B + t * (I_DCB - B);
		Vector3 B_BAD = B + t * (I_BAD - B);

		Vector3 C_ABC = C + t * (I_ABC - C);
		Vector3 C_DCB = C + t * (I_DCB - C);
		Vector3 C_ACD = C + t * (I_ACD - C);

		Vector3 D_BAD = D + t * (I_BAD - D);
		Vector3 D_DCB = D + t * (I_DCB - D);
		Vector3 D_ACD = D + t * (I_ACD - D);

		vlist.Push(A_ABC); //0
		vlist.Push(A_ACD); //1
		vlist.Push(A_BAD); //2

		vlist.Push(B_ABC); //3
		vlist.Push(B_DCB); //4
		vlist.Push(B_BAD); //5

		vlist.Push(C_ABC); //6
		vlist.Push(C_ACD); //7
		vlist.Push(C_DCB); //8

		vlist.Push(D_ACD); //9
		vlist.Push(D_BAD); //10
		vlist.Push(D_DCB); //11

		//create the faces 
		// 12 edge faces + 4 triangles

		// tris
		// 0 1 2
		//

		//AB 0 3 5 2
		flist.Push(0);
		flist.Push(3);
		flist.Push(5);
		flist.Push(0);
		flist.Push(5);
		flist.Push(2);

		//AC 1 7 6 0
		flist.Push(1);
		flist.Push(7);
		flist.Push(6);
		flist.Push(1);
		flist.Push(6);
		flist.Push(0);

		//DA  9 1 2 10
		flist.Push(9);
		flist.Push(1);
		flist.Push(2);
		flist.Push(9);
		flist.Push(2);
		flist.Push(10);

		//BC 3 6 8 4
		flist.Push(3);
		flist.Push(6);
		flist.Push(8);
		flist.Push(3);
		flist.Push(8);
		flist.Push(4);

		//BD 4 11 10 5
		flist.Push(4);
		flist.Push(11);
		flist.Push(10);
		flist.Push(4);
		flist.Push(10);
		flist.Push(5);

		//CD 7 9 11 8
		flist.Push(7);
		flist.Push(9);
		flist.Push(11);
		flist.Push(7);
		flist.Push(11);
		flist.Push(8);

		// tris
		flist.Push(0);
		flist.Push(1);
		flist.Push(2);
		flist.Push(3);
		flist.Push(4);
		flist.Push(5);
		flist.Push(6);
		flist.Push(7);
		flist.Push(8);
		flist.Push(9);
		flist.Push(10);
		flist.Push(11);

		Variant tet_lattice = TriMesh_Make(vlist, flist);
		latticeMeshes.Push(tet_lattice);
	}


	return true;
}

bool Geomlib::E0E1E4LatticeFromTetrahedra(const Urho3D::Variant & meshIn, Urho3D::VariantVector & latticeMeshes, Urho3D::VariantVector& polylines, float t)
{
	// make sure the input mesh has tetrahedra
	if (!TriMesh_Verify(meshIn)) {
		std::cerr << "ERROR: Geomlib:: meshIn unverified\n";
		latticeMeshes = VariantVector();
		return false;
	}
	VariantMap mesh = meshIn.GetVariantMap();
	Variant tet_var = mesh["tetrahedra"];
	if (tet_var.GetType() != Urho3D::VAR_VARIANTVECTOR) {
		latticeMeshes = VariantVector();
		return false;
	}

	VariantVector tets = tet_var.GetVariantVector();
	VariantVector verts = TriMesh_GetVertexList(mesh);

	for (int i = 0; i < tets.Size(); i += 4) {
		// verts
		Vector3 v_0 = verts[tets[i].GetInt()].GetVector3();
		Vector3 v_3 = verts[tets[i + 1].GetInt()].GetVector3();
		Vector3 v_2 = verts[tets[i + 2].GetInt()].GetVector3();
		Vector3 v_1 = verts[tets[i + 3].GetInt()].GetVector3();

		// edge verts, setting this random convention
		// E_0 = [0,1]
		// E_1 = [1,2]
		// E_4 = [0,4]
		float t2 = t / 2.0;

		Vector3 E_0_d = v_0 + (v_1 - v_0) / 2 - t2*(v_1 - v_0); //0
		Vector3 E_0_u = v_0 + (v_1 - v_0) / 2 + t2*(v_1 - v_0); //1
		Vector3 E_0 = v_0 + (v_1 - v_0) / 2;
												 
		Vector3 E_1_u = v_1 + (v_2 - v_1) / 2 - t2*(v_2 - v_1); //2
		Vector3 E_1_d = v_1 + (v_2 - v_1) / 2 + t2*(v_2 - v_1); //3
		Vector3 E_1 = v_1 + (v_2 - v_1) / 2;
												 
		Vector3 E_4_d = v_0 + (v_3 - v_0) / 2 - t2*(v_3 - v_0); //4
		Vector3 E_4_u = v_0 + (v_3 - v_0) / 2 + t2*(v_3 - v_0); //5
		Vector3 E_4 = v_0 + (v_3 - v_0) / 2;

		VariantVector vlist, flist;
		VariantVector plist; // for making a polyline.

		plist.Push(E_0);
		plist.Push(E_1);
		plist.Push(E_4);

		vlist.Push(E_0_d);
		vlist.Push(E_0_u);
		vlist.Push(E_1_d);
		vlist.Push(E_1_u);
		vlist.Push(E_4_d);
		vlist.Push(E_4_u);

		flist.Push(0);
		flist.Push(1);
		flist.Push(3);
		flist.Push(0);
		flist.Push(3);
		flist.Push(2);

		flist.Push(2);
		flist.Push(3);
		flist.Push(5);
		flist.Push(2);
		flist.Push(5);
		flist.Push(4);


		Variant tet_lattice = TriMesh_Make(vlist, flist);
		latticeMeshes.Push(tet_lattice);

		Variant poly = Polyline_Make(plist);
		polylines.Push(poly);

	}

	return true;
}
