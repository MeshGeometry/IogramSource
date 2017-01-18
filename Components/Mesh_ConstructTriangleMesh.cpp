#include "Mesh_ConstructTriangleMesh.h"

#include <assert.h>



#include "ConversionUtilities.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_ConstructTriangleMesh::iconTexture = "Textures/Icons/Mesh_ConstructTriangleMesh.png";

Mesh_ConstructTriangleMesh::Mesh_ConstructTriangleMesh(Context* context) :
	IoComponentBase(context, 3, 1)
{
	SetName("ConTriMesh");
	SetFullName("Construct Triangle Mesh");
	SetDescription("Construct a mesh from vertex and face lists");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Primitive");

	inputSlots_[0]->SetName("Vertices");
	inputSlots_[0]->SetVariableName("V");
	inputSlots_[0]->SetDescription("List of coordinates of vertices");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDataAccess(DataAccess::LIST);

	inputSlots_[1]->SetName("Faces");
	inputSlots_[1]->SetVariableName("F");
	inputSlots_[1]->SetDescription("List of face indices");
	inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[1]->SetDataAccess(DataAccess::LIST);

	inputSlots_[2]->SetName("Normals");
	inputSlots_[2]->SetVariableName("N");
	inputSlots_[2]->SetDescription("List of vertex normals. If ommitted, they will be computed.");
	inputSlots_[2]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[2]->SetDataAccess(DataAccess::LIST);
	inputSlots_[2]->SetDefaultValue(Vector3::ZERO);
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Constructed mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_ConstructTriangleMesh::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());

	Variant vertices = inSolveInstance[0]; // copy vertices
	Variant faces = inSolveInstance[1]; // copy faces
	Variant normals = inSolveInstance[2];

	///////////////////////////////////////////////////////////////////////////////////////////////
	HashMap<StringHash, Variant> meshMap;
	meshMap["vertices"] = vertices;
	meshMap["faces"] = faces;

	Eigen::MatrixXf V;
	Eigen::MatrixXi F;
	IglMeshToMatrices(meshMap, V, F);

	if (normals.GetType() == VAR_VARIANTVECTOR && normals.GetVariantVector().Size() >= 3)
		meshMap["normals"] = normals;
	else
		meshMap["normals"] = IglComputeVertexNormals(V, F);

	Variant outMesh = TriMesh_Make(vertices, faces);

	//Variant outMesh(meshMap);
	///////////////////////////////////////////////////////////////////////////////////////////////

	outSolveInstance[0] = outMesh;
}