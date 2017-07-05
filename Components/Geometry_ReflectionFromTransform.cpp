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


#include "Geometry_ReflectionFromTransform.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Plane.h>

using namespace Urho3D;

String Geometry_ReflectionFromTransform::iconTexture = "Textures/Icons/Geometry_ReflectionFromTransform.png";


Geometry_ReflectionFromTransform::Geometry_ReflectionFromTransform(Urho3D::Context* context) : IoComponentBase(context, 1, 1)
{
    SetName("ConstructReflection");
    SetFullName("Construct Reflection Transformation");
    SetDescription("Construct a reflection transformation from plane");
    SetGroup(IoComponentGroup::TRANSFORM);
    SetSubgroup("Matrix");
    
    Variant y_unit = Vector3(0.0f, 1.0f, 0.0f);
    Variant default_rot = Matrix3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    
    inputSlots_[0]->SetName("Transform");
    inputSlots_[0]->SetVariableName("T");
    inputSlots_[0]->SetDescription("Transformation representing plane");
    inputSlots_[0]->SetVariantType(VariantType::VAR_MATRIX3X4);
    inputSlots_[0]->SetDefaultValue(Matrix3x4::IDENTITY);
    inputSlots_[0]->DefaultSet();
    
    
    outputSlots_[0]->SetName("Rotation Transform");
    outputSlots_[0]->SetVariableName("T");
    outputSlots_[0]->SetDescription("Reflection transformation out");
    outputSlots_[0]->SetVariantType(VariantType::VAR_MATRIX3X4);
    outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Geometry_ReflectionFromTransform::SolveInstance(
                                        const Vector<Variant>& inSolveInstance,
                                        Vector<Variant>& outSolveInstance
                                        )
{
    
    // Verify slot 0
    if (inSolveInstance[0].GetType() != VariantType::VAR_MATRIX3X4) {
        URHO3D_LOGWARNING("T must be type Matrix3X4.");
        outSolveInstance[0] = Variant();
        return;
    }
    
    

    
    Urho3D::Matrix3x4 transform = inSolveInstance[0].GetMatrix3x4();
    Vector3 P = transform.Translation();
    Vector3 N = transform.RotationMatrix()*Vector3::UP;
    
    Urho3D::Plane refl_plane(N, P);
    
    Urho3D::Matrix3x4 reflection = refl_plane.ReflectionMatrix();	
    
    
    /////////////////
    // ASSIGN OUTPUTS
    
    //Quaternion rot(rotationMatrix_out);
    outSolveInstance[0] = reflection;
}
