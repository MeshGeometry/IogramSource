#include "IoGeometryAPI.h"

#include <stdio.h>
#include <iostream>


#include <Urho3D/AngelScript/Script.h>
#include <AngelScript/angelscript.h>

#include "DxfWriter.h"
#include "Geomlib_BestFitPlane.h"
#include "Geomlib_ClosestPoint.h"
#include "Geomlib_ConstructTransform.h"
#include "Geomlib_GeoConversions.h"
#include "Geomlib_HausdorffDistance.h"
#include "Geomlib_Incenter.h"
#include "Geomlib_JoinMeshes.h"
#include "Geomlib_MeshTetrahedralize.h"
#include "Geomlib_PolylineBlend.h"
#include "Geomlib_PolylineDivide.h"
#include "Geomlib_PolylineExtrude.h"
//TODO: #include "Geomlib_PolylineIntersection.h" --- currently there is no function here
#include "Geomlib_PolylineLoft.h"
#include "Geomlib_PolylineOffset.h"
#include "Geomlib_PolylinePointFromParameter.h"
#include "Geomlib_PolylinePointToTransform.h"
#include "Geomlib_PolylineRefine.h"
#include "Geomlib_PolylineRevolve.h"
#include "Geomlib_PolylineSweep.h"
//TODO: #include "Geomlib_PolylineTangentVector.h" --- currently there is no function here
#include "Geomlib_RayTriangleIntersection.h"
#include "Geomlib_ReadOBJ.h"
#include "Geomlib_ReadOFF.h"
#include "Geomlib_ReadPLY.h"
//TODO: #include "Geomlib_RebuildPolyline.h" --- not ready
//TODO: #include "Geomlib_Reflection.h" --- not ready
#include "Geomlib_RemoveDuplicates.h"
//#include "Geomlib_Rhodo.h" --- probably skip this one
#include "Geomlib_SmoothPolyline.h"
#include "Geomlib_SunPosition.h"
#include "Geomlib_TransformVertexList.h"
#include "Geomlib_TriangulatePolygon.h"
#include "Geomlib_TriMeshAverageEdgeLength.h"
#include "Geomlib_TriMeshClosestPoint.h"
#include "Geomlib_TriMeshEdgeCollapse.h"
#include "Geomlib_TriMeshEdgeSplit.h"
#include "Geomlib_TriMeshFrame.h"
#include "Geomlib_TriMeshLoopSubdivide.h"
#include "Geomlib_TriMeshMeanCurvatureFlow.h"
#include "Geomlib_TriMeshOffset.h"
#include "Geomlib_TriMeshOrientOutward.h"
#include "Geomlib_TriMeshPlaneIntersection.h"
#include "Geomlib_TriMeshSaveOFF.h"
#include "Geomlib_TriMeshSubdivide.h"
#include "Geomlib_TriMeshThicken.h"
#include "Geomlib_TriMeshVolume.h"
#include "Geomlib_TriMeshWindow.h"
//TODO: #include "Geomlib_TriTriIntersection.h" --- currently there is no function here
#include "Geomlib_WriteOBJ.h"
#include "Geomlib_WriteOFF.h"
#include "Geomlib_WritePLY.h"

#include "TriMesh.h"
#include "NMesh.h"
#include "MeshTopologyQueries.h"

#define CHECK_GEO_REG(result) if (result <= 0) { \
		printf("geo_reg: FAIL\n"); \
		failed = true; \
	}

using namespace Urho3D;

// For scripting. This function mimics the Geometry_WriteDxf component.
void WriteDxf(
	Urho3D::String dxfPath,
	bool forceZUp,
	Urho3D::String layer,
	Urho3D::Context* context,
	CScriptArray* meshes_arr,
	CScriptArray* polys_arr,
	CScriptArray* points_arr
)
{
	if (dxfPath.Empty()) {
		return;
	}
	int len = dxfPath.Length();
	if (
		len < 4 ||
		dxfPath.Substring(len - 4) != String(".dxf")
		)
	{
		dxfPath += ".dxf";
	}

	layer = layer.Empty() ? "Default" : layer;

	// create the writer
	DxfWriter* dWriter = new DxfWriter(context);

	VariantVector meshes = Urho3D::ArrayToVector<Variant>(meshes_arr);
	VariantVector polys = Urho3D::ArrayToVector<Variant>(polys_arr);
	VariantVector points = Urho3D::ArrayToVector<Variant>(points_arr);

	// get the geometry
	if (meshes.Size() == 1) {
		if (meshes[0].GetType() == VAR_NONE) {
			meshes = VariantVector();
		}
	}
	if (polys.Size() == 1) {
		if (polys[0].GetType() == VAR_NONE) {
			polys = VariantVector();
		}
	}
	if (points.Size() == 1) {
		if (points[0].GetType() == VAR_NONE) {
			points = VariantVector();
		}
	}

	// set the meshes
	for (int i = 0; i < meshes.Size(); i++)
	{
		VariantMap* mMap = meshes[i].GetVariantMapPtr();

		//only support triangle meshes right now
		if (TriMesh_Verify((*mMap))) {
			dWriter->SetMesh((*mMap)["vertices"].GetVariantVector(), (*mMap)["faces"].GetVariantVector(), layer);
		}
		else if (NMesh_Verify((*mMap))) {
			Variant triMesh = NMesh_ConvertToTriMesh(*mMap);
			dWriter->SetMesh(TriMesh_GetVertexList(triMesh), TriMesh_GetFaceList(triMesh), layer);
		}
	}

	// set the polylines
	for (int i = 0; i < polys.Size(); i++) {

		VariantMap* p = polys[i].GetVariantMapPtr();
		if (Polyline_Verify(*p)) {
			VariantVector verts = Polyline_ComputeSequentialVertexList(*p);
			dWriter->SetPolyline(verts, layer);
		}
	}

	// set the points
	dWriter->SetPoints(points, layer);

	//save
	dWriter->Save(dxfPath);
}

void BestFitPlaneFromVariantArray(
	Urho3D::CScriptArray* point_cloud_arr,
	Urho3D::Vector3& point,
	Urho3D::Vector3& normal
)
{
	Vector<Variant> point_cloud = ArrayToVector<Variant>(point_cloud_arr);
	Geomlib::BestFitPlane(point_cloud, point, normal);
}

void BestFitPlaneFromVector3Array(
	CScriptArray* point_cloud_arr,
	Urho3D::Vector3& point,
	Urho3D::Vector3& normal
)
{
	Vector<Vector3> point_cloud = ArrayToVector<Vector3>(point_cloud_arr);
	Geomlib::BestFitPlane(point_cloud, point, normal);
}

Urho3D::Vector3 SegmentClosestPoint(
	const Urho3D::Vector3& A,
	const Urho3D::Vector3& B,
	const Urho3D::Vector3& Q
)
{
	return Geomlib::SegmentClosestPoint(A, B, Q);
}

Urho3D::Vector3 TrianglePerimeterClosestPoint(
	const Urho3D::Vector3& A,
	const Urho3D::Vector3& B,
	const Urho3D::Vector3& C,
	const Urho3D::Vector3& Q
)
{
	return Geomlib::TrianglePerimeterClosestPoint(A, B, C, Q);
}

Urho3D::Vector3 TriangleClosestPoint(
	const Urho3D::Vector3& A,
	const Urho3D::Vector3& B,
	const Urho3D::Vector3& C,
	const Urho3D::Vector3& Q
)
{
	return Geomlib::TriangleClosestPoint(A, B, C, Q);
}

Urho3D::Matrix3x4 ConstructTransformFromVariant(Urho3D::Variant var_in)
{
	return Geomlib::ConstructTransform(var_in);
}

Urho3D::Matrix3x4 ConstructTransformFromVariantAndSource(Urho3D::Variant var_in, Urho3D::Matrix3x4 source)
{
	return Geomlib::ConstructTransform(var_in, source);
}

Urho3D::Vector3 GeoToXYZ(
	double refLat,
	double refLon,
	double lat,
	double lon
)
{
	return Geomlib::GeoToXYZ(refLat, refLon, lat, lon);
}

bool TriMesh_HausdorffDistance(
	const Urho3D::Variant& mesh1,
	const Urho3D::Variant& mesh2,
	float& distance
)
{
	return Geomlib::TriMesh_HausdorffDistance(mesh1, mesh2, distance);
}

Urho3D::Vector3 Incenter(
	const Urho3D::Vector3& A,
	const Urho3D::Vector3& B,
	const Urho3D::Vector3& C
)
{
	return Geomlib::Incenter(A, B, C);
}

Urho3D::Variant JoinMeshes(Urho3D::CScriptArray* mesh_list_arr)
{
	Vector<Variant> meshlist = ArrayToVector<Variant>(mesh_list_arr);
	return Geomlib::JoinMeshes(meshlist);
}

bool MeshTetrahedralize(
	const Urho3D::Variant& meshIn,
	float maxVolume,
	Urho3D::Variant& meshOut
)
{
	return Geomlib::MeshTetrahedralize(meshIn, maxVolume, meshOut);
}

bool PolylineBlend(
	const Urho3D::Variant& polyline1,
	const Urho3D::Variant& polyline2,
	Urho3D::Variant& mesh
)
{
	return Geomlib::PolylineBlend(polyline1, polyline2, mesh);
}

bool PolylineDivide(
	const Urho3D::Variant& polylineIn,
	int n,
	Urho3D::Variant& polylineOut
)
{
	return Geomlib::PolylineDivide(polylineIn, n, polylineOut);
}

bool PolylineExtrude(
	const Urho3D::Variant& polyIn,
	Urho3D::Variant& meshOut,
	Urho3D::Vector3 dir
)
{
	return Geomlib::PolylineExtrude(polyIn, meshOut, dir);
}

bool PolylineLoft(
	Urho3D::CScriptArray* polylines_arr,
	Urho3D::Variant& mesh
)
{
	Vector<Variant> polylines = ArrayToVector<Variant>(polylines_arr);
	return Geomlib::PolylineLoft(polylines, mesh);
}

bool PolylineOffset(
	const Urho3D::Variant& polyIn,
	Urho3D::Variant& polyOut,
	float offset
)
{
	return Geomlib::PolylineOffset(polyIn, polyOut, offset);
}

bool PolylinePointFromParameter(
	const Urho3D::Variant& polyline,
	float t,
	Urho3D::Vector3& point
)
{
	return Geomlib::PolylinePointFromParameter(polyline, t, point);
}

bool PolylineTransformFromParameter(
	const Urho3D::Variant& polyline,
	float t,
	Urho3D::Matrix3x4& transform
)
{
	return Geomlib::PolylineTransformFromParameter(polyline, t, transform);
}

bool GetVertexNormal(
	const Urho3D::Variant& polyline,
	Urho3D::Vector3& normal,
	int vert_id
)
{
	return Geomlib::GetVertexNormal(polyline, normal, vert_id);
}

bool PolylinePointToTransform(
	const Urho3D::Variant& polyline,
	int i,
	Urho3D::Variant& T
)
{
	return Geomlib::PolylinePointToTransform(polyline, i, T);
}

bool PolylineRefine(
	const Urho3D::Variant& polylineIn,
	Urho3D::CScriptArray* ts_arr,
	Urho3D::Variant& polylineOut
)
{
	Vector<float> ts = ArrayToVector<float>(ts_arr);

	return Geomlib::PolylineRefine(polylineIn, ts, polylineOut);
}

bool PolylineRevolve(
	const Urho3D::Vector3& axis,
	const Urho3D::Variant& section,
	const int resolution,
	Urho3D::Variant& mesh
)
{
	return Geomlib::PolylineRevolve(axis, section, resolution, mesh);
}

bool PolylineSweep_single(
	const Urho3D::Variant& rail,
	const Urho3D::Variant& section,
	Urho3D::Variant& mesh
)
{
	return Geomlib::PolylineSweep_single(rail, section, mesh);
}

bool PolylineSweep_double(
	const Urho3D::Variant& rail1,
	const Urho3D::Variant& rail2,
	const Urho3D::Variant& section,
	Urho3D::Variant& mesh
)
{
	return Geomlib::PolylineSweep_double(rail1, rail2, section, mesh);
}

bool RayTriangleIntersection(
	const Urho3D::Vector3& A,
	const Urho3D::Vector3& B,
	const Urho3D::Vector3& C,
	const Urho3D::Vector3& O,
	const Urho3D::Vector3& D,
	float& s
)
{
	return Geomlib::RayTriangleIntersection(A, B, C, O, D, s);
}

bool ReadOBJFromFilename(
	const Urho3D::String& obj_filename,
	Urho3D::Variant& tri_mesh,
	bool yup
)
{
	return Geomlib::ReadOBJ(obj_filename, tri_mesh, yup);
}

bool ReadOBJFromFile(
	Urho3D::File* source,
	Urho3D::Variant& tri_mesh,
	bool yup
)
{
	return Geomlib::ReadOBJ(source, tri_mesh, yup);
}

bool ReadOFFFromFilename(
	const Urho3D::String& ply_filename,
	Urho3D::Variant& tri_mesh,
	bool yup
)
{
	return Geomlib::ReadOFF(ply_filename, tri_mesh, yup);
}

bool ReadOFFFromFile(
	Urho3D::File* source,
	Urho3D::Variant& tri_mesh,
	bool yup
)
{
	return Geomlib::ReadOFF(source, tri_mesh, yup);
}

bool ReadPLYFromFilename(
	const Urho3D::String& ply_filename,
	Urho3D::Variant& tri_mesh,
	bool yup
)
{
	return Geomlib::ReadPLY(ply_filename, tri_mesh, yup);
}

bool ReadPLYFromFile(
	Urho3D::File* source,
	Urho3D::Variant& tri_mesh,
	bool yup
)
{
	return Geomlib::ReadPLY(source, tri_mesh, yup);
}

void RemoveDuplicatesVariantArray(
	Urho3D::CScriptArray* vertexVariantArray,
	Urho3D::Variant& vertices,
	Urho3D::Variant& indices
)
{
	Vector<Variant> vertexList = Urho3D::ArrayToVector<Variant>(vertexVariantArray);
	Geomlib::RemoveDuplicates(vertexList, vertices, indices);
}

void RemoveDuplicatesVector3Array(
	Urho3D::CScriptArray* vertexVector3Array,
	Urho3D::Variant& vertices,
	Urho3D::Variant& faces
)
{
	Vector<Vector3> vertexList = Urho3D::ArrayToVector<Vector3>(vertexVector3Array);
	Geomlib::RemoveDuplicates(vertexList, vertices, faces);
}

bool SmoothPolyline(
	const Urho3D::Variant& polylineIn,
	Urho3D::Variant& polylineOut
)
{
	return Geomlib::SmoothPolyline(polylineIn, polylineOut);
}

Urho3D::Vector3 ComputeSunPosition(
	float year,
	float month,
	float day,
	float hour,
	float tzone,
	float latit,
	float longit
)
{
	return Geomlib::ComputeSunPosition(year, month, day, hour, tzone, latit, longit);
}

Urho3D::CScriptArray* TransformVertexArray(const Urho3D::Matrix3x4& T, Urho3D::CScriptArray* vertexArray)
{
	Vector<Variant> vertexList = ArrayToVector<Variant>(vertexArray);
	Vector<Variant> trVertexList = Geomlib::TransformVertexList(T, vertexList);
	return Urho3D::VectorToArray<Variant>(trVertexList, "Array<Variant>");
}

bool TriangulatePolygonWithNoHoles(const Urho3D::Variant& polyIn, Urho3D::Variant& polyOut)
{
	return Geomlib::TriangulatePolygon(polyIn, polyOut);
}

bool TriangulatePolygonWithHoles(const Urho3D::Variant& polyIn, Urho3D::CScriptArray* holes, Urho3D::Variant& polyOut)
{
	Vector<Variant> holes_list = Urho3D::ArrayToVector<Variant>(holes);
	return Geomlib::TriangulatePolygon(polyIn, holes_list, polyOut);
}

float TriMeshAverageEdgeLength(const Urho3D::Variant& tri_mesh)
{
	return Geomlib::TriMeshAverageEdgeLength(tri_mesh);
}

bool TriMeshClosestPoint(
	const Urho3D::Variant& mesh,
	const Urho3D::Vector3 q,
	int& index,
	Urho3D::Vector3& p
)
{
	return Geomlib::TriMeshClosestPoint(mesh, q, index, p);
}

Urho3D::Variant TriMesh_CollapseShortEdges(
	const Urho3D::Variant& tri_mesh,
	float collapse_threshold
)
{
	return Geomlib::TriMesh_CollapseShortEdges(tri_mesh, collapse_threshold);
}

Urho3D::Variant TriMesh_SplitLongEdges(
	const Urho3D::Variant& tri_mesh,
	float split_threshold
)
{
	return Geomlib::TriMesh_SplitLongEdges(tri_mesh, split_threshold);
}

bool TriMeshFrame(
	const Urho3D::Variant& meshIn,
	float t,
	Urho3D::Variant& meshOut
)
{
	return Geomlib::TriMeshFrame(meshIn, t, meshOut);
}

bool TriMeshLoopSubdivide(
	const Urho3D::Variant& meshIn,
	int steps,
	Urho3D::Variant& meshOut
)
{
	return Geomlib::TriMeshLoopSubdivide(meshIn, steps, meshOut);
}

Urho3D::Variant TriMesh_MeanCurvatureFlowStep(
	const Urho3D::Variant& tri_mesh
)
{
	return Geomlib::TriMesh_MeanCurvatureFlowStep(tri_mesh);
}

bool TriMesh_MeanCurvatureFlow(
	const Urho3D::Variant& tri_mesh,
	int num_steps,
	Urho3D::Variant& tri_mesh_out
)
{
	return Geomlib::TriMesh_MeanCurvatureFlow(tri_mesh, num_steps, tri_mesh_out);
}

bool TriMeshOffset(
	const Urho3D::Variant& meshIn,
	float d,
	Urho3D::Variant& meshOut
)
{
	return Geomlib::TriMeshOffset(meshIn, d, meshOut);
}

bool TriMeshOrientOutward(
	const Urho3D::Variant& mesh_in,
	Urho3D::Variant& mesh_out
)
{
	return Geomlib::TriMeshOrientOutward(mesh_in, mesh_out);
}

bool TriMeshPlaneIntersection(
	const Urho3D::Variant& mesh,
	const Urho3D::Vector3& point,
	const Urho3D::Vector3& normal,
	Urho3D::Variant& mesh_normal_side,
	Urho3D::Variant& mesh_non_normal_side
)
{
	return Geomlib::TriMeshPlaneIntersection(mesh, point, normal, mesh_normal_side, mesh_non_normal_side);
}

bool TriMeshWriteOFF(
	const Urho3D::Variant& meshIn,
	Urho3D::File& destination
)
{
	return Geomlib::TriMeshWriteOFF(meshIn, destination);
}

bool TriMeshSubdivide(
	const Urho3D::Variant& meshIn,
	int steps,
	Urho3D::Variant& meshOut
)
{
	return Geomlib::TriMeshSubdivide(meshIn, steps, meshOut);
}

bool TriMeshThicken(
	const Urho3D::Variant& meshIn,
	float d,
	Urho3D::Variant& meshOut
)
{
	return Geomlib::TriMeshThicken(meshIn, d, meshOut);
}

float TriMeshVolume(const Urho3D::Variant& tri_mesh)
{
	return Geomlib::TriMeshVolume(tri_mesh);
}

bool TriMeshWindow(
	const Urho3D::Variant& meshIn,
	float t,
	Urho3D::Variant& meshOut
)
{
	return Geomlib::TriMeshWindow(meshIn, t, meshOut);
}

bool WriteOBJ(
	const Urho3D::String& obj_filename,
	const Urho3D::Variant& tri_mesh,
	bool zup
)
{
	return Geomlib::WriteOBJ(obj_filename, tri_mesh, zup);
}

bool WriteOFF(
	const Urho3D::String& off_filename,
	const Urho3D::Variant& tri_mesh,
	bool zup
)
{
	return Geomlib::WriteOFF(off_filename, tri_mesh, zup);
}

bool WritePLY(
	const Urho3D::String& ply_filename,
	const Urho3D::Variant& tri_mesh,
	bool zup
)
{
	return Geomlib::WritePLY(ply_filename, tri_mesh, zup);
}

void RegisterGeometryFunctions(Urho3D::Context* context)
{
	Script* script_system = context->GetSubsystem<Script>();
	asIScriptEngine* engine = script_system->GetScriptEngine();

	bool failed = false;

	int res;
	/*
	res = engine->RegisterGlobalFunction(
		"void WriteDxf(String, bool, String, Context@, Array<Variant>@, Array<Variant>@, Array<Variant>@)",
		asFUNCTION(WriteDxf),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	*/

	res = engine->RegisterGlobalFunction(
		"void BestFitPlaneFromVariantArray(Array<Variant>@, Vector3&, Vector3&)",
		asFUNCTION(BestFitPlaneFromVariantArray),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"void BestFitPlaneFromVector3Array(Array<Vector3>@, Vector3&, Vector3&)",
		asFUNCTION(BestFitPlaneFromVector3Array),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Vector3 SegmentClosestPoint(const Vector3&, const Vector3&, const Vector3&)",
		asFUNCTION(SegmentClosestPoint),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Vector3 TrianglePerimeterClosestPoint(const Vector3&, const Vector3&, const Vector3&, const Vector3&)",
		asFUNCTION(TrianglePerimeterClosestPoint),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Vector3 TriangleClosestPoint(const Vector3&, const Vector3&, const Vector3&, const Vector3&)",
		asFUNCTION(TriangleClosestPoint),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Matrix3x4 ConstructTransformFromVariant(Variant)",
		asFUNCTION(ConstructTransformFromVariant),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"Matrix3x4 ConstructTransformFromVariantAndSource(Variant, Matrix3x4)",
		asFUNCTION(ConstructTransformFromVariantAndSource),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Vector3 GeoToXYZ(double, double, double, double)",
		asFUNCTION(GeoToXYZ),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"bool TriMesh_HausdorffDistance(const Variant&, const Variant&, float&)",
		asFUNCTION(TriMesh_HausdorffDistance),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Vector3 Incenter(const Vector3&, const Vector3&, const Vector3&)",
		asFUNCTION(Incenter),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Variant JoinMeshes(Array<Variant>@)",
		asFUNCTION(JoinMeshes),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool MeshTetrahedralize(const Variant&, float, Variant&)",
		asFUNCTION(MeshTetrahedralize),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool PolylineBlend(const Variant&, const Variant&, Variant&)",
		asFUNCTION(PolylineBlend),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool PolylineDivide(const Variant&, int, Variant&)",
		asFUNCTION(PolylineDivide),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool PolylineExtrude(const Variant&, Variant&, Vector3)",
		asFUNCTION(PolylineExtrude),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool PolylineLoft(Array<Variant>@, Variant&)",
		asFUNCTION(PolylineLoft),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool PolylineOffset(const Variant&, Variant&, float)",
		asFUNCTION(PolylineOffset),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool PolylinePointFromParameter(const Variant&, float, Vector3&)",
		asFUNCTION(PolylinePointFromParameter),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"bool PolylineTransformFromParameter(const Variant&, float, Matrix3x4&)",
		asFUNCTION(PolylineTransformFromParameter),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"bool GetVertexNormal(const Variant&, Vector3&, int)",
		asFUNCTION(GetVertexNormal),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"bool PolylinePointToTransform(const Variant&, int, Variant&)",
		asFUNCTION(PolylinePointToTransform),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"bool PolylineRefine(const Variant&, Array<float>@, Variant&)",
		asFUNCTION(PolylineRefine),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"bool PolylineSweep_single(const Variant&, const Variant&, Variant&)",
		asFUNCTION(PolylineSweep_single),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"bool PolylineSweep_double(const Variant&, const Variant&, const Variant&, Variant&)",
		asFUNCTION(PolylineSweep_double),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	// Read*** functions

	res = engine->RegisterGlobalFunction(
		"bool RayTriangleIntersection(const Vector3&, const Vector3&, const Vector3&, const Vector3&, const Vector3&, float&)",
		asFUNCTION(RayTriangleIntersection),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"bool ReadOBJFromFilename(const String&, Variant&, bool)",
		asFUNCTION(ReadOBJFromFilename),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"bool ReadOBJFromFile(File@, Variant&, bool)",
		asFUNCTION(ReadOBJFromFile),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool ReadOFFFromFilename(const String&, Variant&, bool)",
		asFUNCTION(ReadOFFFromFilename),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool ReadOFFFromFile(File@, Variant&, bool)",
		asFUNCTION(ReadOFFFromFile),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool ReadPLYFromFilename(const String&, Variant&, bool)",
		asFUNCTION(ReadPLYFromFilename),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool ReadPLYFromFile(File@, Variant&, bool)",
		asFUNCTION(ReadPLYFromFile),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"void RemoveDuplicatesVariantArray(Array<Variant>@, Variant&, Variant&)",
		asFUNCTION(RemoveDuplicatesVariantArray),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"void RemoveDuplicatesVector3Array(Array<Vector3>@, Variant&, Variant&)",
		asFUNCTION(RemoveDuplicatesVector3Array),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"bool SmoothPolyline(const Variant&, Variant&)",
		asFUNCTION(SmoothPolyline),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"Vector3 ComputeSunPosition(float, float, float, float, float, float, float)",
		asFUNCTION(ComputeSunPosition),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Array<Variant>@ TransformVertexArray(const Matrix3x4&, Array<Variant>@)",
		asFUNCTION(TransformVertexArray),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"bool TriangulatePolygonWithNoHoles(const Variant&, Variant&)",
		asFUNCTION(TriangulatePolygonWithNoHoles),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);
	res = engine->RegisterGlobalFunction(
		"bool TriangulatePolygonWithHoles(const Variant&, Array<Variant>@, Variant&)",
		asFUNCTION(TriangulatePolygonWithHoles),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"float TriMeshAverageEdgeLength(const Variant&)",
		asFUNCTION(TriMeshAverageEdgeLength),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"bool TriMeshClosestPoint(const Variant&, const Vector3, int&, Vector3&)",
		asFUNCTION(TriMeshClosestPoint),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Variant TriMesh_CollapseShortEdges(const Variant&, float)",
		asFUNCTION(TriMesh_CollapseShortEdges),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"Variant TriMesh_SplitLongEdges(const Variant&, float)",
		asFUNCTION(TriMesh_SplitLongEdges),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	res = engine->RegisterGlobalFunction(
		"bool TriMeshFrame(const Variant&, float, Variant&)",
		asFUNCTION(TriMeshFrame),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool TriMeshLoopSubdivide(const Variant&, int, Variant&)",
		asFUNCTION(TriMeshLoopSubdivide),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"Variant TriMesh_MeanCurvatureFlowStep(const Variant&)",
		asFUNCTION(TriMesh_MeanCurvatureFlowStep),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool TriMesh_MeanCurvatureFlow(const Variant&, int, Variant&)",
		asFUNCTION(TriMesh_MeanCurvatureFlow),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool TriMeshOffset(const Variant&, float, Variant&)",
		asFUNCTION(TriMeshOffset),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool TriMeshOrientOutward(const Variant&, Variant&)",
		asFUNCTION(TriMeshOrientOutward),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool TriMeshPlaneIntersection(const Variant&, const Vector3&, const Vector3&, Variant&, Variant&)",
		asFUNCTION(TriMeshPlaneIntersection),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool TriMeshWriteOFF(const Variant&, File&)",
		asFUNCTION(TriMeshWriteOFF),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool TriMeshSubdivide(const Variant&, int, Variant&)",
		asFUNCTION(TriMeshSubdivide),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool TriMeshThicken(const Variant&, float, Variant&)",
		asFUNCTION(TriMeshThicken),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"float TriMeshVolume(const Variant&)",
		asFUNCTION(TriMeshVolume),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool TriMeshWindow(const Variant&, float, Variant&)",
		asFUNCTION(TriMeshWindow),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool WriteOBJ(const String&, const Variant&, bool)",
		asFUNCTION(WriteOBJ),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool WriteOFF(const String&, const Variant&, bool)",
		asFUNCTION(WriteOFF),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res)

	res = engine->RegisterGlobalFunction(
		"bool WritePLY(const String&, const Variant&, bool)",
		asFUNCTION(WritePLY),
		asCALL_CDECL
	);
	CHECK_GEO_REG(res);

	bool stockgeometry_res = RegisterStockGeometryFunctions(context);
	bool polyline_res = RegisterPolylineFunctions(context);
	bool nmesh_res = RegisterNMeshFunctions(context);
	bool trimesh_res = RegisterTriMeshFunctions(context);
	bool meshtopologyqueries_res = RegisterMeshTopologyQueryFunctions(context);

	if (failed)
	{
		URHO3D_LOGINFO("RegisterGeometryFunctions -- Failed to compile scripts!");
	}
	else {
		URHO3D_LOGINFO("RegisterGeometryFunctions -- OK!");
	}
}