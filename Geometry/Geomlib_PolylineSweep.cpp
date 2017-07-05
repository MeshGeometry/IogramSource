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


#include "Geomlib_PolylineSweep.h"

#include <iostream>

#include "Geomlib_PolylineRefine.h"
#include "Geomlib_PolylineLoft.h"
#include "Geomlib_ConstructTransform.h"
#include "Geomlib_TransformVertexList.h"
#include "Geomlib_PolylineDivide.h"
#include "TriMesh.h"
#include "Polyline.h"


namespace {
    
    using Urho3D::Vector;
    using Urho3D::Variant;
    using Urho3D::VariantVector;
    using Urho3D::Vector3;
    using Urho3D::Equals;
    
    bool Vector3Equals(const Vector3& lhs, const Vector3& rhs)
    {
        return Equals(lhs.x_, rhs.x_) && Equals(lhs.y_, rhs.y_) && Equals(lhs.z_, rhs.z_);
    }
    
    Vector<Vector3> RemoveRepeats(const Vector<Vector3>& vertexList)
    {
        if (vertexList.Size() <= 1) {
            return vertexList;
        }
        
        Vector<Vector3> newVertexList;
        newVertexList.Push(vertexList[0]);
        for (unsigned i = 0; i < vertexList.Size() - 1; ++i) {
            if (!Vector3Equals(vertexList[i], vertexList[i + 1])) {
                newVertexList.Push(vertexList[i + 1]);
            }
        }
        
        return newVertexList;
    }
    
    Vector<float> ComputePolylineParams(const VariantVector& vertexList)
    {
        float length = 0.0f;
        // loop over each edge [i, i + 1]
        for (int i = 0; i < vertexList.Size() - 1; ++i) {
            Vector3 seg = vertexList[i + 1].GetVector3() - vertexList[i].GetVector3();
            length += seg.Length();
        }
        if (Urho3D::Equals(length, 0.0f)) {
            // give up!
            return Vector<float>();
        }
        
        Vector<float> dists;
        
        float sublength = 0.0f;
        dists.Push(sublength);
        for (unsigned i = 0; i < vertexList.Size() - 1; ++i) {
            Vector3 seg = vertexList[i + 1].GetVector3() - vertexList[i].GetVector3();
            sublength += seg.Length();
            dists.Push(sublength);
        }
        
        Vector<float> ret;
        for (unsigned i = 0; i < dists.Size(); ++i) {
            ret.Push(dists[i] / length);
        }
        return ret;
    }
    
} // namespace

bool Geomlib::PolylineSweep_single(
                          const Urho3D::Variant& rail,
                          const Urho3D::Variant& section,
                          Urho3D::Variant& mesh
                          )
{
        using Urho3D::Variant;
        using Urho3D::VariantType;
        using Urho3D::VariantMap;
        using Urho3D::VariantVector;
        using Urho3D::Vector;
        using Urho3D::Vector3;
    
        /////////////////////////
        // VERIFY INPUT POLYLINES
    
        if (!Polyline_Verify(rail)) {
            mesh = Variant();
            return false;
        }
        if (!Polyline_Verify(section)) {
            mesh = Variant();
            return false;
        }
    
        //////////////////
        // CLEAN & EXTRACT
    
        Variant cleanRail = Polyline_Clean(rail);
        VariantVector cleanRailVerts = Polyline_ComputeSequentialVertexList(rail);
    
        Variant cleanSection = Polyline_Clean(section);
        VariantVector cleanSectionVerts = Polyline_ComputeSequentialVertexList(section);
    Vector3 sectionStart = cleanSectionVerts[0].GetVector3();
    
    Vector<Variant> sections;
    // iterate through vertices of the rail curve, create affine transformation of section polyline
    for (int i = 0; i < cleanRailVerts.Size(); ++i){
        Vector3 targetVert = cleanRailVerts[i].GetVector3();
        Vector3 translation = targetVert - sectionStart;
        
        Urho3D::Matrix3x4 curr_trans = Geomlib::ConstructTransform(translation);
        
        Vector<Variant> newVerts = Geomlib::TransformVertexList(curr_trans, cleanSectionVerts);
        Variant TranslatedSection = Polyline_Make(newVerts);
        sections.Push(TranslatedSection);
    }
    Geomlib::PolylineLoft(sections, mesh);
    
    return true;
}

bool Geomlib::PolylineSweep_double(
                          const Urho3D::Variant& rail1,
                          const Urho3D::Variant& rail2,
                          const Urho3D::Variant& section,
                          Urho3D::Variant& mesh
                          )
{
    using Urho3D::Variant;
    using Urho3D::VariantType;
    using Urho3D::VariantMap;
    using Urho3D::VariantVector;
    using Urho3D::Vector;
    using Urho3D::Vector3;
    
    /////////////////////////
    // VERIFY INPUT POLYLINES
    
    if (!Polyline_Verify(rail1)) {
        mesh = Variant();
        return false;
    }
    if (!Polyline_Verify(rail2)) {
        mesh = Variant();
        return false;
    }
    if (!Polyline_Verify(section)) {
        mesh = Variant();
        return false;
    }
    
    //////////////////
    // CLEAN & EXTRACT
    
    Variant cleanRail1 = Polyline_Clean(rail1);
    VariantVector cleanRail1Verts = Polyline_ComputeSequentialVertexList(rail1);
    
    Variant cleanRail2 = Polyline_Clean(rail2);
    VariantVector cleanRail2Verts = Polyline_ComputeSequentialVertexList(rail2);
    
    Variant cleanSection = Polyline_Clean(section);
    VariantVector cleanSectionVerts = Polyline_ComputeSequentialVertexList(section);
    Vector3 sectionStart = cleanSectionVerts[0].GetVector3();
    Vector3 sectionEnd;
   // float sectionScale =
    int sectionVerts = cleanSectionVerts.Size();
    bool section_is_closed = Polyline_IsClosed(section);
    if (!section_is_closed)
        sectionEnd = cleanSectionVerts[sectionVerts-1].GetVector3();
    else{
        int mid = (int)cleanSectionVerts.Size()/2;
		float res = cleanSectionVerts.Size() % 2;
		if (res == 1) {
			// odd number of verts
			mid += 1;
		}

        sectionEnd = cleanSectionVerts[mid].GetVector3();
    }
    
    Variant shortRail;
    Variant longRail;
    Variant matchedRail;
    
    shortRail = (cleanRail1Verts.Size() > cleanRail2Verts.Size()) ? cleanRail2 : cleanRail1;
    longRail = (cleanRail1Verts.Size() > cleanRail2Verts.Size()) ? cleanRail1 : cleanRail2;
    
    VariantVector longRailVerts = Polyline_ComputeSequentialVertexList(longRail);
    int n = longRailVerts.Size();
    
    Geomlib::PolylineDivide(shortRail, n-1, matchedRail);
    VariantVector matchedRailVerts = Polyline_ComputeSequentialVertexList(matchedRail);
    
    Vector<Variant> sections;
    // iterate through vertices of the rail curve, create affine transformation of section polyline
    if (!section_is_closed){
        for (int i = 0; i < longRailVerts.Size(); ++i){
            Vector3 targetStartVert = longRailVerts[i].GetVector3();
            Vector3 targetEndVert = matchedRailVerts[i].GetVector3();
        
            Vector3 translation_start = targetStartVert - sectionStart;
            Vector3 translation_end = targetEndVert - sectionEnd;
        
            Vector3 translation_diff = (translation_end - translation_start)/(float)sectionVerts;
            
            // now create the translated polyline
            Vector<Variant> transformedVerts;
            for (int j = 0; j < cleanSectionVerts.Size(); ++j){
                Urho3D::Matrix3x4 curr_trans = Geomlib::ConstructTransform(translation_start + j*translation_diff);
                Vector3 transVert = curr_trans*cleanSectionVerts[j].GetVector3();
                transformedVerts.Push(transVert);
            }
            Variant TranslatedSection = Polyline_Make(transformedVerts);
            sections.Push(TranslatedSection);
        }
    }
    else{
		// deal with the case that there are an odd number of vertices
		// sectionVerts_A >= sectionVerts_B
		int sectionVerts_B = sectionVerts / 2;
		int sectionVerts_A = sectionVerts - sectionVerts_B;

        for (int i = 0; i < longRailVerts.Size(); ++i){
            Vector3 targetStartVert = longRailVerts[i].GetVector3();
            Vector3 targetEndVert = matchedRailVerts[i].GetVector3();
            
            Vector3 translation_start = targetStartVert - sectionStart;
            Vector3 translation_end = targetEndVert - sectionEnd;
            
            Vector3 translation_diff_A = (translation_end - translation_start)/sectionVerts_A;
            Vector3 translation_diff_B = (translation_end - translation_start)/sectionVerts_B;
            
            // now create the translated polyline
            Vector<Variant> transformedVerts;
            for (int j = 0; j < sectionVerts_A; ++j){
                Urho3D::Matrix3x4 curr_trans = Geomlib::ConstructTransform(translation_start + j*translation_diff_A);
                Vector3 transVert = curr_trans*cleanSectionVerts[j].GetVector3();
                transformedVerts.Push(transVert);
            }
            for (int j = sectionVerts_A; j < cleanSectionVerts.Size(); ++j){
                Urho3D::Matrix3x4 curr_trans = Geomlib::ConstructTransform(translation_start + (sectionVerts_A -1 - (j- sectionVerts_B))*translation_diff_B);
                Vector3 transVert = curr_trans*cleanSectionVerts[j].GetVector3();
                transformedVerts.Push(transVert);
            }
            Variant TranslatedSection = Polyline_Make(transformedVerts);
            sections.Push(TranslatedSection);
        }
    }

    
    Geomlib::PolylineLoft(sections, mesh);
    
    return true;
}


//bool Geomlib::PolylineBlend(
//                            const Urho3D::Variant& polyline1,
//                            const Urho3D::Variant& polyline2,
//                            Urho3D::Variant& mesh
//                            )
//{
//    using Urho3D::Variant;
//    using Urho3D::VariantType;
//    using Urho3D::VariantMap;
//    using Urho3D::VariantVector;
//    using Urho3D::Vector;
//    using Urho3D::Vector3;
//    
//    /////////////////////////
//    // VERIFY INPUT POLYLINES
//    
//    if (!Polyline_Verify(polyline1)) {
//        mesh = Variant();
//        return false;
//    }
//    if (!Polyline_Verify(polyline2)) {
//        mesh = Variant();
//        return false;
//    }
//    
//    //////////////////
//    // CLEAN & EXTRACT
//    
//    Variant cleanPoly1 = Polyline_Clean(polyline1);
//    VariantVector cleanVerts1 = Polyline_ComputeSequentialVertexList(polyline1);
//    
//    Variant cleanPoly2 = Polyline_Clean(polyline2);
//    VariantVector cleanVerts2 = Polyline_ComputeSequentialVertexList(polyline2);
//    
//    Vector<float> params1 = ComputePolylineParams(cleanVerts1);
//    Vector<float> params2 = ComputePolylineParams(cleanVerts2);
//    
//    if (params1.Size() > 0 && params2.Size() > 0) {
//        Variant polyOut1;
//        bool success1 = PolylineRefine(cleanPoly1, params2, polyOut1);
//        if (!success1) {
//            mesh = Variant();
//            std::cerr << "ERROR: Geomlib::PolylineBlend --- PolylineRefine failed on polyline 1\n";
//            return false;
//        }
//        
//        Variant polyOut2;
//        bool success2 = PolylineRefine(cleanPoly2, params1, polyOut2);
//        if (!success2) {
//            mesh = Variant();
//            std::cerr << "ERROR: Geomlib::PolylineBlend --- PolylineRefine failed on polyline 2\n";
//            return false;
//        }
//        
//        VariantVector outVerts1 = Polyline_ComputeSequentialVertexList(polyOut1);
//        VariantVector outVerts2 = Polyline_ComputeSequentialVertexList(polyOut2);
//        
//        if (outVerts1.Size() == outVerts2.Size()) {
//            if (outVerts1.Size() <= 1) {
//                std::cerr << "ERROR: Geomlib::PolylineBlend --- polys have <= 1 verts after refinement\n";
//                mesh = Variant();
//                return false;
//            }
//            VariantVector meshVertList = outVerts1;
//            meshVertList.Push(outVerts2);
//            VariantVector meshFaceList;
//            unsigned N = outVerts1.Size();
//            
//            // Each pass processes edge [i, i + 1].
//            for (unsigned i = 0; i < outVerts1.Size() - 1; ++i) {
//                // first face
//                meshFaceList.Push(i);
//                meshFaceList.Push(i + 1);
//                meshFaceList.Push(N + i + 1);
//                // second face
//                meshFaceList.Push(N + i + 1);
//                meshFaceList.Push(N + i);
//                meshFaceList.Push(i);
//            }
//            
//            Variant meshVertices(meshVertList);
//            Variant meshFaces(meshFaceList);
//            mesh = TriMesh_Make(meshVertices, meshFaces);
//            return true;
//        }
//        else {
//            std::cerr << "ERROR: Geomlib::PolylineBlend --- polys have unequal vert num after refinement\n";
//            mesh = Variant();
//            return false;
//        }
//    }
//    
//    mesh = Variant();
//    std::cerr << "ERROR: Geomlib::PolylineBlend --- params1 or params2 has size 0\n";
//    return false;
//}
