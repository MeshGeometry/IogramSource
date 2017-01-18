#include "Mesh_SaveMesh.h"
#include "Geomlib_TriMeshSaveOFF.h"

using namespace Urho3D;

String Mesh_SaveMesh::iconTexture = "Textures/Icons/Mesh_SaveMesh.png";

Mesh_SaveMesh::Mesh_SaveMesh(Context* context) :IoComponentBase(context, 0, 0)
{
	SetName("SaveMesh");
	SetFullName("Save Mesh");
	SetDescription("Saves a mesh in a variety of formats");

	AddInputSlot(
		"Mesh",
		"M",
		"Mesh to save",
		VAR_VARIANTMAP,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Path",
		"P",
		"Path",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Result",
		"R",
		"Result",
		VAR_BOOL,
		DataAccess::ITEM
	);
}

void Mesh_SaveMesh::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	VariantMap mesh = inSolveInstance[0].GetVariantMap();

	if (mesh.Keys().Contains("vertices") && mesh.Keys().Contains("faces"))
	{
		String path = inSolveInstance[1].GetString();
		if (path.Empty())
		{
			outSolveInstance[0] = Variant();
			return;
		}

		File* file = new File(GetContext());
		file->Open(path, FileMode::FILE_WRITE);

		bool result = Geomlib::TriMeshWriteOFF(mesh, *file);

		outSolveInstance[0] = result;

		file->Close();
	}
	else
	{
		outSolveInstance[0] = Variant();
		return;
	}
}


