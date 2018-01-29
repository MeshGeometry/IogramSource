//
// Copyright (c) 2016 - 2017 Mesh Consultants Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#include "RegisterCoreComponents.h"
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
#include "Geometry_ReadDxf.h"
#include "Geometry_WriteDxf.h"
#include "Geometry_Reflection.h"
#include "Geometry_ReflectionFromTransform.h"
#include "Geometry_PlaneTransform.h"
#include "Geometry_ProjectOnto.h"
#include "Geometry_FillWithShape.h"
#include "Geometry_CubeTetLattice.h"
#include "Geometry_ReadTET.h"
#include "Sets_Series.h"
#include "Sets_LogisticGrowthSeries.h"
#include "Sets_ListItem.h"
#include "Sets_IfThen.h"
#include "Sets_Merge.h"
#include "Sets_ListConstruct.h"
#include "Sets_ShiftList.h"
#include "Sets_LoopBegin.h"
#include "Sets_LoopEnd.h"
#include "Sets_Heap.h"
#include "Sets_Freeze.h"
#include "Sets_Pop.h"
#include "Sets_NamedPair.h"
#include "Sets_AddKeyValue.h"
#include "Sets_GetValueByKey.h"
#include "Sets_ExportViewData.h"
#include "Sets_ImportViewData.h"
#include "Sets_DivideRange.h"
#include "Sets_DivideDomain3D.h"
#include "Maths_MassAverage.h"
#include "Maths_MassAddition.h"
#include "Input_Slider.h"
#include "Input_Panel.h"
#include "Input_Int.h"
#include "Input_Float.h"
//#include "Input_Label.h"
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
#include "Input_SketchPlane.h"
#include "Interop_SystemCommand.h"
#include "Interop_AsyncSystemCommand.h"
#include "Interop_JsonSchema.h"
#ifdef URHO3D_NETWORK
#include "Interop_SendData.h"
#include "Interop_ReceiveData.h"
#endif
#include "Graphics_LoadResource.h"
#include "Graphics_SaveResource.h"
#include "Graphics_CreateMaterial.h"
#include "Graphics_StandardMaterial.h"
#include "Graphics_ModifyMaterial.h"
#include "Graphics_Zone.h"
#include "Graphics_Viewport.h"
#include "Graphics_MeshRenderer.h"
#include "Graphics_PointRenderer.h"
#include "Graphics_CurveToModel.h"
#include "Graphics_CurveRenderer.h"
#include "Graphics_MeshEdges.h"
#include "Graphics_RenderTexture.h"
#include "Graphics_Texture2D.h"
#include "Graphics_Texture3D.h"
#include "Graphics_LayersToImage.h"
#include "Graphics_SampleTexture.h"
#include "Graphics_Light.h"
#include "Graphics_VertexColors.h"
#include "Graphics_Skybox.h"
#include "Graphics_Grid.h"
#include "Graphics_ReflectionProbe.h"
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
//#include "Mesh_ReadTriangleMesh.h"
#include "Mesh_DeconstructTriangleMesh.h"
#include "Mesh_ConstructTriangleMesh.h"
#include "Mesh_ClosestPoint.h"
#include "Mesh_HexayurtMesh.h"
#include "Mesh_CubeMesh.h"
#include "Mesh_Icosahedron.h"
#include "Mesh_Sphere.h"
#include "Mesh_Plane.h"
#include "Mesh_Pipe.h"
#include "Mesh_CubicLattice.h"
#include "Mesh_FieldRemesh.h"
#include "Mesh_SaveMesh.h"
#include "Mesh_CleanMesh.h"
#include "Mesh_BoundingBox.h"
#include "Mesh_HarmonicDeformation.h"
#include "Mesh_HausdorffDistance.h"
#include "Mesh_Voxelize.h"
#include "Mesh_ReadOBJ.h"
#include "Mesh_ReadOFF.h"
#include "Mesh_ReadPLY.h"
#include "Mesh_WriteOBJ.h"
#include "Mesh_WriteOFF.h"
#include "Mesh_WritePLY.h"
#include "Mesh_ReadSTL.h"
#include "Mesh_SignedDistance.h"
#include "Curve_ZigZagPolyline.h"
#include "Curve_HelixSpiral.h"
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
#include "Curve_SketchPlane.h"
#include "Curve_Rebuild.h"
#include "Curve_Length.h"
#include "Curve_PolylineSweep.h"
#include "Curve_PolylineRevolve.h"
#include "Curve_SelfIntersections.h"
#include "Curve_MakeKnot.h"
#include "Curve_Pipe.h"
//#include "Curve_ReadBagOfEdges.h"
#include "Mesh_SubdivideMesh.h"
#include "Input_SliderListener.h"
#include "Vector_DeconstructVector.h"
#include "Maths_EvalFunction.h"
#include "Maths_Expression.h"
#include "Input_ColorWheel.h"
#include "Vector_ClosestPoint.h"
#include "Vector_Distance.h"
#include "Vector_ColorRGBA.h"
#include "Vector_BestFitPlane.h"
#include "Vector_ColorPalette.h"
#include "Vector_Grid3D.h"
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
#include "Mesh_TriangulateNMesh.h"
#include "Mesh_Tetrahedralize.h"
#include "Mesh_TetLattice.h"
#include "Mesh_MeshPlaneIntersection.h"
#include "Mesh_AverageEdgeLength.h"
#include "Mesh_UnifyNormals.h"
#include "Mesh_SplitLongEdges.h"
#include "Mesh_CollapseShortEdges.h"
#include "Mesh_MeanCurvatureFlow.h"
#include "Mesh_PerVertexEval.h"
#include "Mesh_ComputeAdjacencyData.h"
#include "Mesh_VertexTopology.h"
#include "Mesh_FaceTopology.h"
#include "Mesh_BoundaryVertices.h"
#include "Mesh_DeconstructFace.h"
#include "Mesh_Torus.h"
#include "Mesh_Cylinder.h"
#include "Mesh_SuperEllipsoid.h"
#include "Mesh_FlipNormals.h"
#include "Mesh_OrientOutward.h"
#include "Mesh_ToYUp.h"
#include "Mesh_ToZUp.h"
#include "Mesh_MeshModeler.h"
#include "Mesh_LinearDeformation.h"
#include "Mesh_BoxMorph.h"
#include "Mesh_Remesh.h"
#include "Mesh_MarchingCubes.h"
#include "Mesh_SlideTowards.h"
#include "Curve_MeshSketch.h"
#include "Spatial_ReadOSM.h"
#include "Spatial_Terrain.h"
#include "Spatial_Sun.h"
#include "Spatial_NavigationMesh.h"
#include "Spatial_CrowdAgent.h"
#include "Spatial_CrowdManager.h"
#include "Spatial_AlignedDimension.h"
//#include "Offsets_NgonMeshReader.h"

#include "Tmp_TreeAccess.h"

#include "Widget_Base.h"
#include "Widget_OptionSlider.h"
#include "Widget_Container.h"
#include "Widget_TextLabel.h"
#include "Widget_Vector3Slider.h"
#include "Widget_ColorSlider.h"

#include "ShapeOp_Solve.h"
#include "ShapeOp_EdgeStrain.h"
#include "ShapeOp_TriangleStrain.h"
#include "ShapeOp_Closeness.h"
#include "ShapeOp_GeometryStrain.h"
#include "ShapeOp_MeshTriangleStrain.h"
#include "ShapeOp_GenericConstraint.h"
#include "ShapeOp_TetStrain.h"

#include "ReflectionProbe.h"
#include "ColorSlider.h"
#include "MultiSlider.h"
#include "TransformEdit.h"
#include "ModelEdit.h"
#include "ModelEditLinear.h"
#include "ModelBoxMorph.h"
#include "MeshSketcher.h"


using namespace Urho3D;

void RegisterCoreComponents(Context* context)
{

	context->RegisterFactory<Widget_Base>();
	context->RegisterFactory<Widget_OptionSlider>();
	context->RegisterFactory<Widget_Container>();
	context->RegisterFactory<Widget_TextLabel>();
	context->RegisterFactory<Widget_Vector3Slider>();
	context->RegisterFactory<Widget_ColorSlider>();

	context->RegisterFactory<ReflectionProbe>();
	context->RegisterFactory<ColorSlider>();
	context->RegisterFactory<MultiSlider>();
	context->RegisterFactory<TransformEdit>();
	context->RegisterFactory<ModelEdit>();
	context->RegisterFactory<ModelEditLinear>();
	context->RegisterFactory<ModelBoxMorph>();
	context->RegisterFactory<MeshSketcher>();

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
	RegisterIogramType<Geometry_ReadDXF>(context);
	RegisterIogramType<Geometry_WriteDXF>(context);
	RegisterIogramType<Geometry_Reflection>(context);
    RegisterIogramType<Geometry_ReflectionFromTransform>(context);
    RegisterIogramType<Geometry_PlaneTransform>(context);
	RegisterIogramType<Geometry_ProjectOnto>(context);
	RegisterIogramType<Geometry_FillWithShape>(context);
	RegisterIogramType<Geometry_CubeTetLattice>(context);
	RegisterIogramType<Geometry_ReadTET>(context);
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
	RegisterIogramType<Sets_Heap>(context);
	RegisterIogramType<Sets_Freeze>(context);
	RegisterIogramType<Sets_Pop>(context);
	RegisterIogramType<Sets_NamedPair>(context);
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
//	RegisterIogramType<Input_Label>(context);
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
	RegisterIogramType<Input_SketchPlane>(context);
	RegisterIogramType<Input_EditGeometryListener>(context);
	RegisterIogramType<Interop_SystemCommand>(context);
	RegisterIogramType<Interop_AsyncSystemCommand>(context);
	RegisterIogramType<Interop_JsonSchema>(context);
#ifdef URHO3D_NETWORK
	RegisterIogramType<Interop_SendData>(context);
	RegisterIogramType<Interop_ReceiveData>(context);
#endif
	RegisterIogramType<Graphics_LoadResource>(context);
	RegisterIogramType<Graphics_SaveResource>(context);
	RegisterIogramType<Graphics_CreateMaterial>(context);
	RegisterIogramType<Graphics_StandardMaterial>(context);
	RegisterIogramType<Graphics_ModifyMaterial>(context);
	RegisterIogramType<Graphics_Zone>(context);
	RegisterIogramType<Graphics_Viewport>(context);
	RegisterIogramType<Graphics_MeshRenderer>(context);
	RegisterIogramType<Graphics_PointRenderer>(context);
	RegisterIogramType<Graphics_MeshEdges>(context);
	RegisterIogramType<Graphics_CurveToModel>(context);
	RegisterIogramType<Graphics_CurveRenderer>(context);
	RegisterIogramType<Graphics_RenderTexture>(context);
	RegisterIogramType<Graphics_Texture2D>(context);
	RegisterIogramType<Graphics_Texture3D>(context);
	RegisterIogramType<Graphics_LayersToImage>(context);
	RegisterIogramType<Graphics_SampleTexture>(context);
	RegisterIogramType<Graphics_Light>(context);
	RegisterIogramType<Graphics_VertexColors>(context);
	RegisterIogramType<Graphics_Skybox>(context);
	RegisterIogramType<Graphics_Grid>(context);
	RegisterIogramType<Graphics_ReflectionProbe>(context);
	RegisterIogramType<Physics_ApplyForce>(context);
	RegisterIogramType<Physics_CollisionShape>(context);
	RegisterIogramType<Physics_Constraint>(context);
	//RegisterIogramType<Scene_Display>(context);
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
	RegisterIogramType<Sets_DivideRange>(context);
	RegisterIogramType<Sets_DivideDomain3D>(context);
	RegisterIogramType<Sets_VariantMap>(context);
	//RegisterIogramType<Mesh_ReadTriangleMesh>(context);
	RegisterIogramType<Mesh_DeconstructTriangleMesh>(context);
	RegisterIogramType<Mesh_ConstructTriangleMesh>(context);
	RegisterIogramType<Mesh_ClosestPoint>(context);
	RegisterIogramType<Mesh_HexayurtMesh>(context);
	RegisterIogramType<Mesh_CubeMesh>(context);
	RegisterIogramType<Mesh_Icosahedron>(context);
	RegisterIogramType<Mesh_Sphere>(context);
	RegisterIogramType<Mesh_Plane>(context);
    RegisterIogramType<Mesh_Cylinder>(context);
	RegisterIogramType<Mesh_Pipe>(context);
	RegisterIogramType<Mesh_CubicLattice>(context);
	RegisterIogramType<Mesh_SaveMesh>(context);
	RegisterIogramType<Mesh_CleanMesh>(context);
	RegisterIogramType<Mesh_BoundingBox>(context);
	RegisterIogramType<Mesh_HausdorffDistance>(context);
	RegisterIogramType<Mesh_HarmonicDeformation>(context);
	RegisterIogramType<Mesh_TriangulateNMesh>(context);
	RegisterIogramType<Mesh_MeshModeler>(context);
	RegisterIogramType<Mesh_SignedDistance>(context);
	//	RegisterIogramType<Mesh_FieldRemesh>(context);
	RegisterIogramType<Curve_ZigZagPolyline>(context);
    RegisterIogramType<Curve_HelixSpiral>(context);
    RegisterIogramType<Curve_PolylineSweep>(context);
    RegisterIogramType<Curve_Polyline>(context);
	RegisterIogramType<Curve_OffsetPolyline>(context);
	RegisterIogramType<Curve_SmoothPolyline>(context);
	RegisterIogramType<Curve_LineSegment>(context);
	RegisterIogramType<Curve_SmoothPolyline>(context);
	RegisterIogramType<Curve_PolylineBlend>(context);
	RegisterIogramType<Curve_PolylineDivide>(context);
	RegisterIogramType<Curve_PolylineLoft>(context);
	RegisterIogramType<Curve_PolylineEvaluate>(context);
	RegisterIogramType<Curve_PolylineRevolve>(context);
	RegisterIogramType<Curve_Pipe>(context);
	RegisterIogramType<Curve_Polygon>(context);
	RegisterIogramType<Curve_SketchPlane>(context);
	RegisterIogramType<Curve_MeshSketch>(context);
	RegisterIogramType<Curve_Rebuild>(context);
	RegisterIogramType<Curve_Length>(context);
	RegisterIogramType<Curve_SelfIntersections>(context);
    RegisterIogramType<Curve_MakeKnot>(context);
	//RegisterIogramType<Curve_ReadBagOfEdges>(context);
	RegisterIogramType<Mesh_SubdivideMesh>(context);
	RegisterIogramType<Input_SliderListener>(context);
	RegisterIogramType<Vector_DeconstructVector>(context);
	RegisterIogramType<Maths_EvalFunction>(context);
	RegisterIogramType<Maths_Expression>(context);
	//RegisterIogramType<Input_ColorWheel>(context);
	RegisterIogramType<Vector_ClosestPoint>(context);
	RegisterIogramType<Vector_Distance>(context);
	RegisterIogramType<Vector_ColorRGBA>(context);
	RegisterIogramType<Vector_ColorPalette>(context);
	RegisterIogramType<Vector_BestFitPlane>(context);
	RegisterIogramType<Vector_Grid3D>(context);
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
	RegisterIogramType<Mesh_TetLattice>(context);
	RegisterIogramType<Mesh_UnifyNormals>(context);
	RegisterIogramType<Mesh_AverageEdgeLength>(context);
	RegisterIogramType<Mesh_CollapseShortEdges>(context);
	RegisterIogramType<Mesh_MeanCurvatureFlow>(context);
	RegisterIogramType<Mesh_PerVertexEval>(context);
	RegisterIogramType<Mesh_ToYUp>(context);
	RegisterIogramType<Mesh_ToZUp>(context);
	RegisterIogramType<Spatial_ReadOSM>(context);
	RegisterIogramType<Spatial_Terrain>(context);
	RegisterIogramType<Spatial_Sun>(context);
	RegisterIogramType<Spatial_NavigationMesh>(context);
	RegisterIogramType<Spatial_CrowdManager>(context);
	RegisterIogramType<Spatial_CrowdAgent>(context);
	RegisterIogramType<Spatial_AlignedDimension>(context);
	RegisterIogramType<Mesh_SplitLongEdges>(context); //Disabled! Crashes on bad meshes like Hexayurt
    RegisterIogramType<Mesh_ComputeAdjacencyData>(context);
    RegisterIogramType<Mesh_FaceTopology>(context);
    RegisterIogramType<Mesh_VertexTopology>(context);
	RegisterIogramType<Mesh_BoundaryVertices>(context);
	RegisterIogramType<Mesh_DeconstructFace>(context);
	RegisterIogramType<Mesh_Torus>(context);
    RegisterIogramType<Mesh_SuperEllipsoid>(context);
    RegisterIogramType<Mesh_FlipNormals>(context);
	RegisterIogramType<Mesh_OrientOutward>(context);
	RegisterIogramType<Mesh_ReadOBJ>(context);
	RegisterIogramType<Mesh_ReadOFF>(context);
	RegisterIogramType<Mesh_ReadPLY>(context);
	RegisterIogramType<Mesh_ReadSTL>(context);
	RegisterIogramType<Mesh_WriteOFF>(context);
	RegisterIogramType<Mesh_WriteOBJ>(context);
	RegisterIogramType<Mesh_WritePLY>(context);
	RegisterIogramType<Mesh_Remesh>(context);
	RegisterIogramType<Mesh_SlideTowards>(context);
	RegisterIogramType<Mesh_LinearDeformation>(context);
	RegisterIogramType<Mesh_BoxMorph>(context);
	RegisterIogramType<Mesh_MarchingCubes>(context);
	RegisterIogramType<Mesh_Voxelize>(context);

	//RegisterIogramType<Offsets_NgonMeshReader>(context);

	RegisterIogramType<Tmp_TreeAccess>(context);

	RegisterIogramType<ShapeOp_Solve>(context);
	RegisterIogramType<ShapeOp_EdgeStrain>(context);
	RegisterIogramType<ShapeOp_TriangleStrain>(context);
	RegisterIogramType<ShapeOp_Closeness>(context);
	RegisterIogramType<ShapeOp_GeometryStrain>(context);
	RegisterIogramType<ShapeOp_MeshTriangleStrain>(context);
	RegisterIogramType<ShapeOp_GenericConstraint>(context);
	RegisterIogramType<ShapeOp_TetStrain>(context);
}
