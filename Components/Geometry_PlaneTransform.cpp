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


#include "Geometry_PlaneTransform.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Plane.h>

using namespace Urho3D;

String Geometry_PlaneTransform::iconTexture = "Textures/Icons/Geometry_PlaneTransform.png";


Geometry_PlaneTransform::Geometry_PlaneTransform(Urho3D::Context* context) : IoComponentBase(context, 2, 1)
{
    SetName("PlaneTransform");
    SetFullName("ConstructPlaneTransform");
    SetDescription("Construct Transform from Point and Normal");
    SetGroup(IoComponentGroup::TRANSFORM);
    SetSubgroup("Matrix");
    
    Variant y_unit = Vector3(0.0f, 1.0f, 0.0f);
    Variant default_rot = Matrix3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    
    inputSlots_[0]->SetName("Point");
    inputSlots_[0]->SetVariableName("P");
    inputSlots_[0]->SetDescription("Point on plane");
    inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
    inputSlots_[0]->SetDefaultValue(Vector3::ZERO);
    inputSlots_[0]->DefaultSet();
    
    inputSlots_[1]->SetName("Normal");
    inputSlots_[1]->SetVariableName("N");
    inputSlots_[1]->SetDescription("Plane Normal");
    inputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
    inputSlots_[1]->SetDefaultValue(Vector3(0,1,0));
    inputSlots_[1]->DefaultSet();
    
    outputSlots_[0]->SetName("Transform");
    outputSlots_[0]->SetVariableName("T");
    outputSlots_[0]->SetDescription("Plane transform out");
    outputSlots_[0]->SetVariantType(VariantType::VAR_MATRIX3X4);
    outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Geometry_PlaneTransform::SolveInstance(
                                        const Vector<Variant>& inSolveInstance,
                                        Vector<Variant>& outSolveInstance
                                        )
{
    
    // Verify slot 0
    if (inSolveInstance[0].GetType() != VariantType::VAR_VECTOR3) {
        URHO3D_LOGWARNING("P must be type Vector3.");
        outSolveInstance[0] = Variant();
        return;
    }
    
    // Verify slot 0
    if (inSolveInstance[1].GetType() != VariantType::VAR_VECTOR3) {
        URHO3D_LOGWARNING("N must be type Vector3.");
        outSolveInstance[0] = Variant();
        return;
    }
    
    Vector3 P = inSolveInstance[0].GetVector3();
    Vector3 N = inSolveInstance[1].GetVector3();
    
    Urho3D::Plane refl_plane(N, P);
    
    Urho3D::Matrix3x4 transform = Matrix3x4::IDENTITY;
    transform.SetTranslation(P);
    Urho3D::Quaternion rotation= Quaternion(Urho3D::Vector3::UP, N);
    transform.SetRotation(rotation.RotationMatrix());
    
    
    /////////////////
    // ASSIGN OUTPUTS
    
    //Quaternion rot(rotationMatrix_out);
    outSolveInstance[0] = transform;
}
