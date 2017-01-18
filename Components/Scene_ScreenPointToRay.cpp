#include "Scene_ScreenPointToRay.h"
#include "Urho3D/UI/Button.h"
#include "Urho3D/UI/Text.h"

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Viewport.h>

using namespace Urho3D;

String Scene_ScreenPointToRay::iconTexture = "Textures/Icons/Scene_ScreenPointToRay.png";

Scene_ScreenPointToRay::Scene_ScreenPointToRay(Urho3D::Context* context) : IoComponentBase(context, 2, 2)
{
	SetName("PointToRay");
	SetFullName("Screen Point To Ray");
	SetDescription("Given a screen point, returns a ray in world coordinates");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("INPUT");

	inputSlots_[0]->SetName("ScreenPoint");
	inputSlots_[0]->SetVariableName("SP");
	inputSlots_[0]->SetDescription("Point in screen coordinates");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("CameraPtr");
	inputSlots_[1]->SetVariableName("CM");
	inputSlots_[1]->SetDescription("Optional pointer to a camera. If none provided, a camera will be found.");
	inputSlots_[1]->SetVariantType(VariantType::VAR_PTR);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Start Point");
	outputSlots_[0]->SetVariableName("SP");
	outputSlots_[0]->SetDescription("Start point of ray in world coords");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[1]->SetName("Direction");
	outputSlots_[1]->SetVariableName("DR");
	outputSlots_[1]->SetDescription("Direction of ray in world coords");
	outputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
	outputSlots_[1]->SetDataAccess(DataAccess::ITEM);
}

void Scene_ScreenPointToRay::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	Camera* cam = scene->GetComponent<Camera>(true);
	Graphics* graphics = GetSubsystem<Graphics>();
	Vector3 sp = inSolveInstance[0].GetVector3();
	Ray sr = cam->GetScreenRay(sp.x_ / graphics->GetWidth(), sp.y_ / graphics->GetHeight());

	//handle active viewport case
	Viewport* activeViewport = (Viewport*)GetGlobalVar("activeViewport").GetVoidPtr();
	if (activeViewport && cam)
	{
		IntRect viewRect = activeViewport->GetRect();

		float width = graphics->GetWidth();
		float height = graphics->GetHeight();
		
		if (viewRect != IntRect::ZERO)
		{
			width = viewRect.Width();
			height = viewRect.Height();
		}


		float x = (float)(sp.x_ - viewRect.left_) / width;
		float y = (float)(sp.y_ - viewRect.top_) / height;

	

		sr = cam->GetScreenRay(x, y);


		outSolveInstance[0] = sr.origin_;
		outSolveInstance[1] = sr.direction_;

		return;
	}

	if (!cam || !activeViewport)
	{
		URHO3D_LOGINFO("could not get active camera or viewport!");
	}


	outSolveInstance[0] = Variant();
	outSolveInstance[1] = Variant();
}