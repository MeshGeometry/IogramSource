#include "Curve_Rebuild.h"
#include "Polyline.h"
#include <assert.h>

#include "Geomlib_RebuildPolyline.h"

using namespace Urho3D;

String Curve_Rebuild::iconTexture = "Textures/Icons/Curve_RebuildPolyline.png";

Curve_Rebuild::Curve_Rebuild(Context* context) :
	IoComponentBase(context, 0, 0)
{
	SetName("RefinePolyline");
	SetFullName("Refine Polyline");
	SetDescription("Refine polyline based on list of parameters");

	AddInputSlot(
		"Curve",
		"C",
		"Rebuilds the curve with the given number of segments",
		VAR_VARIANTMAP,
		DataAccess::ITEM
	);

	AddInputSlot(
		"NumPoints",
		"N",
		"Number of target points",
		VAR_INT,
		DataAccess::ITEM,
		10
	);

	AddOutputSlot(
		"Curve",
		"C",
		"The rebuilt curve.",
		VAR_VARIANTMAP,
		DataAccess::ITEM
	);
}

void Curve_Rebuild::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	if (!Polyline_Verify(inSolveInstance[0]))
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	int numPts = inSolveInstance[1].GetInt();

	Variant newPoly = Geomlib::RebuildPolyline(inSolveInstance[0], numPts);

	outSolveInstance[0] = newPoly;
}