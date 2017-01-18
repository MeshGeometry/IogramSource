#include "Mesh_FillPolygon.h"
#include "Polyline.h"
#include "TriMesh.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ConversionUtilities.h"
#include "Geomlib_Incenter.h"
#include "Geomlib_TriangulatePolygon.h"

using namespace Urho3D;

String Mesh_FillPolygon::iconTexture = "Textures/Icons/Mesh_FillPolygon.png";

Mesh_FillPolygon::Mesh_FillPolygon(Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("FillPolygon");
	SetFullName("Fill polygon");
	SetDescription("Triangulates a polygon");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Polygon");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Tries to mesh a polygon");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Holes");
	inputSlots_[1]->SetVariableName("H");
	inputSlots_[1]->SetDescription("Holes of the polygon");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[1]->SetDataAccess(DataAccess::LIST);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Transform");
	inputSlots_[2]->SetVariableName("T");
	inputSlots_[2]->SetDescription("Optional transform for triangulation");
	inputSlots_[2]->SetVariantType(VariantType::VAR_MATRIX3X4);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(Matrix3x4::IDENTITY);
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Triangulated polygon");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_FillPolygon::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	Variant polyIn = inSolveInstance[0];
	Matrix3x4 matrix = inSolveInstance[2].GetMatrix3x4();
	VariantVector holes = inSolveInstance[1].GetVariantVector();

	Variant meshOut;

	if (Polyline_Verify(polyIn))
	{
		//don't support new polyline type yet in polygon filling
		VariantVector verts = Polyline_ComputeSequentialVertexList(polyIn);
		VariantMap newPoly;
		newPoly["vertices"] = verts;


		//do a bounding box check
		
		Vector<Vector3> vs;
		for (int i = 0; i < verts.Size(); i++)
		{
			vs.Push(verts[i].GetVector3());
		}
		BoundingBox bb(&vs[0],vs.Size());
		if (bb.Size().Length() < 0.001f)
		{
			SetAllOutputsNull(outSolveInstance);
			return;
		}


		bool success = Geomlib::TriangulatePolygon(newPoly, holes, meshOut);
	}
	else
	{
		bool success = Geomlib::TriangulatePolygon(polyIn, holes, meshOut);
	}


	outSolveInstance[0] = meshOut;
}