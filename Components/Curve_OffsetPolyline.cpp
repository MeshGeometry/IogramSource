#include "Curve_OffsetPolyline.h"

#include "Polyline.h"
#include "Geomlib_PolylineOffset.h"
#include <assert.h>

using namespace Urho3D;

String Curve_OffsetPolyline::iconTexture = "Textures/Icons/Curve_OffsetPolyline.png";

Curve_OffsetPolyline::Curve_OffsetPolyline(Context* context) :
	IoComponentBase(context, 2, 1)
{
	SetName("OffsetPolyline");
	SetFullName("Offset Polyline (inexact)");
	SetDescription("Offsets a polyline by offsetting its vertices");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Geometry");

	inputSlots_[0]->SetName("Polyline");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Polyline to offset");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Distance");
	inputSlots_[1]->SetVariableName("D");
	inputSlots_[1]->SetDescription("Distance to offset by");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(1.0f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Offset Polyline");
	outputSlots_[0]->SetVariableName("OP");
	outputSlots_[0]->SetDescription("Offset polyline");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_OffsetPolyline::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	Variant polylineIn = inSolveInstance[0];
	if (!Polyline_Verify(polylineIn)) {
		URHO3D_LOGWARNING("P must be a valid polyline.");
		outSolveInstance[0] = Variant();
		return;
	}
	// Verify input slot 1
	VariantType type1 = inSolveInstance[1].GetType();
	if (!(type1 == VariantType::VAR_FLOAT || type1 == VariantType::VAR_INT)) {
		URHO3D_LOGWARNING("D must be an int or float type.");
		outSolveInstance[0] = Variant();
		return;
	}
	float distance = inSolveInstance[1].GetFloat();

	Variant polyOut;
	Geomlib::PolylineOffset(polylineIn, polyOut, distance);

	outSolveInstance[0] = polyOut;
}