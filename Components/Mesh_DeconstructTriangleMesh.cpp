#include "Mesh_DeconstructTriangleMesh.h"

using namespace Urho3D;

String Mesh_DeconstructTriangleMesh::iconTexture = "Textures/Icons/Mesh_DeconstructTriangleMesh.png";

Mesh_DeconstructTriangleMesh::Mesh_DeconstructTriangleMesh(Context* context) :IoComponentBase(context, 1, 4)
{
	SetName("DeconstructMesh");
	SetFullName("Deconstruct Mesh");
	SetDescription("Deconstructs a triangle mesh into vertices, faces, and normals");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Path to Model");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("Vertices");
	outputSlots_[0]->SetVariableName("V");
	outputSlots_[0]->SetDescription("Vertices Out");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

	outputSlots_[1]->SetName("Faces");
	outputSlots_[1]->SetVariableName("F");
	outputSlots_[1]->SetDescription("Faces Out");
	outputSlots_[1]->SetVariantType(VariantType::VAR_INT); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[1]->SetDataAccess(DataAccess::LIST);

	outputSlots_[2]->SetName("Normals");
	outputSlots_[2]->SetVariableName("N");
	outputSlots_[2]->SetDescription("Normals Out");
	outputSlots_[2]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[2]->SetDataAccess(DataAccess::LIST);

	outputSlots_[3]->SetName("FaceCounts");
	outputSlots_[3]->SetVariableName("FC");
	outputSlots_[3]->SetDescription("Faces counts");
	outputSlots_[3]->SetVariantType(VariantType::VAR_INT);
	outputSlots_[3]->SetDataAccess(DataAccess::LIST);
}

void Mesh_DeconstructTriangleMesh::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	//some checks
	if (inSolveInstance[0].GetType() != VAR_VARIANTMAP)
	{
		URHO3D_LOGERROR("Expected a VariantMap and was given a " + inSolveInstance[0].GetTypeName());
		return;
	}

	VariantMap mData = inSolveInstance[0].GetVariantMap();

	if (mData.Keys().Contains("vertices") && mData.Keys().Contains("faces") && mData.Keys().Contains("normals"))
	{
		outSolveInstance[0] = mData["vertices"];
		outSolveInstance[1] = mData["faces"];

		VariantVector faceCounts;
		VariantVector faces = mData["faces"].GetVariantVector();
		for (int i = 0; i < faces.Size()/3; i++)
		{
			faceCounts.Push(3);
		}

		outSolveInstance[2] = mData["normals"];
		outSolveInstance[3] = faceCounts;
	}
	else
	{
		URHO3D_LOGERROR("Incomplete data given in Mesh paramater. Meshes require vertices, faces, and normals.");
		return;
	}
	
}