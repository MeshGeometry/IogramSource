#include "Curve_SmoothPolyline.h"

#include <assert.h>

#include "Polyline.h"
#include "Geomlib_SmoothPolyline.h"

using namespace Urho3D;

String Curve_SmoothPolyline::iconTexture = "Textures/Icons/Curve_SmoothPolyline.png";

Curve_SmoothPolyline::Curve_SmoothPolyline(Context* context) :
	IoComponentBase(context, 2, 1)
{
	SetName("SmoothPolyline");
	SetFullName("Smooth Polyline");
	SetDescription("Smooth polyline via subdivision");
	SetGroup(IoComponentGroup::CURVE);
	SetSubgroup("Geometry");

	inputSlots_[0]->SetName("Polyline");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Polyline to smooth");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Iterations");
	inputSlots_[1]->SetVariableName("I");
	inputSlots_[1]->SetDescription("Number of smoothing iterations to perform");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(1));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Smoothed Polyline");
	outputSlots_[0]->SetVariableName("P");
	outputSlots_[0]->SetDescription("Smoothed and subdivided polyline");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_SmoothPolyline::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	Variant inPolyline = inSolveInstance[0];
	if (!Polyline_Verify(inPolyline)) {
		URHO3D_LOGWARNING("P must be a valid polyline.");
		outSolveInstance[0] = Variant();
		return;
	}
	// Verify input slot 1
	if (inSolveInstance[1].GetType() != VariantType::VAR_INT) {
		URHO3D_LOGWARNING("I must be an integer.");
		outSolveInstance[0] = Variant();
		return;
	}
	int steps = inSolveInstance[1].GetInt();
	if (steps < 0) {
		URHO3D_LOGWARNING("I must be >= 0.");
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Variant outPolyline;

	if (steps == 0) {
		outPolyline = inPolyline;
	}
	else {
		for (int i = 0; i < steps; ++i) {
			bool success = Geomlib::SmoothPolyline(inPolyline, outPolyline);
			if (!success) {
				URHO3D_LOGWARNING("Smoothing operation failed.");
				outSolveInstance[0] = Variant();
				return;
			}
			inPolyline = outPolyline;
		}
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = outPolyline;
}