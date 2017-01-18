#include "Curve_PolylineLoft.h"

#include <assert.h>

#include "Geomlib_PolylineLoft.h"
#include "Polyline.h"

using namespace Urho3D;

String Curve_PolylineLoft::iconTexture = "Textures/Icons/Curve_PolylineLoft.png";

Curve_PolylineLoft::Curve_PolylineLoft(Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("LoftPolylines");
	SetFullName("Loft Polylines");
	SetDescription("Perform loft operation on a list of polylines");
	SetGroup(IoComponentGroup::CURVE);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("PolylineList");
	inputSlots_[0]->SetVariableName("L");
	inputSlots_[0]->SetDescription("List of polylines to loft");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::LIST);

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Lofted mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_PolylineLoft::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	if (inSolveInstance[0].GetType() != VariantType::VAR_VARIANTVECTOR) {
		URHO3D_LOGWARNING("L must be a list of polylines.");
		outSolveInstance[0] = Variant();
		return;
	}
	VariantVector polyList = inSolveInstance[0].GetVariantVector();
	for (unsigned i = 0; i < polyList.Size(); ++i) {
		if (!Polyline_Verify(polyList[i])) {
			URHO3D_LOGWARNING("L must contain only valid polylines.");
			outSolveInstance[0] = Variant();
			return;
		}
	}

	///////////////////
	// COMPONENT'S WORK

	Variant mesh;
	bool success = Geomlib::PolylineLoft(polyList, mesh);
	if (!success) {
		URHO3D_LOGWARNING("PolylineLoft operation failed.");
		outSolveInstance[0] = Variant();
		return;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = mesh;
}