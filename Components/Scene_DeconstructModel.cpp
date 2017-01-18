#include "Scene_DeconstructModel.h"
#include <Urho3D/Graphics/AnimatedModel.h>
#include "TriMesh.h"
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

String Scene_DeconstructModel::iconTexture = "Textures/Icons/Scene_DeconstructModel.png";

namespace
{
	void ModelToMesh(Model* mdl, Variant& meshOut)
	{
		VariantVector verts;
		VariantVector faces;
		VariantVector normals;

		int numGeos = mdl->GetNumGeometries();
		int numLod = mdl->GetNumGeometryLodLevels(numGeos - 1);
		Geometry* geom = mdl->GetGeometry(numGeos - 1, numLod - 1);
		if (!geom)
		{
			return;
		}
		VertexBuffer* vb = mdl->GetGeometry(0, 0)->GetVertexBuffer(0);
		URHO3D_LOGINFO("vb length: " + String(vb->GetVertexCount()));

		const unsigned char* vertexData = (const unsigned char*)vb->Lock(0, vb->GetVertexCount());
		if (vertexData)
		{
			unsigned numVertices = vb->GetVertexCount();
			unsigned vertexSize = vb->GetVertexSize();
			// Copy the original vertex positions
			for (unsigned i = 0; i < numVertices; ++i)
			{
				const Vector3& src = *reinterpret_cast<const Vector3*>(vertexData + i * vertexSize);
				const Vector3& norm = *reinterpret_cast<const Vector3*>(vertexData + i * vertexSize + 12);
				verts.Push(src);
				normals.Push(norm);
			}
			vb->Unlock();
		}


		IndexBuffer* ib = mdl->GetGeometry(0, 0)->GetIndexBuffer();
		unsigned short* indexData = (unsigned short*)ib->Lock(0, ib->GetIndexCount());
		if (indexData)
		{
			unsigned numIndices = ib->GetIndexCount();
			unsigned indexSize = ib->GetIndexSize();

			// Copy the original vertex positions
			for (unsigned i = 0; i < numIndices; ++i)
			{
				int a = (int)indexData[i];
				faces.Push(a);
			}
			ib->Unlock();
		}

		meshOut = TriMesh_Make(verts, faces);
	}

	void RawDataToMesh(SharedPtr<VertexBuffer> vb, SharedPtr<IndexBuffer> ib, Variant& meshOut)
	{
		VariantVector verts;
		VariantVector faces;
		VariantVector normals;
		
		const unsigned char* vertexData = (const unsigned char*)vb->Lock(0, vb->GetVertexCount());
		if (vertexData)
		{
			unsigned numVertices = vb->GetVertexCount();
			unsigned vertexSize = vb->GetVertexSize();
			// Copy the original vertex positions
			for (unsigned i = 0; i < numVertices; ++i)
			{
				const Vector3& src = *reinterpret_cast<const Vector3*>(vertexData + i * vertexSize);
				const Vector3& norm = *reinterpret_cast<const Vector3*>(vertexData + i * vertexSize + 12);
				verts.Push(src);
				normals.Push(norm);
			}
			vb->Unlock();
		}

		unsigned short* indexData = (unsigned short*)ib->Lock(0, ib->GetIndexCount());
		if (indexData)
		{
			unsigned numIndices = ib->GetIndexCount();
			unsigned indexSize = ib->GetIndexSize();

			// Copy the original vertex positions
			for (unsigned i = 0; i < numIndices; ++i)
			{
				int a = (int)indexData[i];
				faces.Push(a);
			}
			ib->Unlock();
		}

		meshOut = TriMesh_Make(verts, faces);
		
	}
}

Scene_DeconstructModel::Scene_DeconstructModel(Urho3D::Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("DeconstructModel");
	SetFullName("Deconstruct Model");
	SetDescription("Deconstructs a static model into vertices, faces and normals");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("");

	inputSlots_[0]->SetName("Model");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Path to Model");
	inputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Mesh Out");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Scene_DeconstructModel::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	//VariantVector vertList = inSolveInstance[0].GetVariantVector();
	String mdlPath = inSolveInstance[0].GetString();
	URHO3D_LOGINFO("model path: " + mdlPath);

	ResourceCache* cache = GetSubsystem<ResourceCache>();

	Model* model = cache->GetResource<Model>(mdlPath);
	Variant meshOut;
	if (model != NULL) {
		ModelToMesh(model, meshOut);
		outSolveInstance[0] = meshOut;
		return;
	}

	AnimatedModel* am = (AnimatedModel*)inSolveInstance[0].GetPtr();
	if (am) {

		Model* m = am->GetModel();
		if (m)
		{
			
			//Vector<SharedPtr<VertexBuffer>> vbs = am->GetMorphVertexBuffers();
			//Vector<SharedPtr<IndexBuffer>> ibs = am->GetModel()->GetIndexBuffers();

			//RawDataToMesh(vbs[0], ibs[0], meshOut);
			outSolveInstance[0] = meshOut;
			return;
		}

	}

	StaticModel* sm = (StaticModel*)inSolveInstance[0].GetPtr();
	if (sm) {
		model = sm->GetModel();
		ModelToMesh(model, meshOut);
		outSolveInstance[0] = meshOut;
		return;
	}



	outSolveInstance[0] = Variant();
}