#include "Curve_Length.h"

#include "Polyline.h"
#include "Geomlib_PolylineOffset.h"
#include <assert.h>

using namespace Urho3D;

String Curve_Length::iconTexture = "Textures/Icons/Curve_Length.png";

Curve_Length::Curve_Length(Context* context) :
	IoComponentBase(context, 2, 1)
{
	SetName("CurveLength");
	SetFullName("Curve Length");
	SetDescription("Calculates the length of a curve.");

	inputSlots_[0]->SetName("Polyline");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Polyline");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);


	outputSlots_[0]->SetName("Length");
	outputSlots_[0]->SetVariableName("L");
	outputSlots_[0]->SetDescription("Curve Length");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_Length::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	Variant polylineIn = inSolveInstance[0];
	if (!Polyline_Verify(polylineIn)) {
		URHO3D_LOGWARNING("P must be a valid polyline.");
		outSolveInstance[0] = Variant();
		return;
	}

	outSolveInstance[0] = Polyline_GetCurveLength(polylineIn);
}