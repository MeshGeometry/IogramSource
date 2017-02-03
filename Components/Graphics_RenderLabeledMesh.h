#pragma once

#include "IoComponentBase.h"
#include <Urho3D/Graphics/Model.h>
#include "Graphics_MeshRenderer.h"

////vertex data types
//struct VertexData
//{
//	Urho3D::Vector3 position; // size of 12 bytes +
//	Urho3D::Vector3 normal; // size of 12 bytes +
//	unsigned color;
//};

class URHO3D_API Graphics_RenderLabeledMesh : public IoComponentBase {

	URHO3D_OBJECT(Graphics_RenderLabeledMesh, IoComponentBase)

public:
	Graphics_RenderLabeledMesh(Urho3D::Context* context);
	~Graphics_RenderLabeledMesh();

	static Urho3D::String iconTexture;

	virtual void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	int TriMesh_Render(Urho3D::Variant trimesh,
		Urho3D::Context* context,
		Urho3D::String material_path,
		Urho3D::Color mainColor,
		Urho3D::Color coatedColor,
		int labelToColor,
		Urho3D::Variant& model_pointer);

#ifdef EMSCRIPTEN
	Urho3D::String normalMat = "Materials/BasicWeb.xml";
	Urho3D::String normalAlphaMat = "Materials/BasicWebAlpha.xml";
#else
	Urho3D::String normalMat = "Materials/BasicPBR.xml";
	Urho3D::String normalAlphaMat = "Materials/BasicPBRAlpha.xml";
#endif

	Urho3D::Vector<int> trackedItems;
	Urho3D::Vector<Urho3D::String> trackedResources;
	int autoNameCounter = 0;

};
