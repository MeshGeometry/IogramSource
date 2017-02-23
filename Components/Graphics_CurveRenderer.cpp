#include "Graphics_CurveRenderer.h"
#include "CurveRenderer.h"
#include "Polyline.h"

#include <Urho3D/Graphics/BillboardSet.h>

using namespace Urho3D;

String Graphics_CurveRenderer::iconTexture = "Textures/Icons/Graphics_CurveRenderer.png";

Graphics_CurveRenderer::Graphics_CurveRenderer(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("Curve Renderer");
	SetFullName("Curve Renderer");
	SetDescription("Curve Renderer");

	AddInputSlot(
		"Curve",
		"P",
		"Curve to render",
		VAR_VARIANTMAP,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Width",
		"W",
		"Width of curve",
		VAR_FLOAT,
		DataAccess::ITEM,
		0.001f
	);

	AddInputSlot(
		"Color",
		"C",
		"Color",
		VAR_COLOR,
		DataAccess::ITEM,
		Color(0.2f, 0.2f, 0.2f,1.0f)
	);

	AddOutputSlot(
		"Node ID",
		"ID",
		"Node ID",
		VAR_INT,
		DataAccess::ITEM
	);
}

void Graphics_CurveRenderer::PreLocalSolve()
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (scene)
	{
		for (int i = 0; i < trackedItems.Size(); i++)
		{
			Node* n = scene->GetNode(trackedItems[i]);
			if (n)
			{
				n->Remove();
			}
		}
	}

	trackedItems.Clear();
}

void Graphics_CurveRenderer::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{

	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	if (scene == NULL)
	{
		URHO3D_LOGERROR("Null scene encountered.");
		return;
	}

	Node* node = scene->CreateChild("CurvePreview");

	Material* mat = NULL;
	mat = cache->GetResource<Material>("Materials/BasicCurve.xml");

	if (!mat)
	{
		cache->GetResource<Material>("Materials/BasicWebAlpha.xml");
	}

	if (!Polyline_Verify(inSolveInstance[0]) || !mat)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	VariantVector verts = Polyline_ComputeSequentialVertexList(inSolveInstance[0]);

	if (verts.Size() < 2)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	BillboardSet* curveDisplay = node->CreateComponent<BillboardSet>();
	SharedPtr<Material> firstMat = mat->Clone();
	curveDisplay->SetNumBillboards(verts.Size());
	curveDisplay->SetMaterial(firstMat);
	curveDisplay->SetSorted(true);
	curveDisplay->SetFaceCameraMode(FaceCameraMode::FC_DIRECTION);
	//curveDisplay->SetFixedScreenSize(true);

	SharedPtr<Material> clonedMat = mat->Clone();
	clonedMat->SetPixelShaderDefines("DRAWPOINT");
	BillboardSet* controlPointDisplay = node->CreateComponent<BillboardSet>();
	controlPointDisplay->SetNumBillboards(verts.Size());
	controlPointDisplay->SetMaterial(clonedMat);
	controlPointDisplay->SetSorted(true);
	controlPointDisplay->SetFaceCameraMode(FaceCameraMode::FC_ROTATE_XYZ);
	//controlPointDisplay->SetFixedScreenSize(true);

	float width = inSolveInstance[1].GetFloat();
	Color col = inSolveInstance[2].GetColor();

	firstMat->SetShaderParameter("MatDiffColor", col);
	clonedMat->SetShaderParameter("MatDiffColor", col);

	for (int i = 0; i < verts.Size() - 1; i++)
	{
		//render the edge
		Billboard* bb = curveDisplay->GetBillboard(i);

		Vector3 vA = verts[i].GetVector3();
		Vector3 vB = verts[i + 1].GetVector3();
		Vector3 edgeVec = vB - vA;
		Vector3 midPt = vA + 0.5f * edgeVec;
		
		bb->position_ = midPt;
		bb->size_ = Vector2(width, 0.5f * (edgeVec.Length()));
		bb->direction_ = edgeVec;
		bb->enabled_ = true;
		bb->color_ = col;

		//render the point
		bb = controlPointDisplay->GetBillboard(i);
		bb->position_ = vA;
		bb->size_ =  0.59f * Vector2(width, width);
		bb->enabled_ = true;
		bb->color_ = col;
		
	}

	curveDisplay->Commit();

	trackedItems.Push(node->GetID());
	outSolveInstance[0] = node->GetID();
}
