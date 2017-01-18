#include "Geometry_AffineTransformation.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>

#include "ConversionUtilities.h"
#include "TriMesh.h"
#include "Polyline.h"

#include "Geomlib_ConstructTransform.h"

using namespace Urho3D;

String Geometry_AffineTransformation::iconTexture = "Textures/Icons/Geometry_AffineTransformation.png";

// defaults to preview triangle mesh
Geometry_AffineTransformation::Geometry_AffineTransformation(Urho3D::Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("Affine Transformation");
	SetFullName("Apply Affine Transformation");
	SetDescription("Apply spatial transformation to geometric object");
	SetGroup(IoComponentGroup::TRANSFORM);
	SetSubgroup("Geometry");

	inputSlots_[0]->SetName("Geometry");
	inputSlots_[0]->SetVariableName("G");
	inputSlots_[0]->SetDescription("Mesh to transform");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	//inputSlots_[0]->SetDefaultValue(default_vec);
	//inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Transformation");
	inputSlots_[1]->SetVariableName("T");
	inputSlots_[1]->SetDescription("Transformation to apply to geometry");
	inputSlots_[1]->SetVariantType(VariantType::VAR_MATRIX3X4);
	inputSlots_[1]->SetDefaultValue(Matrix3x4::IDENTITY);
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("NewGeometry");
	outputSlots_[0]->SetVariableName("H");
	outputSlots_[0]->SetDescription("Transformed Geometry");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Geometry_AffineTransformation::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	// This works on meshes or polylines
	// Urho can "multiply" Matrix3X4's and Vector3's (representing position)

	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	bool is_tri_mesh = false;
	bool is_polyline = false;
	bool is_point3d = false;

	// Verify input 0 and extract vertexList
	Variant inGeom = inSolveInstance[0];
	VariantVector vertexList;
	if (TriMesh_Verify(inGeom)) {
		vertexList = TriMesh_GetVertexList(inGeom);
		is_tri_mesh = true;
	}
	else if (Polyline_Verify(inGeom)) {
		vertexList = Polyline_GetVertexList(inGeom);
		is_polyline = true;
	}
	else if (inGeom.GetType() == VariantType::VAR_VECTOR3) {
		is_point3d = true;
	}
	else {
		URHO3D_LOGWARNING("G must be a triangle mesh or polyline.");
		outSolveInstance[0] = Variant();
		return;
	}
	//// Verify input 1
	//if (inSolveInstance[1].GetType() != VariantType::VAR_MATRIX3X4) {
	//	URHO3D_LOGWARNING("T must be a 3x4 matrix.");
	//	outSolveInstance[0] = Variant();
	//	return;
	//}
	Matrix3x4 transform = Geomlib::ConstructTransform(inSolveInstance[1]);

	///////////////////
	// COMPONENT'S WORK

	Variant geomOut;

	if (is_tri_mesh || is_polyline) {

		VariantMap geomMap = inGeom.GetVariantMap();
		VariantVector newVertexList;

		for (unsigned i = 0; i < vertexList.Size(); ++i) {
			Vector3 vert = vertexList[i].GetVector3();
			Vector3 transformedVert = transform*vert;
			newVertexList.Push(Variant(transformedVert));
		}

		// newVertexList is ready
		Variant newVertices(newVertexList);
		geomMap["vertices"] = newVertices;
		geomOut = geomMap;
	}
	else if (is_point3d) {
		Vector3 pt = inGeom.GetVector3();
		pt = transform * pt;
		geomOut = Variant(pt);
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = geomOut;
}