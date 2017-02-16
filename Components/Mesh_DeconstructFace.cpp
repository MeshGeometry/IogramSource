#include "Mesh_DeconstructFace.h"

#include "TriMesh.h"
#include "MeshTopologyQueries.h"
#include "ConversionUtilities.h"


using namespace Urho3D;

String Mesh_DeconstructFace::iconTexture = "Textures/Icons/Mesh_DeconstructFace.png";

Mesh_DeconstructFace::Mesh_DeconstructFace(Context* context) : IoComponentBase(context, 2, 4)
{
	SetName("DeconstructFace");
	SetFullName("Compute Mesh Face Data");
	SetDescription("Returns normal, centroid and vertices of face");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Analysis");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("TriMesh");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("FaceID");
	inputSlots_[1]->SetVariableName("ID");
	inputSlots_[1]->SetDescription("ID of face of interest");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(0);
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("FaceVertexIDs");
	outputSlots_[0]->SetVariableName("V_ID");
	outputSlots_[0]->SetDescription("Indices of vertices on mesh face");
	outputSlots_[0]->SetVariantType(VariantType::VAR_INT);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

	outputSlots_[1]->SetName("FaceVertices");
	outputSlots_[1]->SetVariableName("V");
	outputSlots_[1]->SetDescription("Vertices on face");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[1]->SetDataAccess(DataAccess::LIST);

	outputSlots_[2]->SetName("FaceNormal");
	outputSlots_[2]->SetVariableName("N");
	outputSlots_[2]->SetDescription("Normal to face");
	outputSlots_[2]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[2]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[3]->SetName("FaceCentroid");
	outputSlots_[3]->SetVariableName("C");
	outputSlots_[3]->SetDescription("Centroid of face");
	outputSlots_[3]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[3]->SetDataAccess(DataAccess::ITEM);

}

void Mesh_DeconstructFace::SolveInstance(
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

	int vertID = inSolveInstance[1].GetInt();

	///////////////////
	// COMPONENT'S WORK

		VariantVector vertexList = TriMesh_GetVertexList(inMesh);
		VariantVector faceList = TriMesh_GetFaceList(inMesh);
		VariantVector normalList = TriMesh_GetNormalList(inMesh);
		Vector3 normal = normalList[vertID].GetVector3();
		normal = normal.Normalized();

		VariantVector faceVertexIDs;
		VariantVector faceVertexVectors;
		Vector3 centroid = Vector3::ZERO;


		// get the vert IDs
		for (int i = 0; i < 3; ++i)
		{
			faceVertexIDs.Push(faceList[3 * vertID + i].GetInt());
		}

		// get the vertex vectors
		for (int i = 0; i < faceVertexIDs.Size(); ++i) {
			int vertID = faceVertexIDs[i].GetInt();
			Vector3 curVert = vertexList[vertID].GetVector3();
			faceVertexVectors.Push(curVert);
			centroid += curVert;
		}

		// find the actual centroid
		centroid = centroid / 3;


		outSolveInstance[0] = Variant(faceVertexIDs);
		outSolveInstance[1] = Variant(faceVertexVectors);
		outSolveInstance[2] = Variant(normal);
		outSolveInstance[3] = Variant(centroid);


}
