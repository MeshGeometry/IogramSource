#include "Curve_ZigZagPolyline.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ConversionUtilities.h"
#include "StockGeometries.h"
#include "Geomlib_TransformVertexList.h"
#include "Geomlib_TriMeshThicken.h"
#include "Polyline.h"

using namespace Urho3D;

String Curve_ZigZagPolyline::iconTexture = "";

Curve_ZigZagPolyline::Curve_ZigZagPolyline(Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("ZigZag");
	SetFullName("ConstructZigZagPolyline");
	SetDescription("Construct a zig zag polyline");
	SetGroup(IoComponentGroup::CURVE);
	SetSubgroup("Primitive");

	inputSlots_[0]->SetName("XScale");
	inputSlots_[0]->SetVariableName("XS");
	inputSlots_[0]->SetDescription("X-direction scale for zig zag");
	inputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Variant(1.0f));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("ZScale");
	inputSlots_[1]->SetVariableName("ZS");
	inputSlots_[1]->SetDescription("Z-direction scale for zig zag");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(Variant(1.0f));
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Transformation");
	inputSlots_[2]->SetVariableName("T");
	inputSlots_[2]->SetDescription("Transformation to apply to hexayurt");
	inputSlots_[2]->SetVariantType(VariantType::VAR_MATRIX3X4);
	inputSlots_[2]->SetDefaultValue(Matrix3x4::IDENTITY);
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Polyline");
	outputSlots_[0]->SetVariableName("P");
	outputSlots_[0]->SetDescription("Zig zag polyline");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_ZigZagPolyline::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	VariantType type0 = inSolveInstance[0].GetType();
	if (!(type0 == VariantType::VAR_FLOAT || type0 == VariantType::VAR_INT)) {
		URHO3D_LOGWARNING("XS must be a valid float or integer.");
		outSolveInstance[0] = Variant();
		return;
	}
	float s = inSolveInstance[0].GetFloat();
	// Verify input slot 1
	VariantType type1 = inSolveInstance[1].GetType();
	if (!(type1 == VariantType::VAR_FLOAT || type1 == VariantType::VAR_INT)) {
		URHO3D_LOGWARNING("ZS must be a valid float or integer.");
		outSolveInstance[0] = Variant();
		return;
	}
	float t = inSolveInstance[1].GetFloat();
	// Verify input slot 2
	VariantType type2 = inSolveInstance[2].GetType();
	if (type2 != VariantType::VAR_MATRIX3X4) {
		URHO3D_LOGWARNING("T must be a valid transform.");
		outSolveInstance[0] = Variant();
		return;
	}
	Matrix3x4 tr = inSolveInstance[2].GetMatrix3x4();

	// Check compatibility between inputs 0 and 1
	if (s == 0.0f && t == 0.0f) {
		URHO3D_LOGWARNING("Cannot have both XS and ZS equal to 0.");
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Variant baseZigZag;
	bool success = MakeZigZag(s, t, 11, baseZigZag);
	if (!success) {
		URHO3D_LOGWARNING("Failed to construct zig zag.");
		outSolveInstance[0] = Variant();
		return;
	}

	Variant zigZag = Polyline_ApplyTransform(baseZigZag, tr);

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = zigZag;
}