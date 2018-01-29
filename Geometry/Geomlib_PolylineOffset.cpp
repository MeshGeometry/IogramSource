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


#include "Geomlib_PolylineOffset.h"
#include "Geomlib_TransformVertexList.h"
#include "Polyline.h"

using Urho3D::Variant;
using Urho3D::VariantVector;
using Urho3D::Vector3;
using Urho3D::Quaternion;

bool Geomlib::PolylineOffset(
                             const Urho3D::Variant& polyIn,
                             Urho3D::Variant& polyOut,
                             float offset
                             )
{
    if (!Polyline_Verify(polyIn))
    {
        return false;
    }

    const VariantVector verts = Polyline_ComputeSequentialVertexList(polyIn);

    VariantVector offsetVerts;

    // Check if polyline is closed
    bool isClosed = false;
    if (verts[0].GetVector3() == verts[verts.Size() - 1].GetVector3())
        isClosed = true;

    // we'll treat the endpoints using the offset vectors for the penultimate vertices on either end
    Vector3 startVec;
    Vector3 endVec;

    int vertCount = verts.Size();

    // threat this as the point we repeat at each vert
    // see polyline pipe
    Vector3 v0(offset, 0.0f, 0.0f);
    VariantVector baseVerts;
    baseVerts.Push(v0);
    Urho3D::Matrix3x4 tr = Urho3D::Matrix3x4::IDENTITY;
    VariantVector newVerts = Geomlib::TransformVertexList(tr, baseVerts);
    Urho3D::Quaternion polyQ;
    polyQ.FromRotationTo(Vector3(0, 0, 1), Vector3(0, 1, 0));
    tr.SetRotation(polyQ.RotationMatrix());
    //newVerts = Geomlib::TransformVertexList(tr, newVerts);
    Vector3 newVert = tr*v0;

    //now loop over all the verts and construct transform
    // add transformed basePoly to list of sections, then loft them
    VariantVector sections;

    for (int i = 0; i < vertCount; ++i)
    {

        Vector3 start = verts[i].GetVector3();

        tr.SetTranslation(start);

        Quaternion rot;
        Vector3 dir;
        if (i == 0) {// endpoint
            dir = verts[i + 1].GetVector3() - start;
            rot.FromLookRotation(dir, Vector3::UP);

        }
        else if (i > 0 && i < vertCount - 1) { // interior
            Vector3 A = start - verts[i - 1].GetVector3();
            Vector3 B = start - verts[i + 1].GetVector3();
            Vector3 C = A.CrossProduct(B);
            Vector3 bisector = (A + B).Normalized();
            dir = bisector.CrossProduct(C);

            rot.FromLookRotation(dir, Vector3::UP);

        }
        else if (i == vertCount - 1) {
            dir = (start - verts[vertCount - 2].GetVector3());
            rot.FromLookRotation(dir, Vector3::UP);
        }

        tr.SetRotation(rot.RotationMatrix());

        Vector3 curPoint = tr*newVert;

        offsetVerts.Push(curPoint);

    }


    polyOut = Polyline_Make(offsetVerts);
    return true;
}

//
//
//bool Geomlib::PolylineOffset(
//    const Urho3D::Variant& polyIn,
//    Urho3D::Variant& polyOut,
//    float offset
//    )
//{
//    if (!Polyline_Verify(polyIn))
//    {
//        return false;
//    }
//
//    const VariantVector verts = Polyline_ComputeSequentialVertexList(polyIn);
//
//    VariantVector offsetVerts;
//
//    // Check if polyline is closed
//    bool isClosed = false;
//    if (verts[0].GetVector3() == verts[verts.Size() - 1].GetVector3())
//        isClosed = true;
//
//    // we'll treat the endpoints using the offset vectors for the penultimate vertices on either end
//    Vector3 startVec;
//    Vector3 endVec;
//    // for now, push a placeholder for the first vertex on to the vector
//    offsetVerts.Push(Vector3::ZERO);
//    int vertCount = verts.Size();
//    Vector3 prev_offsetVec;
//    for (int i = 1; i < vertCount - 1; ++i)
//    {
//        // the vertex to offset
//        Vector3 curVert = verts[i].GetVector3();
//
//        // get the vectors defining the segments adjacent to the current vert
//        Vector3 vecA = curVert - verts[i - 1].GetVector3();
//        Vector3 vecB = curVert - verts[i + 1].GetVector3();
//
//        // find the bisecting vector and normalize for offset direction vector
//        Vector3 offsetVec = vecA + vecB;
//        // save the vectors for the start and end verts
//        offsetVec.Normalize();
//        if (i == 1)
//            startVec = offsetVec;
//        if (i == (vertCount - 2))
//            endVec = offsetVec;
//
//        // try to ensure that the offset vectors don't cross the polyline
//        if (i == 1)
//            prev_offsetVec = offsetVec;
//        else{
//            if (offsetVec.DotProduct(prev_offsetVec) < 0)
//                offsetVec *= -1;
//            prev_offsetVec = offsetVec;
//        }
//
//        // actually offset the current vertex and save to new vertex list
//        Vector3 offVert = curVert + offset*offsetVec;
//        offsetVerts.Push(offVert);
//    }
//
//    // Now treat the end points, depending on whether the polyline is closed or not.
//    if (isClosed && (vertCount >= 4)) {
//
//        Vector3 curVert = verts[0].GetVector3();
//
//        Vector3 vecA = curVert - verts[1].GetVector3();
//        Vector3 vecB = curVert - verts[vertCount - 2].GetVector3();
//        Vector3 offsetVec = vecA + vecB;
//        offsetVec.Normalize();
//
//        Vector3 endOffsetVert = curVert + offset * offsetVec;
//        offsetVerts[0] = endOffsetVert; //start vert
//        offsetVerts.Push(endOffsetVert); //end vert
//    }
//    else {
//        // start vert
//        Vector3 startOffsetVert = verts[0].GetVector3() + offset*startVec;
//        offsetVerts[0] = startOffsetVert;
//
//        //end vert
//        Vector3 endOffsetVert = verts[vertCount - 1].GetVector3();
//        offsetVerts.Push(endOffsetVert + offset*endVec);
//    }
//
//    polyOut = Polyline_Make(offsetVerts);
//    return true;
//}
//
