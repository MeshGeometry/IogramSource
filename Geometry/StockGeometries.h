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


#pragma once

#include <Urho3D/Core/Variant.h>

Urho3D::Variant MakeHexayurtMesh(float s);

Urho3D::Variant MakeCubeMesh(float s);

Urho3D::Variant MakeZigZag(float s, int n);
bool MakeZigZag(float s, float t, int n, Urho3D::Variant& zigZagPolyline);


Urho3D::Variant MakeIcosahedron();
Urho3D::Variant MakeSphere();
Urho3D::Variant MakeRegularPolygon(int n);

Urho3D::Variant MakeSuperTorus(Urho3D::Variant& triMesh,
                               float outer_radius, float inner_radius, float first_power, float second_power, int res);

Urho3D::Variant MakePlane(float y_coord);