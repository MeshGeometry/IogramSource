#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/IO/Deserializer.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>

using namespace Urho3D;

typedef Pair<int, Variant> LinePair;

URHO3D_API class DxfWriter : public Object
{
	URHO3D_OBJECT(DxfWriter, Object);

public:
	DxfWriter(Context* context);
	~DxfWriter() {};

	/**************************************************************************
	Dxf info comes in pairs of lines, eg:
	---- 0         <- code id
	---- HEADER    <- value for this code
	***************************************************************************/
	bool WriteLinePair(int code, String value);

	//main loop for writing
	bool Save(String path);

	//setters
	void SetMesh(Vector<Vector3> vertices, Vector<int> indices, String layer = "Default");
	void SetMesh(VariantVector vertices, VariantVector indices, String layer = "Default");
	void SetMesh(VariantVector meshes, String layer = "Default");
	void SetPolyline(Vector<Vector3> vertices, String layer = "Default");
	void SetPolyline(VariantVector vertices, String layer = "Default");
	//void SetPolyline(VariantVector polylines, String layer = "Default");
	void SetPoint(Vector3 point, String layer = "Default");
	void SetPoints(Vector<Vector3> points, String layer = "Default");
	void SetPoints(VariantVector points, String layer = "Default");

	void SetForceZUp(bool ZUp) { forceZUp_ = ZUp; }
	bool GetForceZUp() { return forceZUp_; }

protected:

	bool forceZUp_;

	//These are the things we want. 
	VariantVector meshes_;
	VariantVector polylines_;
	VariantVector points_;

	//writers
	void WriteHeader();
	void WriteEntities();
	void WriteMesh(int id);
	void WritePolyline(int id);
	void WritePoint(int id);
	void WriteVertex(Vector3 vertex, String layer);
	void WriteIndices(int a, int b, int c, String layer);

};