#include "Mesh_BoundaryVertices.h"

#include "igl/is_border_vertex.h"

#include "TriMesh.h"
#include "MeshTopologyQueries.h"
#include "ConversionUtilities.h"


using namespace Urho3D;

String Mesh_BoundaryVertices::iconTexture = "Textures/Icons/Mesh_BoundaryVertices.png";

Mesh_BoundaryVertices::Mesh_BoundaryVertices(Context* context) : IoComponentBase(context, 1, 4)
{
	SetName("VertexTopology");
	SetFullName("Compute Mesh Vertex Topology");
	SetDescription("Computes Mesh Topology Data for a given vertex");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Analysis");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("TriMesh");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);


	outputSlots_[0]->SetName("BoundaryVertexIDs");
	outputSlots_[0]->SetVariableName("B_ID");
	outputSlots_[0]->SetDescription("Indices of vertices on mesh boundary");
	outputSlots_[0]->SetVariantType(VariantType::VAR_INT);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

	outputSlots_[1]->SetName("BoundaryVertices");
	outputSlots_[1]->SetVariableName("B");
	outputSlots_[1]->SetDescription("Vertices on boundary");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[1]->SetDataAccess(DataAccess::LIST);

	outputSlots_[2]->SetName("InteriorVertexIDs");
	outputSlots_[2]->SetVariableName("I_ID");
	outputSlots_[2]->SetDescription("Indices of interior vertices of mesh");
	outputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	outputSlots_[2]->SetDataAccess(DataAccess::LIST);

	outputSlots_[3]->SetName("InteriorVertices");
	outputSlots_[3]->SetVariableName("I");
	outputSlots_[3]->SetDescription("Vertices in mesh interior");
	outputSlots_[3]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[3]->SetDataAccess(DataAccess::LIST);

}

void Mesh_BoundaryVertices::SolveInstance(
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
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	bool loadSuccess = IglMeshToMatrices(inMesh, V, F);

	if (loadSuccess) {
		VariantVector vertexList = TriMesh_GetVertexList(inMesh);
		std::vector<bool> isBorder = igl::is_border_vertex(V, F);

		VariantVector boundaryVertices;
		VariantVector interiorVertices;

		for (int i = 0; i < isBorder.size(); ++i) {
			if (isBorder[i] == true)
				boundaryVertices.Push(i);
			else if (isBorder[i] == false)
				interiorVertices.Push(i);
		}

		VariantVector boundaryVertexVectors;
		VariantVector interiorVertexVectors;

		for (int i = 0; i < boundaryVertices.Size(); ++i) {
			int vertID = boundaryVertices[i].GetInt();
			boundaryVertexVectors.Push(vertexList[vertID].GetVector3());
		}

		for (int i = 0; i < interiorVertices.Size(); ++i) {
			int vertID = interiorVertices[i].GetInt();
			interiorVertexVectors.Push(vertexList[vertID].GetVector3());
		}

		outSolveInstance[0] = Variant(boundaryVertices);
		outSolveInstance[1] = Variant(boundaryVertexVectors);
		outSolveInstance[2] = Variant(interiorVertices);
		outSolveInstance[3] = Variant(interiorVertexVectors);

	}
	else {
		URHO3D_LOGWARNING("Failed to convert mesh to libigl matrices.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}


}
