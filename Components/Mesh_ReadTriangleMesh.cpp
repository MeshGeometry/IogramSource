#include "Mesh_ReadTriangleMesh.h"

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

String Mesh_ReadTriangleMesh::iconTexture = "Textures/Icons/Mesh_ReadTriangleMesh.png";

Mesh_ReadTriangleMesh::Mesh_ReadTriangleMesh(Context* context) : IoComponentBase(context, 3, 3)
{
	SetName("ReadTriMesh");
	SetFullName("Read Triangle Mesh");
	SetDescription("Read triangle mesh from file");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Primitive");

	inputSlots_[0]->SetName("Mesh file");
	inputSlots_[0]->SetVariableName("F");
	inputSlots_[0]->SetDescription("Path to file storing the mesh (obj, off, ply, dxf)");
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

	outputSlots_[0]->SetName("Mesh out");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Mesh structure out");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);

	outputSlots_[1]->SetName("Polylines out");
	outputSlots_[1]->SetVariableName("PL");
	outputSlots_[1]->SetDescription("Polylines out");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[1]->SetDataAccess(DataAccess::LIST);

	outputSlots_[2]->SetName("Points out");
	outputSlots_[2]->SetVariableName("PT");
	outputSlots_[2]->SetDescription("Points out");
	outputSlots_[2]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[2]->SetDataAccess(DataAccess::LIST);
}

void Mesh_ReadTriangleMesh::SolveInstance(
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
		outSolveInstance[1] = Variant();
		outSolveInstance[2] = Variant();

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


	VariantVector meshesOut;
	VariantVector polylinesOut;
	VariantVector pointsOut;

	if (scene == NULL)
	{
		outSolveInstance[0] = meshesOut;
		outSolveInstance[1] = polylinesOut;
		outSolveInstance[2] = pointsOut;
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

		// Towards more flexible input
		if (mesh->mPrimitiveTypes & aiPrimitiveType::aiPrimitiveType_POLYGON) {
			std::cout << "found aiPrimitiveType_POLYGON!\n";
		}
		if (mesh->mPrimitiveTypes & aiPrimitiveType::aiPrimitiveType_TRIANGLE) {
			std::cout << "found aiPrimitiveType_TRIANGLE!\n";
		}

		if (face_size_this_pass == 3) {
			// TRI_MESH
			std::cout << "mesh i=" << i << " is a TRI_MESH\n";

			for (int i = 0; i < mesh->mNumFaces; ++i) {
				assert(mesh->mFaces[i].mNumIndices == 3);

				int i0 = mesh->mFaces[i].mIndices[0];
				int i1 = mesh->mFaces[i].mIndices[1];
				int i2 = mesh->mFaces[i].mIndices[2];

				if (
					(i0 < 0 || i0 > numVertices - 1) ||
					(i1 < 0 || i1 > numVertices - 1) ||
					(i2 < 0 || i2 > numVertices - 1)
					)
				{
					continue;
				}

				if (i0 == i1 && i1 == i2) {
					// point?
					/*
					Vector3 cur_pt = verts[i0].GetVector3();
					pointsOut.Push(cur_pt);
					*/
					continue;
				}

				if (i0 == i1 || i1 == i2 || i2 == i0) {
					/*
					int j0 = i0;
					int j1 = (i0 == i1) ? i2 : i1;

					Vector3 aa = verts[j0].GetVector3();
					Vector3 bb = verts[j1].GetVector3();
					VariantVector seg;
					seg.Push(Variant(aa));
					seg.Push(Variant(bb));
					Variant pol = Polyline_Make(seg);
					polylinesOut.Push(pol);
					*/
					continue;
				}

				faceList.Push(i0);
				faceList.Push(i1);
				faceList.Push(i2);
			}

			Variant outMesh = TriMesh_Make(vertexList, faceList);

			meshesOut.Push(outMesh);
		}
		else if (face_size_this_pass == 2) {
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
			}

			/*
			if (edge_indices.size() > 0 && edge_indices.size() % 2 == 0) {
				int rows = edge_indices.size() / 2;
				Eigen::MatrixXi E(rows, 2);
				for (int j = 0; j < rows; ++j) {
					std::cout << "E.row(j), j = " << j << std::endl;
					E.row(j) = Eigen::RowVector2i(edge_indices[2 * j], edge_indices[2 * j + 1]);
				}
				std::cout << "E" << std::endl;
				std::cout << E << std::endl;
			}
			*/

			Variant outPolyline = Polyline_Make(vertexList);
			polylinesOut.Push(outPolyline);
		}
		else if (face_size_this_pass = 1) {
			// POINT3D
			std::cout << "mesh i=" << i << "is a POINT3D\n";

			for (int i = 0; i < mesh->mNumFaces; ++i) {
				assert(mesh->mFaces[i].mNumIndices == 1);

				int i0 = mesh->mFaces[i].mIndices[0];

				Vector3 pp = vertexList[i0].GetVector3();
				pointsOut.Push(pp);
			}
		}
		else {
			// EMPTY FACE LEAVE IT ALONE
		}
	}

	outSolveInstance[0] = meshesOut;
	outSolveInstance[1] = polylinesOut;
	outSolveInstance[2] = pointsOut;

}