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


#include "Mesh_SuperEllipsoid.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ConversionUtilities.h"
#include "StockGeometries.h"
#include "Geomlib_TransformVertexList.h"
#include "TriMesh.h"
#include "NMesh.h"
#include "Geomlib_TriMeshThicken.h"

using namespace Urho3D;

String Mesh_SuperEllipsoid::iconTexture = "Textures/Icons/Mesh_SuperEllipsoid.png";

Mesh_SuperEllipsoid::Mesh_SuperEllipsoid(Context* context) : IoComponentBase(context, 6, 2)
{
    SetName("SuperEllipsoid");
    SetFullName("ConstructSuperEllipsoidMesh");
    SetDescription("Construct an ellipsoid mesh from params");
    SetGroup(IoComponentGroup::MESH);
    SetSubgroup("Primitive");
    
    inputSlots_[0]->SetName("X");
    inputSlots_[0]->SetVariableName("X");
    inputSlots_[0]->SetDescription("X scale (> 0)");
    inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
    inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    inputSlots_[0]->SetDefaultValue(Variant(3.0f));
    inputSlots_[0]->DefaultSet();
    
    inputSlots_[1]->SetName("Y");
    inputSlots_[1]->SetVariableName("Y");
    inputSlots_[1]->SetDescription("Y scale (> 0)");
    inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
    inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
    inputSlots_[1]->SetDefaultValue(Variant(3.0f));
    inputSlots_[1]->DefaultSet();
    
    inputSlots_[2]->SetName("Z");
    inputSlots_[2]->SetVariableName("Z");
    inputSlots_[2]->SetDescription("Z scale (> 0)");
    inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
    inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
    inputSlots_[2]->SetDefaultValue(Variant(4.0f));
    inputSlots_[2]->DefaultSet();
    
    inputSlots_[3]->SetName("FirstPower");
    inputSlots_[3]->SetVariableName("r");
    inputSlots_[3]->SetDescription("FirstPower (0.2 < r < 4)");
    inputSlots_[3]->SetVariantType(VariantType::VAR_FLOAT);
    inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
    inputSlots_[3]->SetDefaultValue(Variant(2.0f));
    inputSlots_[3]->DefaultSet();
    
    inputSlots_[4]->SetName("SecondPower");
    inputSlots_[4]->SetVariableName("t");
    inputSlots_[4]->SetDescription("SecondPower (0.2 < t < 4)");
    inputSlots_[4]->SetVariantType(VariantType::VAR_FLOAT);
    inputSlots_[4]->SetDataAccess(DataAccess::ITEM);
    inputSlots_[4]->SetDefaultValue(Variant(2.5f));
    inputSlots_[4]->DefaultSet();
    
    inputSlots_[5]->SetName("MeshResolution");
    inputSlots_[5]->SetVariableName("R");
    inputSlots_[5]->SetDescription("Integer (>3) describing mesh resolution");
    inputSlots_[5]->SetVariantType(VariantType::VAR_INT);
    inputSlots_[5]->SetDefaultValue(Variant(10));
    inputSlots_[5]->DefaultSet();
    
    //inputSlots_[4]->SetName("Transformation");
    //inputSlots_[4]->SetVariableName("T");
    //inputSlots_[4]->SetDescription("Transformation to apply to cube");
    //inputSlots_[4]->SetVariantType(VariantType::VAR_MATRIX3X4);
    //inputSlots_[4]->SetDefaultValue(Matrix3x4::IDENTITY);
    //inputSlots_[4]->DefaultSet();
    
    outputSlots_[0]->SetName("TriMesh");
    outputSlots_[0]->SetVariableName("M");
    outputSlots_[0]->SetDescription("SuperEllipsoid TriMesh");
    outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
    outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    
    outputSlots_[1]->SetName("QuadMesh");
    outputSlots_[1]->SetVariableName("Q");
    outputSlots_[1]->SetDescription("SuperEllipsoid QuadMesh");
    outputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
    outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_SuperEllipsoid::SolveInstance(
                               const Vector<Variant>& inSolveInstance,
                               Vector<Variant>& outSolveInstance
                               )
{
    assert(inSolveInstance.Size() == inputSlots_.Size());
    assert(outSolveInstance.Size() == outputSlots_.Size());
    
    ///////////////////
    // VERIFY & EXTRACT
    
    // Verify input slot 0
    VariantType type0 = inSolveInstance[0].GetType();
    if (!(type0 == VariantType::VAR_FLOAT || type0 == VariantType::VAR_INT)) {
        URHO3D_LOGWARNING("A must be a valid float or integer.");
        outSolveInstance[0] = Variant();
        return;
    }
    float A = inSolveInstance[0].GetFloat();
    if (A <= 0.0f) {
        URHO3D_LOGWARNING("A must be > 0.");
        outSolveInstance[0] = Variant();
        return;
    }
    
    //verify input slot 1
    VariantType type1 = inSolveInstance[1].GetType();
    if (!(type1 == VariantType::VAR_FLOAT || type1 == VariantType::VAR_INT)) {
        URHO3D_LOGWARNING("B must be a valid float or integer.");
        outSolveInstance[0] = Variant();
        return;
    }
    float B = inSolveInstance[1].GetFloat();
    if (B <= 0.0f) {
        URHO3D_LOGWARNING("B must be > 0.");
        outSolveInstance[0] = Variant();
        return;
    }
    
    // Verify input slot 2
    VariantType type2 = inSolveInstance[2].GetType();
    if (!(type2 == VariantType::VAR_INT || type2 == VariantType::VAR_FLOAT)) {
        URHO3D_LOGWARNING("C must be a valid integer or float.");
        outSolveInstance[0] = Variant();
        return;
    }
    float C = inSolveInstance[2].GetFloat();
    if (C <= 0.0f ) {
        URHO3D_LOGWARNING("C must be > 0.");
        outSolveInstance[0] = Variant();
        return;
    }
    
    // Verify input slot 3
    VariantType type3 = inSolveInstance[3].GetType();
    if (!(type3 == VariantType::VAR_INT || type3 == VariantType::VAR_FLOAT)) {
        URHO3D_LOGWARNING("First power must be a valid integer or float.");
        outSolveInstance[0] = Variant();
        return;
    }
    float r = inSolveInstance[3].GetFloat();
    if (r <= 0.2f || r > 4.0f) {
        URHO3D_LOGWARNING("First power must be between 0.2 and 4.0");
        outSolveInstance[0] = Variant();
        return;
    }
    
    // Verify input slot 4
    VariantType type4 = inSolveInstance[4].GetType();
    if (!(type4 == VariantType::VAR_INT || type4 == VariantType::VAR_FLOAT)) {
        URHO3D_LOGWARNING("Second power must be a valid integer or float.");
        outSolveInstance[0] = Variant();
        return;
    }
    float t = inSolveInstance[4].GetFloat();
    if (t <= 0.2f || t > 4.0f) {
        URHO3D_LOGWARNING("Second power must be between 0.2 and 4.0");
        outSolveInstance[0] = Variant();
        return;
    }

    
    // Verify input slot 5
    int res = inSolveInstance[5].GetInt();
    if (res < 3) {
        //URHO3D_LOGWARNING("R must be larger than 3");
        //outSolveInstance[0] = Variant();
        //return;
		res = 3;
    }
    
    ///////////////////
    // COMPONENT'S WORK
    
    Variant ellipsoidTriMesh;
   // MakeSuperEllipsoid(torusTriMesh, 3.0, 3.0, 4.0, 2.0, 2.5, 35);
    Variant ellipsoidQuadMesh = MakeSuperEllipsoid(ellipsoidTriMesh, A, B, C, r, t, res);
    
    /////////////////
    // ASSIGN OUTPUTS
    
    outSolveInstance[0] = ellipsoidTriMesh;
    outSolveInstance[1] = ellipsoidQuadMesh;
}

int Mesh_SuperEllipsoid::Sgn(float x)
{
    if (x < 0)
        return -1;
    else if (x == 0.0)
        return 0;
    else if (x > 0)
        return 1;
    else
        return 2000;
}

float Mesh_SuperEllipsoid::c(float w, float m)
{
    return Sgn(cos(w))*std::pow(std::abs(cos(w)),m);
}

float Mesh_SuperEllipsoid::s(float w, float m)
{
    return Sgn(sin(w))*std::pow(std::abs(sin(w)),m);
}

//bool MakeSuperEllipsoid(Urho3D::Variant& triMesh, float outer_radius, float inner_radius, float first_power, float second_power, int res)
Urho3D::Variant Mesh_SuperEllipsoid::MakeSuperEllipsoid(Urho3D::Variant& triMesh, float A, float B, float C, float r, float t, int res)
{
    // regard the torus as a rectangle, with opposite sides identified.
    // res gives the number of divisions in each direction. So the rectangle will be divded into res*res quads
    // to create the vertex list, we create the vertices row by row.
    // example: res = 4
    // v_03, v_13, v_23, v_33
    // v_02, v_12, v_22, v_32
    // v_01, v_11, v_21, v_31
    // v_00, v_10, v_20, v_30
    
    
    // so the translation to vertex IDs is
    // 12 13 14 15
    // 8 9 10 11
    // 4 5 6 7
    // 0 1 2 3
    
    
    // the faces may then be created from this list.
    // see http://paulbourke.net/geometry/torus/source2.c
    
    float DTOR = 0.01745329252;
    
    // make the vertex list
    VariantVector vertex_list;
    
    float dv = 180.0f / res;
    float du = 360.0f / res;
    
    for (int u = 0; u < res+1; ++u) {
        for (int v = 0; v < res+1; ++v) {
            float theta = (-90 +v*dv)*DTOR;
            float phi = (-180+u*du)*DTOR;
            
            float C_theta_t = c(theta, 2/t);
            float C_phi_r = c(phi, 2/r);
            float S_phi_r = s(phi, 2/r);
            float S_theta_t = s(theta, 2/t);
            
            Vector3 curVert;
            curVert.x_ = A*C_theta_t*C_phi_r;
            curVert.y_ = B*C_theta_t*S_phi_r;
            curVert.z_ = C*S_theta_t;
            vertex_list.Push(Variant(curVert));
        }
    }
    
    VariantVector face_list;
    VariantVector tri_face_list;
    
    int mod = (res+1)*(res+1);
    // make the face list
    for (int i = 0; i < res+1; ++i) {
        for (int j = 0; j < res; ++j) {
            // all faces are quads:
            face_list.Push(4);
            
            int lower_left = res*i + j;
            int lower_right = res*i + ((j + 1));
            int upper_right = (res*(i + 1) + ((j + 1))+1)%(mod);
            int upper_left = (res*(i + 1)  + j+1)%mod;

            // making quad face
            face_list.Push(upper_left);
            face_list.Push(upper_right);
            face_list.Push(lower_right);
            face_list.Push(lower_left);

            
            // making tri_face 1
            tri_face_list.Push(upper_right);
            tri_face_list.Push(lower_right);
            tri_face_list.Push(lower_left);

            
            // making tri_face 2
            tri_face_list.Push(upper_left);
            tri_face_list.Push(upper_right);
            tri_face_list.Push(lower_left);

            
        }
    }
    
    triMesh = TriMesh_Make(vertex_list, tri_face_list);
    
    Urho3D::Variant superEllipsoid = NMesh_Make(vertex_list, face_list);
    
    return superEllipsoid;
    //return torus;
}


