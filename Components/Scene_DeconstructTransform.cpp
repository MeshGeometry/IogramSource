#include "Scene_DeconstructTransform.h"

#include "Scene_DeconstructTransform.h"

#include <assert.h>

using namespace Urho3D;

String Scene_DeconstructTransform::iconTexture = "Textures/Icons/Scene_DeconstructTransform.png";

Scene_DeconstructTransform::Scene_DeconstructTransform(Context* context) :
	IoComponentBase(context, 0, 0)
{
	SetName("DeconstructTransform");
	SetFullName("Deconstruct Transform");
	SetDescription("Deconstruct a transform into position, rotation, and scale");

	AddInputSlot(
		"Transform",
		"T",
		"Transform to deconstruct",
		VAR_MATRIX3X4,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Position",
		"P",
		"Position",
		VAR_VECTOR3,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Scale",
		"S",
		"Scale",
		VAR_VECTOR3,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Rotation",
		"R",
		"Rotation",
		VAR_QUATERNION,
		DataAccess::ITEM
	);

}

void Scene_DeconstructTransform::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Variant matVar = inSolveInstance[0];
	if (matVar.GetType() != VAR_MATRIX3X4)
	{
		URHO3D_LOGERROR("Expected a matrix, received a: " + inSolveInstance[0].GetTypeName());
		outSolveInstance[0] = Variant();
		outSolveInstance[1] = Variant();
		outSolveInstance[2] = Variant();
		return;
	}

	Matrix3x4 mat = matVar.GetMatrix3x4();
	outSolveInstance[0] = mat.Translation();
	outSolveInstance[1] = mat.Scale();
	outSolveInstance[2] = mat.Rotation();

}