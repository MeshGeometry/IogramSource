#include "Graphics_RenderLabeledMesh.h"
#include "TriMesh.h"
#include "NMesh.h"
#include "Polyline.h"

#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

String Graphics_RenderLabeledMesh::iconTexture = "Textures/Icons/Scene_MeshRenderer.png";

Graphics_RenderLabeledMesh::Graphics_RenderLabeledMesh(Urho3D::Context* context) : IoComponentBase(context, 5, 2)
{
	SetName("LabeledMeshRenderer");
	SetFullName("LabeledMeshRenderer");
	SetDescription("Converts a labeled mesh to a viewable object in the scene.");;

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh Structure");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Material");
	inputSlots_[1]->SetVariableName("MT");
	inputSlots_[1]->SetDescription("Path to material.");
	inputSlots_[1]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[2]->SetName("Color");
	inputSlots_[2]->SetVariableName("C");
	inputSlots_[2]->SetDescription("MainColor");
	inputSlots_[2]->SetVariantType(VariantType::VAR_COLOR);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(Color::WHITE);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("CoatedColor");
	inputSlots_[3]->SetVariableName("CC");
	inputSlots_[3]->SetDescription("CoatedColor");
	inputSlots_[3]->SetVariantType(VariantType::VAR_COLOR);
	inputSlots_[3]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[3]->SetDefaultValue(Color::BLACK);
	inputSlots_[3]->DefaultSet();

	inputSlots_[4]->SetName("LabelToColor");
	inputSlots_[4]->SetVariableName("L");
	inputSlots_[4]->SetDescription("ID of label to color");
	inputSlots_[4]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[4]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[4]->SetDefaultValue(-1);
	inputSlots_[4]->DefaultSet();

	outputSlots_[0]->SetName("NodeID");
	outputSlots_[0]->SetVariableName("ID");
	outputSlots_[0]->SetDescription("ID of rendered node");
	outputSlots_[0]->SetVariantType(VariantType::VAR_INT); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("StaticModelPointer");
	outputSlots_[1]->SetVariableName("SM");
	outputSlots_[1]->SetDescription("Void Pointer to Static Model");
	outputSlots_[1]->SetVariantType(VariantType::VAR_PTR); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}

Graphics_RenderLabeledMesh::~Graphics_RenderLabeledMesh()
{

}

void Graphics_RenderLabeledMesh::PreLocalSolve()
{
	//delete old nodes
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (scene)
	{
		for (int i = 0; i < trackedItems.Size(); i++)
		{
			Node* oldNode = scene->GetNode(trackedItems[i]);
			if (oldNode)
			{
				oldNode->Remove();
			}

		}

		trackedItems.Clear();
	}

	//release resource
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	for (int i = 0; i < trackedResources.Size(); i++)
	{
		String resourcePath = trackedResources[i];
		if (resourcePath.Contains("tmp/models/"))
			rc->ReleaseResource<Model>(trackedResources[i]);
		if (resourcePath.Contains("tmp/materials/"))
			rc->ReleaseResource<Material>(trackedResources[i]);
	}

}

void Graphics_RenderLabeledMesh::SolveInstance(
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
	Variant var_labels = mData["labels"];
	if (var_labels.GetType() != VariantType::VAR_VARIANTVECTOR)
		return;

	bool isTriMesh = TriMesh_Verify(inSolveInstance[0]);

	Context* context = GetContext();

	if (isTriMesh)
	{
		Color col = inSolveInstance[2].GetColor();
		Color coatCol = inSolveInstance[3].GetColor();
		String matPath = inSolveInstance[1].GetString();
		int labelToColor = inSolveInstance[4].GetInt();

		//adjust defaults so that alpha behaves correctly
		if (col.a_ < 1.0f && matPath.Empty())
		{
			matPath = normalAlphaMat;
		}
		else if (col.a_ >= 1.0f && matPath.Empty())
		{
			matPath = normalMat;
		}

		// TRIMESH render
		if (isTriMesh) {
			Variant model_pointer;
			int nodeId = TriMesh_Render(inSolveInstance[0], context, matPath, col, coatCol, labelToColor, model_pointer);

			trackedItems.Push(nodeId);
			outSolveInstance[0] = nodeId;
			outSolveInstance[1] = model_pointer.GetPtr();
		}//TRIMESH render
	}
	else
	{
		URHO3D_LOGERROR("Incomplete data given in Mesh paramater. Meshes require vertices, faces, and normals.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
}

int Graphics_RenderLabeledMesh::TriMesh_Render(Urho3D::Variant trimesh,
	Urho3D::Context* context,
	Urho3D::String material_path,
	Urho3D::Color mainColor,
	Urho3D::Color coatColor, 
	int labelToColor,
	Urho3D::Variant& model_pointer)
{

	Vector<VertexData> vbd;
	Vector<Vector3> tmpVerts;
	Vector<int> tmpFaces;

	VariantVector verts = TriMesh_GetVertexList(trimesh);
	VariantVector faces = TriMesh_GetFaceList(trimesh);
	VariantVector labels = TriMesh_GetLabelList(trimesh);
	VariantVector normals;

	assert(faces.Size()/3 == labels.Size());

	normals = TriMesh_ComputeFaceNormals(trimesh, true);
	tmpFaces.Resize(faces.Size());
	tmpVerts.Resize(faces.Size());
	vbd.Resize(faces.Size());

	unsigned int vCol = Urho3D::Color::WHITE.ToUInt();
	unsigned int vColCoat = coatColor.ToUInt();

	//render with duplicate verts for flat face shading
	for (unsigned i = 0; i < faces.Size(); i++)
	{
		bool coated = false;

		int fId = faces[i].GetInt();
		int normId = (int)floor((float)i / 3.0f);

		IntVector2 label = labels[normId].GetIntVector2();
		if (label.x_ == labelToColor || label.y_ == labelToColor)
			coated = true;

		//unsigned int col = vCols[normId%numColors].GetColor().ToUInt();
		if (fId < (int)verts.Size())
		{
			vbd[i].position = verts[fId].GetVector3();
			vbd[i].normal = normals[normId].GetVector3();
			if (coated)
				vbd[i].color = vColCoat;
			else
				vbd[i].color = vCol;
			tmpVerts[i] = vbd[i].position;
		}

		tmpFaces[i] = i;

	}

	SharedPtr<VertexBuffer> vb(new VertexBuffer(context));
	SharedPtr<IndexBuffer> ib(new IndexBuffer(context));

	// Shadowed buffer needed for raycasts to work, and so that data can be automatically restored on device loss
	vb->SetShadowed(true);
	vb->SetSize(vbd.Size(), Urho3D::MASK_POSITION | Urho3D::MASK_NORMAL | Urho3D::MASK_COLOR);
	vb->SetData((void*)&vbd[0]);

	ib->SetShadowed(true);
	ib->SetSize(tmpFaces.Size(), true);
	ib->SetData(&tmpFaces[0]);

	Geometry* geom = new Geometry(context);
	geom->SetNumVertexBuffers(1);
	geom->SetVertexBuffer(0, vb);
	geom->SetIndexBuffer(ib);
	geom->SetDrawRange(Urho3D::TRIANGLE_LIST, 0, faces.Size());

	SharedPtr<Model> model(new Model(context));
	model->SetNumGeometries(1);
	model->SetGeometry(0, 0, geom);
	model->SetBoundingBox(BoundingBox(&tmpVerts[0], tmpVerts.Size()));
	model->SetGeometryCenter(0, Vector3::ZERO);

	Vector<SharedPtr<VertexBuffer>> allVBuffers;
	Vector<SharedPtr<IndexBuffer>> allIBuffers;

	allVBuffers.Push(vb);
	allIBuffers.Push(ib);

	PODVector<unsigned int> morphStarts;
	PODVector<unsigned int> morphRanges;

	model->SetVertexBuffers(allVBuffers, morphStarts, morphRanges);
	model->SetIndexBuffers(allIBuffers);

	//create a new node
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	Material* mat = GetSubsystem<ResourceCache>()->GetResource<Material>(material_path);
	if (!scene || !mat)
	{
		//SetAllOutputsNull(outSolveInstance);
		return -1;
	}

	Node* mNode = scene->CreateChild("MeshPreviewNode");
	StaticModel* sm = mNode->CreateComponent<StaticModel>();
	int smID = sm->GetID();

	//SharedPtr<Material> cloneMat = mat->Clone();
	//cloneMat->SetName("tmp/materials/generated_mat_" + String(smID));
	//cloneMat->SetShaderParameter("MatDiffColor", col);
	mat->SetShaderParameter("MatDiffColor", mainColor);
	model->SetName("tmp/models/generated_model_" + String(smID));

	//add these to the resource cache
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	bool res = rc->AddManualResource(model);
	//res = rc->AddManualResource(cloneMat);
	//trackedResources.Push(cloneMat->GetName());
	trackedResources.Push(model->GetName());

	sm->SetModel(model);
	//sm->SetMaterial(cloneMat);
	sm->SetMaterial(mat);

	model_pointer = Variant(model);

	return mNode->GetID();
}




