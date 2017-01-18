#include "Mesh_FacePolylines.h"

#include <assert.h>


#include "ConversionUtilities.h"
#include "TriMesh.h"
#include "Polyline.h"



using namespace Urho3D;

String Mesh_FacePolylines::iconTexture = "Textures/Icons/Mesh_FacePolylines.png";

Mesh_FacePolylines::Mesh_FacePolylines(Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("FacePolylines");
	SetFullName("Face Polylines");
	SetDescription("Returns the face polylines from a mesh");

	AddInputSlot(
		"Mesh",
		"M",
		"Mesh",
		VAR_VARIANTMAP,
		ITEM
	);

	AddOutputSlot(
		"Polylines",
		"P",
		"Polylines",
		VAR_VARIANTMAP,
		LIST
	);

}

void Mesh_FacePolylines::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{


	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	Variant inMesh = inSolveInstance[0];
	if (!TriMesh_Verify(inMesh)) {
		URHO3D_LOGWARNING("M must be a valid mesh.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//assuming tri mesh here...
	VariantVector faces = TriMesh_GetFaceList(inMesh);
	VariantVector verts = TriMesh_GetVertexList(inMesh);
	VariantVector polys;
	for (int i = 0; i < faces.Size() / 3; i++)
	{
		int a = faces[3 * i].GetInt();
		int b = faces[3 * i + 1].GetInt();
		int c = faces[3 * i + 2].GetInt();

		Vector3 vA = verts[a].GetVector3();
		Vector3 vB = verts[b].GetVector3();
		Vector3 vC = verts[c].GetVector3();

		VariantVector pVerts;
		pVerts.Push(vA);
		pVerts.Push(vB);
		pVerts.Push(vC);
		//close
		pVerts.Push(vA);

		polys.Push(Polyline_Make(pVerts));

	}

	// ASSIGN OUTPUTS

	outSolveInstance[0] = polys;
}
