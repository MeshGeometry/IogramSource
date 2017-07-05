#pragma once

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/AngelScript/APITemplates.h>
#include <AngelScript/angelscript.h>

// These are not in Geomlib namespace, but IoGeometryAPI probably wants to include them
#include "StockGeometries.h"
#include "Polyline.h"

// For scripting. This function mimics the Geometry_WriteDxf component.
void WriteDxf(
	Urho3D::String dxfPath,
	bool forceZUp,
	Urho3D::String layer,
	Urho3D::Context* context,
	Urho3D::CScriptArray* meshes_arr,
	Urho3D::CScriptArray* polys_arr,
	Urho3D::CScriptArray* points_arr
);

void BestFitPlaneFromVariantArray(
	Urho3D::CScriptArray* point_cloud_arr,
	Urho3D::Vector3& point,
	Urho3D::Vector3& normal
);

void BestFitPlaneFromVector3Array(
	Urho3D::CScriptArray* point_cloud_arr,
	Urho3D::Vector3& point,
	Urho3D::Vector3& normal
);

Urho3D::Vector3 SegmentClosestPoint(
	const Urho3D::Vector3& A,
	const Urho3D::Vector3& B,
	const Urho3D::Vector3& Q
);

Urho3D::Vector3 TrianglePerimeterClosestPoint(
	const Urho3D::Vector3& A,
	const Urho3D::Vector3& B,
	const Urho3D::Vector3& C,
	const Urho3D::Vector3& Q
);

Urho3D::Vector3 TriangleClosestPoint(
	const Urho3D::Vector3& A,
	const Urho3D::Vector3& B,
	const Urho3D::Vector3& C,
	const Urho3D::Vector3& Q
);

Urho3D::Matrix3x4 ConstructTransformFromVariant(Urho3D::Variant var_in);
Urho3D::Matrix3x4 ConstructTransformFromVariantAndSource(Urho3D::Variant var_in, Urho3D::Matrix3x4 source);

Urho3D::Vector3 GeoToXYZ(
	double refLat,
	double refLon,
	double lat,
	double lon
);

Urho3D::CScriptArray* TransformVertexArray(const Urho3D::Matrix3x4& T, Urho3D::CScriptArray* vertexArray);

bool TriangulatePolygonWithNoHoles(const Urho3D::Variant& polyIn, Urho3D::Variant& polyOut);
bool TriangulatePolygonWithHoles(const Urho3D::Variant& polyIn, Urho3D::CScriptArray* holes, Urho3D::Variant& polyOut);

bool TriMesh_HausdorffDistance(
	const Urho3D::Variant& mesh1,
	const Urho3D::Variant& mesh2,
	float& distance
);

Urho3D::Vector3 Incenter(
	const Urho3D::Vector3& A,
	const Urho3D::Vector3& B,
	const Urho3D::Vector3& C
);

Urho3D::Variant JoinMeshes(Urho3D::CScriptArray* mesh_list_arr);

bool MeshTetrahedralize(
	const Urho3D::Variant& meshIn,
	float maxVolume,
	Urho3D::Variant& meshOut
);

bool PolylineBlend(
	const Urho3D::Variant& polyline1,
	const Urho3D::Variant& polyline2,
	Urho3D::Variant& mesh
);

bool PolylineDivide(
	const Urho3D::Variant& polylineIn,
	int n,
	Urho3D::Variant& polylineOut
);

bool PolylineExtrude(
	const Urho3D::Variant& polyIn,
	Urho3D::Variant& meshOut,
	Urho3D::Vector3 dir
);

bool PolylineLoft(
	Urho3D::CScriptArray* polylines_arr,
	Urho3D::Variant& mesh
);

bool PolylineOffset(
	const Urho3D::Variant& polyIn,
	Urho3D::Variant& polyOut,
	float offset
);

bool PolylinePointFromParameter(
	const Urho3D::Variant& polyline,
	float t,
	Urho3D::Vector3& point
);

bool PolylineTransformFromParameter(
	const Urho3D::Variant& polyline,
	float t,
	Urho3D::Matrix3x4& transform
);

bool GetVertexNormal(
	const Urho3D::Variant& polyline,
	Urho3D::Vector3& normal,
	int vert_id
);

bool PolylinePointToTransform(
	const Urho3D::Variant& polyline,
	int i,
	Urho3D::Variant& T
);

bool PolylineRefine(
	const Urho3D::Variant& polylineIn,
	Urho3D::CScriptArray* ts,
	Urho3D::Variant& polylineOut
);

bool PolylineRevolve(
	const Urho3D::Vector3& axis,
	const Urho3D::Variant& section,
	const int resolution,
	Urho3D::Variant& mesh
);

bool PolylineSweep_single(
	const Urho3D::Variant& rail,
	const Urho3D::Variant& section,
	Urho3D::Variant& mesh
);

bool PolylineSweep_double(
	const Urho3D::Variant& rail1,
	const Urho3D::Variant& rail2,
	const Urho3D::Variant& section,
	Urho3D::Variant& mesh
);

bool RayTriangleIntersection(
	const Urho3D::Vector3& A,
	const Urho3D::Vector3& B,
	const Urho3D::Vector3& C,
	const Urho3D::Vector3& O,
	const Urho3D::Vector3& D,
	float& s
);

bool ReadOBJFromFilename(
	const Urho3D::String& obj_filename,
	Urho3D::Variant& tri_mesh,
	bool yup
);

bool ReadOBJFromFile(
	Urho3D::File* source,
	Urho3D::Variant& tri_mesh,
	bool yup
);

bool ReadOFFFromFilename(
	const Urho3D::String& off_filename,
	Urho3D::Variant& tri_mesh,
	bool yup
);


bool ReadOFFFromFile(
	Urho3D::File* source,
	Urho3D::Variant& tri_mesh,
	bool yup
);

bool ReadPLYFromFilename(
	const Urho3D::String& ply_filename,
	Urho3D::Variant& tri_mesh,
	bool yup
);

bool ReadPLYFromFile(
	Urho3D::File* source,
	Urho3D::Variant& tri_mesh,
	bool yup
);

void RemoveDuplicatesVariantArray(
	Urho3D::CScriptArray* vertexVariantArray,
	Urho3D::Variant& vertices,
	Urho3D::Variant& indices
);

void RemoveDuplicatesVector3Array(
	Urho3D::CScriptArray* vertexVector3Array,
	Urho3D::Variant& vertices,
	Urho3D::Variant& faces
);

bool SmoothPolyline(
	const Urho3D::Variant& polylineIn,
	Urho3D::Variant& polylineOut
);

Urho3D::Vector3 ComputeSunPosition(
	float year,
	float month,
	float day,
	float hour,
	float tzone,
	float latit,
	float longit
);

float TriMeshAverageEdgeLength(const Urho3D::Variant& tri_mesh);

bool TriMeshClosestPoint(
	const Urho3D::Variant& mesh,
	const Urho3D::Vector3 q,
	int& index,
	Urho3D::Vector3& p
);

Urho3D::Variant TriMesh_CollapseShortEdges(
	const Urho3D::Variant& tri_mesh,
	float collapse_threshold
);

Urho3D::Variant TriMesh_SplitLongEdges(
	const Urho3D::Variant& tri_mesh,
	float split_threshold
);

bool TriMeshFrame(
	const Urho3D::Variant& meshIn,
	float t,
	Urho3D::Variant& meshOut
);

bool TriMeshLoopSubdivide(
	const Urho3D::Variant& meshIn,
	int steps,
	Urho3D::Variant& meshOut
);

Urho3D::Variant TriMesh_MeanCurvatureFlowStep(
	const Urho3D::Variant& tri_mesh
);

bool TriMesh_MeanCurvatureFlow(
	const Urho3D::Variant& tri_mesh,
	int num_steps,
	Urho3D::Variant& tri_mesh_out
);

bool TriMeshOffset(
	const Urho3D::Variant& meshIn,
	float d,
	Urho3D::Variant& meshOut
);

bool TriMeshOrientOutward(
	const Urho3D::Variant& mesh_in,
	Urho3D::Variant& mesh_out
);

bool TriMeshPlaneIntersection(
	const Urho3D::Variant& mesh,
	const Urho3D::Vector3& point,
	const Urho3D::Vector3& normal,
	Urho3D::Variant& mesh_normal_side,
	Urho3D::Variant& mesh_non_normal_side
);

bool TriMeshWriteOFF(
	const Urho3D::Variant& meshIn,
	Urho3D::File& destination
);

bool TriMeshSubdivide(
	const Urho3D::Variant& meshIn,
	int steps,
	Urho3D::Variant& meshOut
);

bool TriMeshThicken(
	const Urho3D::Variant& meshIn,
	float d,
	Urho3D::Variant& meshOut
);

float TriMeshVolume(const Urho3D::Variant& tri_mesh);

bool TriMeshWindow(
	const Urho3D::Variant& meshIn,
	float t,
	Urho3D::Variant& meshOut
);

bool WriteOBJ(
	const Urho3D::String& obj_filename,
	const Urho3D::Variant& tri_mesh,
	bool zup
);

bool WriteOFF(
	const Urho3D::String& off_filename,
	const Urho3D::Variant& tri_mesh,
	bool zup
);

bool WritePLY(
	const Urho3D::String& ply_filename,
	const Urho3D::Variant& tri_mesh,
	bool zup
);

//

void RegisterGeometryFunctions(Urho3D::Context* context);
