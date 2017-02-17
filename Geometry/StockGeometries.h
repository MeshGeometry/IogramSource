#pragma once

#include <Urho3D/Core/Variant.h>

Urho3D::Variant MakeHexayurtMesh(float s);

Urho3D::Variant MakeCubeMesh(float s);

Urho3D::Variant MakeZigZag(float s, int n);
bool MakeZigZag(float s, float t, int n, Urho3D::Variant& zigZagPolyline);


Urho3D::Variant MakeIcosahedron();
Urho3D::Variant MakeSphere();
Urho3D::Variant MakeRegularPolygon(int n);

Urho3D::Variant MakeSuperTorus(float outer_radius, float inner_radius, float first_power, float second_power, int res);