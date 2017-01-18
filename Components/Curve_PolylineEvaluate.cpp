#include "Curve_PolylineEvaluate.h"

#include <assert.h>

#include "Polyline.h"
#include "Geomlib_PolylinePointFromParameter.h"

using namespace Urho3D;

String Curve_PolylineEvaluate::iconTexture = "Textures/Icons/Curve_EvaluatePolyline.png";

Curve_PolylineEvaluate::Curve_PolylineEvaluate(Context* context) :
	IoComponentBase(context, 2, 1)
{
	SetName("EvaluatePolyline");
	SetFullName("Evaluate Polyline");
	SetDescription("Evaluate point on polyline from parameter");
	SetGroup(IoComponentGroup::CURVE);
	SetSubgroup("Geometry");

	inputSlots_[0]->SetName("Polyline");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Polyline");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Parameter");
	inputSlots_[1]->SetVariableName("T");
	inputSlots_[1]->SetDescription("Parameter to evaluate");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(0.5f));
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Point");
	outputSlots_[0]->SetVariableName("P");
	outputSlots_[0]->SetDescription("Point on polyline corresponding to parameter");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_PolylineEvaluate::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	if (!Polyline_Verify(inSolveInstance[0])) {
		URHO3D_LOGWARNING("P must be a valid polyline.");
		outSolveInstance[0] = Variant();
		return;
	}
	Variant polyline = inSolveInstance[0];
	// Verify input slot 1
	if (inSolveInstance[1].GetType() != VariantType::VAR_FLOAT) {
		URHO3D_LOGWARNING("T must have type float.");
		outSolveInstance[0] = Variant();
		return;
	}
	float t = inSolveInstance[1].GetFloat();
	if (t < 0.0f || t > 1.0f) {
		URHO3D_LOGWARNING("T must be in range 0.0f <= T <= 1.0f.");
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Vector3 point;
	bool success = Geomlib::PolylinePointFromParameter(polyline, t, point);
	if (!success) {
		URHO3D_LOGWARNING("EvaluatePolyline operation failed.");
		outSolveInstance[0] = Variant();
		return;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = Variant(point);
}