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

namespace Geomlib {
	bool MeshTetrahedralize(
		const Urho3D::Variant& meshIn,
		float maxVolume,
		Urho3D::Variant& meshOut
	);
    
    bool MeshTetrahedralizeFromPoints(
        const Urho3D::Variant& meshIn,
        const Urho3D::VariantVector& pointsIn,
        float maxVolume,
        Urho3D::Variant& meshOut
        );

	bool EdgeLatticeFromTetrahedra(
		const Urho3D::Variant& meshIn, // mesh with tetrahedra
		Urho3D::VariantVector& latticeMeshes,
		float t // inset
	);

	// The E0-E1 E1-E4 lattice from 
	// Elastic Textures for Additive Fabrication

	bool E0E1E4LatticeFromTetrahedra(
		const Urho3D::Variant& meshIn, // mesh with tetrahedra
		Urho3D::VariantVector& latticeMeshes,
		Urho3D::VariantVector& polylines,
		float t // inset
	);
}
