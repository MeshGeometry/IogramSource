#include "Maths_RhodoLattice.h"

#include <assert.h>

#include <iostream>

#include <Urho3D/Core/Variant.h>

#include "Geomlib_Rhodo.h"
#include "Polyline.h"

using namespace Urho3D;

String Maths_RhodoLattice::iconTexture = "Textures/Icons/Maths_RhodoLattice.png";

Maths_RhodoLattice::Maths_RhodoLattice(Urho3D::Context* context) : IoComponentBase(context, 5, 2)
{
	SetName("RhodoLattice");
	SetFullName("Rhodo Lattice");
	SetDescription("Generates 3D lattice from rhombic dodecahedral tiling");
	SetGroup(IoComponentGroup::MATHS);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("transform");
	inputSlots_[0]->SetVariableName("T");
	inputSlots_[0]->SetDescription("Transform for coordinate system");
	inputSlots_[0]->SetVariantType(VariantType::VAR_MATRIX3X4);
	inputSlots_[0]->SetDefaultValue(Matrix3x4::IDENTITY);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("numberX");
	inputSlots_[1]->SetVariableName("NX");
	inputSlots_[1]->SetDescription("Approx. width in rhodo diameters along x-axis of box");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDefaultValue(4);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("numberY");
	inputSlots_[2]->SetVariableName("NY");
	inputSlots_[2]->SetDescription("Approx. width in rhodo diameters along y-axis of box");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[2]->SetDefaultValue(4);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("numberZ");
	inputSlots_[3]->SetVariableName("NZ");
	inputSlots_[3]->SetDescription("Approx. width in rhodo diameters along z-axis of box");
	inputSlots_[3]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[3]->SetDefaultValue(4);
	inputSlots_[3]->DefaultSet();

	inputSlots_[4]->SetName("Diameter");
	inputSlots_[4]->SetVariableName("D");
	inputSlots_[4]->SetDescription("Diameter of one rhombic dodecahedron");
	inputSlots_[4]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[4]->SetDefaultValue(4.0f);
	inputSlots_[4]->DefaultSet();

	outputSlots_[0]->SetName("Rectangular Grid Vertices");
	outputSlots_[0]->SetVariableName("G");
	outputSlots_[0]->SetDescription("Vertices");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

	outputSlots_[1]->SetName("Rhodo Lattice Polylines");
	outputSlots_[1]->SetVariableName("P");
	outputSlots_[1]->SetDescription("...");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[1]->SetDataAccess(DataAccess::LIST);
}

void Maths_RhodoLattice::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	Matrix3x4 trans = inSolveInstance[0].GetMatrix3x4();
	int xNum = inSolveInstance[1].GetInt();
	int yNum = inSolveInstance[2].GetInt();
	int zNum = inSolveInstance[3].GetInt();
	float rhdiam = inSolveInstance[4].GetFloat();
	float scale = 1.0f;
	if (rhdiam > 0.0f) {
		scale = 0.25f * rhdiam;
	}

	VariantVector vertexList;
	VariantVector polylineList;

	for (int f = 0; f < 4; ++f) {
		for (int i = 0; i < xNum; ++i) {
			for (int j = 0; j < yNum; ++j) {
				for (int k = 0; k < zNum; ++k) {
					Geomlib::Rhodo rh(f, i, j, k);
					VariantVector rhVerts = rh.ComputeVertices(scale);
					vertexList.Push(rhVerts);

					VariantVector rhPolylines = rh.ComputeRhombicPolylines(scale);
					polylineList.Push(rhPolylines);
				}
			}
		}
	}

	VariantVector transVertexList;
	for (unsigned i = 0; i < vertexList.Size(); ++i) {
		Vector3 transVert = trans * vertexList[i].GetVector3();
		transVertexList.Push(transVert);
	}

	VariantVector transPolylineList;
	for (unsigned i = 0; i < polylineList.Size(); ++i) {
		VariantMap polyMap = polylineList[i].GetVariantMap();
		VariantVector vs = polyMap["vertices"].GetVariantVector();
		VariantVector nvs;
		for (unsigned j = 0; j < vs.Size(); ++j) {
			nvs.Push(trans * vs[j].GetVector3());
		}
		transPolylineList.Push(Polyline_Make(nvs));
	}

	outSolveInstance[0] = transVertexList;
	outSolveInstance[1] = transPolylineList;
}