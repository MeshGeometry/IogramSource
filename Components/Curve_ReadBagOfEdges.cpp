#include "Curve_ReadBagOfEdges.h"

#include <assert.h>

#include <iostream>
#include <string>

#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "ConversionUtilities.h"
#include "Polyline.h"
#include "TriMesh.h"
#include "Geomlib_ConstructTransform.h"

using namespace Urho3D;

#include <Eigen/Core>

#pragma warning(push, 0)
#include <igl/read_triangle_mesh.h>
#pragma warning(pop)

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/Exporter.hpp>
#include <assimp/postprocess.h>

namespace {
}

String Curve_ReadBagOfEdges::iconTexture = "Textures/Icons/DefaultIcon.png";

Curve_ReadBagOfEdges::Curve_ReadBagOfEdges(Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("ReadBagOfEdges");
	SetFullName("Read Bag Of Edges");
	SetDescription("Read bag of edges from file");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Primitive");

	inputSlots_[0]->SetName("Edge file");
	inputSlots_[0]->SetVariableName("F");
	inputSlots_[0]->SetDescription("Path to file storing the edges (obj, off, ply, dxf)");
	inputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue("Models/bumpy.off");
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Force Y Up");
	inputSlots_[1]->SetVariableName("Y_Up");
	inputSlots_[1]->SetDescription("Force y-axis to vertical/up");
	inputSlots_[1]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue(true);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Transform");
	inputSlots_[2]->SetVariableName("T");
	inputSlots_[2]->SetDescription("Apply transform to imported geometry");
	inputSlots_[2]->SetVariantType(VariantType::VAR_MATRIX3X4);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(Matrix3x4::IDENTITY);
	inputSlots_[2]->DefaultSet();

	/*
	outputSlots_[0]->SetName("Mesh out");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Mesh structure out");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);
	*/

	outputSlots_[0]->SetName("Polylines out");
	outputSlots_[0]->SetVariableName("PL");
	outputSlots_[0]->SetDescription("Polylines out");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

	/*
	outputSlots_[2]->SetName("Points out");
	outputSlots_[2]->SetVariableName("PT");
	outputSlots_[2]->SetDescription("Points out");
	outputSlots_[2]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[2]->SetDataAccess(DataAccess::LIST);
	*/
}

void Curve_ReadBagOfEdges::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	// WARNING: Do not auto validate inputs (i.e., IsAllInputValid)
	// as we want to allow multiple type of inputs at the Transform slot

	String meshFile = inSolveInstance[0].GetString();
	FileSystem* fs = GetSubsystem<FileSystem>();

	bool force_y_up = inSolveInstance[1].GetBool();
	Matrix3x4 transform = Matrix3x4::IDENTITY;
	if (inSolveInstance[2].GetType() == VariantType::VAR_MATRIX3X4) {
		transform = inSolveInstance[2].GetMatrix3x4();
	}
	else {
		transform = Geomlib::ConstructTransform(inSolveInstance[2]);
	}

	//construct a file using resource cache
	SharedPtr<File> rf = GetSubsystem<ResourceCache>()->GetFile(meshFile);

	if (!rf) {

		outSolveInstance[0] = Variant();

		URHO3D_LOGERROR("Could not open file in either resources or filesystem: " + meshFile);

		return;
	}

	//load the resource from memory
	String ext = GetExtension(meshFile);
	const int size = rf->GetSize();
	Vector<char> vb;
	vb.Resize(rf->GetSize());
	rf->Read(&vb[0], size);

	unsigned int pFlags = aiProcess_SortByPType | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices;
	//const aiScene* scene = aiImportFileFromMemory(&vb[0], vb.Size(), aiProcess_SortByPType | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices, ext.CString());
	const aiScene* scene = aiImportFileFromMemory(&vb[0], vb.Size(), pFlags, ext.CString());

	VariantVector polylinesOut;

	if (scene == NULL)
	{
		outSolveInstance[0] = polylinesOut;
		return;
	}

	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		VariantVector vertexList;
		VariantVector faceList;

		aiMesh* mesh = scene->mMeshes[i];

		//get verts
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			Vector3 currVert;
			if (force_y_up) {
				currVert = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].z, -1 * mesh->mVertices[i].y);
			}
			else {
				currVert = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			}
			vertexList.Push(transform * currVert);
		}
		std::cout << "vertexList.Size()=" << vertexList.Size() << "\n";

		int numVertices = mesh->mNumVertices;

		int face_size_this_pass = 0;
		if (mesh->mNumFaces > 0) {
			face_size_this_pass = mesh->mFaces[0].mNumIndices;
		}
		else {
			continue;
		}
		if (face_size_this_pass != 2) {
			std::cout << "face_size_this_pass != 2, continuing...." << std::endl;
			continue;
		}
		std::cout << "face_size_this_pass == 2" << std::endl;

		// Towards more flexible input
		/*
		if (mesh->mPrimitiveTypes & aiPrimitiveType::aiPrimitiveType_POLYGON) {
			std::cout << "found aiPrimitiveType_POLYGON!\n";
		}
		if (mesh->mPrimitiveTypes & aiPrimitiveType::aiPrimitiveType_TRIANGLE) {
			std::cout << "found aiPrimitiveType_TRIANGLE!\n";
		}
		*/

		// POLYLINE
		std::cout << "mesh i=" << i << " is a POLYLINE\n";

		std::cout << "printing indices from mesh->mFaces[i].mIndices[0,1] etc." << std::endl;
		int edge_count = 0;
		std::vector<int> edge_indices;
		for (int i = 0; i < mesh->mNumFaces; ++i) {
			if (mesh->mFaces[i].mNumIndices != 2) {
				continue;
			}
			++edge_count;
			//assert(mesh->mFaces[i].mNumIndices == 2);

			int i0 = mesh->mFaces[i].mIndices[0];
			int i1 = mesh->mFaces[i].mIndices[1];
			edge_indices.push_back(i0);
			edge_indices.push_back(i1);
			std::cout << "[i0, i1] = " << i0 << ", " << i1 << std::endl;

			Vector3 aa = vertexList[i0].GetVector3();
			Vector3 bb = vertexList[i1].GetVector3();
			VariantVector seg;
			seg.Push(Variant(aa));
			seg.Push(Variant(bb));
			Variant pol = Polyline_Make(seg);
			polylinesOut.Push(pol);
		}
	}

	outSolveInstance[0] = polylinesOut;
}