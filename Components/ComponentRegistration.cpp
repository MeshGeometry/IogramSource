#include "ComponentRegistration.h"

///////////////////////////////////////// COMPONENT REGISTRATION ///////////////////////////////////////////////////////

/*
In order for a graph component to work with the system,
you must include it here and follow the registration pattern in RegisterComponents
*/

#include "Maths_Addition.h"
#include "Maths_Subtraction.h"
#include "Maths_Multiplication.h"
#include "Maths_Division.h"
#include "Maths_GreaterThan.h"
#include "Maths_LessThan.h"
#include "Maths_RandomValue.h"
#include "Maths_DotProduct.h"
#include "Maths_CrossProduct.h"
#include "Maths_VectorLength.h"
#include "Maths_UnitizeVector.h"
#include "Maths_Lerp.h"
#include "Maths_RectangularGrid.h"
#include "Maths_RectangularArray.h"
#include "Maths_HexGrid.h"
#include "Maths_RhodoArray3D.h"
#include "Maths_RhodoLattice.h"
#include "Geometry_Rotation.h"
//#include "Geometry_Transform.h"
#include "Geometry_AffineTransformation.h"
#include "Geometry_LookAt.h"
#include "Sets_Series.h"
#include "Sets_LogisticGrowthSeries.h"
#include "Sets_ListItem.h"
#include "Sets_IfThen.h"
#include "Sets_Merge.h"
#include "Sets_ListConstruct.h"
#include "Sets_ShiftList.h"
#include "Sets_LoopBegin.h"
#include "Sets_LoopEnd.h"
#include "Sets_DataRecorder.h"
#include "Sets_AddKeyValue.h"
#include "Sets_GetValueByKey.h"
#include "Sets_ExportViewData.h"
#include "Sets_ImportViewData.h"
#include "Maths_MassAverage.h"
#include "Maths_MassAddition.h"
#include "Input_Slider.h"
#include "Input_Panel.h"
#include "Input_Int.h"
#include "Input_Float.h"
#include "Input_Label.h"
#include "Input_Toggle.h"
#include "Input_ScreenToggle.h"
#include "Input_ScreenContainer.h"
#include "Input_CustomElement.h"
#include "Input_ButtonListener.h"
#include "Input_KeyboardListener.h"
#include "Input_MouseDownListener.h"
#include "Input_GamepadListener.h"
#include "Input_ScreenLineEdit.h"
#include "Input_ScreenText.h"
#include "Input_StringReplace.h"
#include "Input_StringAppend.h"
#include "Input_ScanDir.h"
#include "Input_Trigger.h"
#include "Input_ObjectMove.h"
#include "Input_Vector3.h"
#include "Input_ColorSlider.h"
#include "Input_GeometryEdit.h"
#include "Input_EditGeometryListener.h"
#include "Interop_SystemCommand.h"
#include "Interop_AsyncSystemCommand.h"
#include "Resource_LoadResource.h"
#include "Resource_CreateMaterial.h"
#include "Graphics_BaseSettings.h"
#include "Graphics_Viewport.h"
#include "Graphics_MeshRenderer.h"
#include "Graphics_CurveToModel.h"
#include "Graphics_CurveRenderer.h"
#include "Graphics_MeshEdges.h"
#include "Physics_PhysicsWorld.h"
#include "Physics_RigidBody.h"
#include "Physics_ApplyForce.h"
#include "Physics_CollisionShape.h"
#include "Physics_Constraint.h"
#include "Scene_DeconstructModel.h"
#include "Scene_Display.h"
#include "Scene_AddNode.h"
#include "Scene_CloneNode.h"
#include "Scene_Text3D.h"
#include "Scene_AddStaticModel.h"
#include "Scene_AnimatedModel.h"
#include "Scene_PlayAnimation.h"
#include "Scene_ScreenPointToRay.h"
#include "Scene_Raycast.h"
#include "Scene_ScreenBloom.h"
#include "Scene_ScriptInstance.h"
#include "Scene_MouseClickListener.h"
#include "Input_LineEditListener.h"
#include "Scene_ModifyNode.h"
#include "Scene_SelectGeometry.h"
#include "Scene_GetNode.h"
#include "Scene_GetComponent.h"
#include "Scene_AddComponent.h"
#include "Scene_ModifyComponent.h"
#include "Scene_SendEvent.h"
#include "Scene_HandleEvent.h"
#include "Scene_SetGlobalVar.h"
#include "Scene_GetGlobalVar.h"
#include "Scene_TriMeshVisualizeScalarField.h"
#include "Mesh_DecimateMesh.h"
#include "Scene_LoadScene.h"
#include "Scene_SaveScene.h"
#include "Scene_AppendRenderPath.h"
#include "Maths_ConstructTransform.h"
#include "Scene_DeconstructTransform.h"
#include "Vector_ConstructVector.h"
#include "Input_Inspect.h"
#include "Mesh_SmoothMesh.h"
#include "Input_ScreenSlider.h"
#include "Tree_Flatten.h"
#include "Tree_Graft.h"
#include "Tree_Reverse.h"
#include "Tree_Simplify.h"
#include "Tree_GetItem.h"
#include "Tree_FlipMatrix.h"
#include "Sets_ListLength.h"
#include "Sets_VariantMap.h"
#include "Mesh_ReadTriangleMesh.h"
#include "Mesh_DeconstructTriangleMesh.h"
#include "Mesh_ConstructTriangleMesh.h"
#include "Mesh_ClosestPoint.h"
#include "Mesh_HexayurtMesh.h"
#include "Mesh_CubeMesh.h"
#include "Mesh_Icosahedron.h"
#include "Mesh_Sphere.h"
#include "Mesh_FieldRemesh.h"
#include "Mesh_SaveMesh.h"
#include "Mesh_CleanMesh.h"
#include "Mesh_BoundingBox.h"
#include "Mesh_HarmonicDeformation.h"
#include "Mesh_HausdorffDistance.h"
#include "Curve_ZigZagPolyline.h"
#include "Curve_Polyline.h"
#include "Curve_OffsetPolyline.h"
#include "Curve_SmoothPolyline.h"
#include "Curve_LineSegment.h"
#include "Curve_SmoothPolyline.h"
#include "Curve_PolylineBlend.h"
#include "Curve_PolylineDivide.h"
#include "Curve_PolylineLoft.h"
#include "Curve_Polygon.h"
#include "Curve_PolylineEvaluate.h"
#include "Mesh_SubdivideMesh.h"
#include "Input_SliderListener.h"
#include "Vector_DeconstructVector.h"
#include "Maths_EvalFunction.h"
#include "Input_ColorWheel.h"
#include "Vector_ClosestPoint.h"
#include "Vector_Distance.h"
#include "Vector_ColorRGBA.h"
#include "Vector_BestFitPlane.h"
#include "Vector_ColorPalette.h"
#include "Scene_UpdateListener.h"
#include "Mesh_Offset.h"
#include "Mesh_Window.h"
#include "Mesh_Frame.h"
#include "Mesh_Thicken.h"
#include "Mesh_LoopSubdivide.h"
#include "Mesh_ExtrudePolyline.h"
#include "Mesh_FillPolygon.h"
#include "Mesh_FacePolylines.h"
#include "Mesh_Boundary.h"
#include "Mesh_JoinMeshes.h"
#include "Mesh_TriMeshVolume.h"
#include "Mesh_Tetrahedralize.h"
#include "Mesh_MeshPlaneIntersection.h"
#include "Mesh_AverageEdgeLength.h"
#include "Mesh_UnifyNormals.h"
#include "Mesh_SplitLongEdges.h"
#include "Mesh_CollapseShortEdges.h"
#include "Mesh_MeanCurvatureFlow.h"
#include "Mesh_PerVertexEval.h"
#include "Spatial_ReadOSM.h"
#include "Offsets_NgonMeshReader.h"

#include "Widget_Base.h"
#include "Widget_OptionSlider.h"
#include "Widget_Container.h"
#include "Widget_TextLabel.h"
#include "Widget_Vector3Slider.h"
#include "Widget_ColorSlider.h"
#include "CurveRenderer.h"

using namespace Urho3D;

void ComponentRegistration::RegisterCoreComponents(Context* context)
{

	context->RegisterFactory<Widget_Base>();
	context->RegisterFactory<Widget_OptionSlider>();
	context->RegisterFactory<Widget_Container>();
	context->RegisterFactory<Widget_TextLabel>();
	context->RegisterFactory<Widget_Vector3Slider>();
	context->RegisterFactory<Widget_ColorSlider>();
	context->RegisterFactory<CurveRenderer>();
	
	RegisterIogramType<Maths_Addition>(context);
	RegisterIogramType<Maths_Subtraction>(context);
	RegisterIogramType<Maths_Multiplication>(context);
	RegisterIogramType<Maths_Division>(context);
	RegisterIogramType<Maths_GreaterThan>(context);
	RegisterIogramType<Maths_LessThan>(context);
	RegisterIogramType<Maths_RandomValue>(context);
	RegisterIogramType<Maths_DotProduct>(context);
	RegisterIogramType<Maths_CrossProduct>(context);
	RegisterIogramType<Maths_VectorLength>(context);
	RegisterIogramType<Maths_UnitizeVector>(context);
	RegisterIogramType<Maths_Lerp>(context);
	RegisterIogramType<Maths_RectangularGrid>(context);
	RegisterIogramType<Maths_RectangularArray>(context);
	RegisterIogramType<Maths_HexGrid>(context);
	//RegisterIogramType<Maths_RhodoArray3D>(context);
	RegisterIogramType<Maths_RhodoLattice>(context);
	RegisterIogramType<Geometry_Rotation>(context);
	RegisterIogramType<Geometry_LookAt>(context);
	//RegisterIogramType<Geometry_Transform>(context);
	RegisterIogramType<Geometry_AffineTransformation>(context);
	RegisterIogramType<Sets_Series>(context);
	RegisterIogramType<Sets_LogisticGrowthSeries>(context);
	RegisterIogramType<Sets_ListItem>(context);
	RegisterIogramType<Sets_IfThen>(context);
	RegisterIogramType<Sets_Merge>(context);
	RegisterIogramType<Sets_ListConstruct>(context);
	RegisterIogramType<Sets_ShiftList>(context);
	RegisterIogramType<Sets_LoopBegin>(context);
	RegisterIogramType<Sets_LoopEnd>(context);
	RegisterIogramType<Sets_DataRecorder>(context);
	RegisterIogramType<Sets_AddKeyValue>(context);
	RegisterIogramType<Sets_GetValueByKey>(context);
	RegisterIogramType<Sets_ExportViewData>(context);
	RegisterIogramType<Sets_ImportViewData>(context);
	RegisterIogramType<Maths_MassAverage>(context);
	RegisterIogramType<Maths_MassAddition>(context);
	RegisterIogramType<Input_Slider>(context);
	RegisterIogramType<Input_Panel>(context);
	RegisterIogramType<Input_Float>(context);
	RegisterIogramType<Input_Int>(context);
	RegisterIogramType<Input_Label>(context);
	RegisterIogramType<Input_Toggle>(context);
	RegisterIogramType<Input_ScreenToggle>(context);
	RegisterIogramType<Input_ScreenContainer>(context);
	RegisterIogramType<Input_CustomElement>(context);
	RegisterIogramType<Input_ButtonListener>(context);
	RegisterIogramType<Input_LineEditListener>(context);
	RegisterIogramType<Input_KeyboardListener>(context);
	RegisterIogramType<Input_MouseDownListener>(context);
	RegisterIogramType<Input_GamepadListener>(context);
	RegisterIogramType<Input_ScreenLineEdit>(context);
	RegisterIogramType<Input_ScreenText>(context);
	RegisterIogramType<Input_StringReplace>(context);
	RegisterIogramType<Input_StringAppend>(context);
	RegisterIogramType<Input_ScanDir>(context);
	RegisterIogramType<Input_Trigger>(context);
	RegisterIogramType<Input_ObjectMove>(context);
	RegisterIogramType<Input_Vector3>(context);
	RegisterIogramType<Input_ColorSlider>(context);
	RegisterIogramType<Input_GeometryEdit>(context);
	RegisterIogramType<Input_EditGeometryListener>(context);
	RegisterIogramType<Interop_SystemCommand>(context);
	RegisterIogramType<Interop_AsyncSystemCommand>(context);
	RegisterIogramType<Resource_LoadResource>(context);
	RegisterIogramType<Resource_CreateMaterial>(context);
	RegisterIogramType<Graphics_BaseSettings>(context);
	RegisterIogramType<Graphics_Viewport>(context);
	RegisterIogramType<Graphics_MeshRenderer>(context);
    RegisterIogramType<Graphics_MeshEdges>(context);
	RegisterIogramType<Graphics_CurveToModel>(context);
	RegisterIogramType<Graphics_CurveRenderer>(context);
	RegisterIogramType<Physics_ApplyForce>(context);
	RegisterIogramType<Physics_CollisionShape>(context);
	RegisterIogramType<Physics_Constraint>(context);
	RegisterIogramType<Scene_Display>(context);
	RegisterIogramType<Scene_DeconstructModel>(context);
	RegisterIogramType<Scene_AddNode>(context);
	RegisterIogramType<Scene_CloneNode>(context);
	RegisterIogramType<Scene_AddStaticModel>(context);
	RegisterIogramType<Scene_AnimatedModel>(context);
	RegisterIogramType<Scene_PlayAnimation>(context);
	RegisterIogramType<Scene_ScreenPointToRay>(context);
	RegisterIogramType<Scene_Raycast>(context);
	RegisterIogramType<Scene_ScreenBloom>(context);
	RegisterIogramType<Physics_PhysicsWorld>(context);
	RegisterIogramType<Physics_RigidBody>(context);
	RegisterIogramType<Scene_Text3D>(context);
	RegisterIogramType<Scene_ScriptInstance>(context);
	RegisterIogramType<Scene_MouseClickListener>(context);
	RegisterIogramType<Scene_SelectGeometry>(context);
	RegisterIogramType<Scene_ModifyNode>(context);
	RegisterIogramType<Scene_GetNode>(context);
	RegisterIogramType<Scene_GetComponent>(context);
	RegisterIogramType<Scene_AddComponent>(context);
	RegisterIogramType<Scene_ModifyComponent>(context);
	RegisterIogramType<Scene_SendEvent>(context);
	RegisterIogramType<Scene_HandleEvent>(context);
	RegisterIogramType<Scene_SetGlobalVar>(context);
	RegisterIogramType<Scene_GetGlobalVar>(context);
	RegisterIogramType<Scene_TriMeshVisualizeScalarField>(context);
	RegisterIogramType<Scene_LoadScene>(context);
	RegisterIogramType<Scene_SaveScene>(context);
	RegisterIogramType<Scene_AppendRenderPath>(context);
	RegisterIogramType<Mesh_DecimateMesh>(context);
	//RegisterIogramType<Scene_MeshToModel>(context);
	RegisterIogramType<Maths_ConstructTransform>(context);
	RegisterIogramType<Scene_DeconstructTransform>(context);
	RegisterIogramType<Vector_ConstructVector>(context);
	RegisterIogramType<Input_Inspect>(context);
	RegisterIogramType<Mesh_SmoothMesh>(context);
	RegisterIogramType<Input_ScreenSlider>(context);
	RegisterIogramType<Tree_Flatten>(context);
	RegisterIogramType<Tree_Graft>(context);
	RegisterIogramType<Tree_Reverse>(context);
	RegisterIogramType<Tree_Simplify>(context);
	RegisterIogramType<Tree_GetItem>(context);
	RegisterIogramType<Tree_FlipMatrix>(context);
	RegisterIogramType<Sets_ListLength>(context);
	RegisterIogramType<Sets_VariantMap>(context);
	RegisterIogramType<Mesh_ReadTriangleMesh>(context);
	RegisterIogramType<Mesh_DeconstructTriangleMesh>(context);
	RegisterIogramType<Mesh_ConstructTriangleMesh>(context);
	RegisterIogramType<Mesh_ClosestPoint>(context);
	RegisterIogramType<Mesh_HexayurtMesh>(context);
	RegisterIogramType<Mesh_CubeMesh>(context);
	RegisterIogramType<Mesh_Icosahedron>(context);
	RegisterIogramType<Mesh_Sphere>(context);
	RegisterIogramType<Mesh_SaveMesh>(context);
	RegisterIogramType<Mesh_CleanMesh>(context);
	RegisterIogramType<Mesh_BoundingBox>(context);
	RegisterIogramType<Mesh_HausdorffDistance>(context);
	RegisterIogramType<Mesh_HarmonicDeformation>(context);
//	RegisterIogramType<Mesh_FieldRemesh>(context);
//	RegisterIogramType<Curve_ZigZagPolyline>(context);
	RegisterIogramType<Curve_Polyline>(context);
	RegisterIogramType<Curve_OffsetPolyline>(context);
	RegisterIogramType<Curve_SmoothPolyline>(context);
	RegisterIogramType<Curve_LineSegment>(context);
	RegisterIogramType<Curve_SmoothPolyline>(context);
	RegisterIogramType<Curve_PolylineBlend>(context);
	RegisterIogramType<Curve_PolylineDivide>(context);
	RegisterIogramType<Curve_PolylineLoft>(context);
	RegisterIogramType<Curve_PolylineEvaluate>(context);
	RegisterIogramType<Curve_Polygon>(context);
	RegisterIogramType<Mesh_SubdivideMesh>(context);
	RegisterIogramType<Input_SliderListener>(context);
	RegisterIogramType<Vector_DeconstructVector>(context);
	RegisterIogramType<Maths_EvalFunction>(context);
	//RegisterIogramType<Input_ColorWheel>(context);
	RegisterIogramType<Vector_ClosestPoint>(context);
	RegisterIogramType<Vector_Distance>(context);
	RegisterIogramType<Vector_ColorRGBA>(context);
	RegisterIogramType<Vector_ColorPalette>(context);
	RegisterIogramType<Vector_BestFitPlane>(context);
	RegisterIogramType<Scene_UpdateListener>(context);
	RegisterIogramType<Mesh_Offset>(context);
	RegisterIogramType<Mesh_Window>(context);
	RegisterIogramType<Mesh_Frame>(context);
	RegisterIogramType<Mesh_Thicken>(context);
	RegisterIogramType<Mesh_LoopSubdivide>(context);
	RegisterIogramType<Mesh_ExtrudePolyline>(context);
	RegisterIogramType<Mesh_FillPolygon>(context);
	RegisterIogramType<Mesh_FacePolylines>(context);
	RegisterIogramType<Mesh_Boundary>(context);
	RegisterIogramType<Mesh_MeshPlaneIntersection>(context);
	RegisterIogramType<Mesh_JoinMeshes>(context);
	RegisterIogramType<Mesh_TriMeshVolume>(context);
	RegisterIogramType<Mesh_Tetrahedralize>(context);
	RegisterIogramType<Mesh_UnifyNormals>(context);
	RegisterIogramType<Mesh_AverageEdgeLength>(context);
	RegisterIogramType<Mesh_CollapseShortEdges>(context);
	RegisterIogramType<Mesh_MeanCurvatureFlow>(context);
	RegisterIogramType<Mesh_PerVertexEval>(context);
	RegisterIogramType<Spatial_ReadOSM>(context);
	RegisterIogramType<Mesh_SplitLongEdges>(context); //Disabled! Crashes on bad meshes like Hexayurt

	RegisterIogramType<Offsets_NgonMeshReader>(context);
}

template <class T> void ComponentRegistration::RegisterIogramType(Context* context)
{
	context->RegisterFactory<T>();

	Variant typeVar = context->GetGlobalVar("IogramTypes");
	VariantMap typeMap = typeVar.GetVariantMap();

	VariantMap infoMap;
	infoMap["typeName"] = T::GetTypeNameStatic();
	infoMap["viewTypeName"] = "NodeViewBase";
	infoMap["texturePath"] = T::iconTexture;	

	String tagString = T::GetTypeNameStatic().Split('_')[0];
	tagString.Replace(',', ' '); //get rid of commas
	Vector<String> parts = tagString.Split(' ');//split in to parts
	VariantVector globalTags = context->GetGlobalVar("IogramTags").GetVariantVector();
	String combinedString = "";
	for (unsigned i = 0; i < parts.Size(); i++)
	{
		String tag = parts[i].Trimmed();
		tag = tag.ToLower();

		if (!globalTags.Contains(tag))
		{
			globalTags.Push(tag);
		}

		combinedString += tag + " ";

	}
	infoMap["tags"] = combinedString;

	context->SetGlobalVar("IogramTags", globalTags);
	typeMap[T::GetTypeNameStatic()] = infoMap;
	context->SetGlobalVar("IogramTypes", typeMap);
}

template <class T, class V> void ComponentRegistration::RegisterIogramType(Context* context)
{
	context->RegisterFactory<T>();
	context->RegisterFactory<V>();

	Variant typeVar = context->GetGlobalVar("IogramTypes");
	VariantMap typeMap = typeVar.GetVariantMap();

	VariantMap infoMap;
	infoMap["typeName"] = T::GetTypeNameStatic();
	infoMap["viewTypeName"] = V::GetTypeNameStatic();
	infoMap["texturePath"] = T::iconTexture;
	typeMap[T::GetTypeNameStatic()] = infoMap;
	context->SetGlobalVar("IogramTypes", typeMap);

	//register tags
	T::SetTags(T::tags);

	int size = typeMap.Keys().Size();
}
