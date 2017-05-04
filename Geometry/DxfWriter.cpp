#include "DxfWriter.h"
#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/IO/Log.h>

namespace {

	File* dest_;
}

DxfWriter::DxfWriter(Context* context) : Object(context),
	forceZUp_(true)
{

}

bool DxfWriter::Save(String path)
{

	FileSystem* fs = new FileSystem(GetContext());

	//create the file
	dest_ = new File(GetContext(), path, FILE_WRITE);

	//double check
	assert(dest_);

	//create the log
	GetContext()->RegisterSubsystem(new Log(GetContext()));

	//oepn
	WriteHeader();

	//write the objects
	WriteEntities();

	//close
	WriteLinePair(0, "EOF");

	dest_->Close();

	return false;
}

//writers
bool DxfWriter::WriteLinePair(int code, String value)
{

	if (!dest_)
		return false;

	String codeString = "   "; //always write three digits with spaces for unused.
	String incoming = String(code);

	switch (incoming.Length())
	{
	case 1:
		codeString[2] = incoming[0];
		break;
	case 2:
		codeString[1] = incoming[0];
		codeString[2] = incoming[1];
		break;
	case 3:
		codeString[0] = incoming[0];
		codeString[1] = incoming[1];
		codeString[2] = incoming[2];
		break;
	}

	dest_->WriteLine(codeString);
	dest_->WriteLine(value);

	return true;
}


void DxfWriter::WriteHeader()
{
	//opener
	WriteLinePair(0, "SECTION");
	WriteLinePair(2, "HEADER");

	WriteLinePair(9, "$ACADVER");
	WriteLinePair(1, "AC1009");


	WriteLinePair(9, "$INSBASE");
	WriteLinePair(10, String(0.0));
	WriteLinePair(20, String(0.0));
	WriteLinePair(30, String(0.0));

	WriteLinePair(9, "$EXTMIN");
	WriteLinePair(10, String(0.0));
	WriteLinePair(20, String(0.0));
	WriteLinePair(30, String(0.0));

	WriteLinePair(9, "$EXTMAX");
	WriteLinePair(10, String(0.0));
	WriteLinePair(20, String(0.0));
	WriteLinePair(30, String(0.0));

	//closer
	WriteLinePair(0, "ENDSEC");
}

void DxfWriter::WriteEntities()
{
	//ENTITIES opener
	WriteLinePair(0, "SECTION");
	WriteLinePair(2, "ENTITIES");

	//write meshes
	for (int i = 0; i < meshes_.Size(); i++)
	{
		WriteMesh(i);
	}

	//write
	for (int i = 0; i < polylines_.Size(); i++)
	{
		WritePolyline(i);
	}

	for (int i = 0; i < points_.Size(); i++)
	{
		WritePoint(i);
	}

	//ENTITIES closer
	WriteLinePair(0, "ENDSEC");
}

void DxfWriter::WriteVertex(Vector3 vertex, String layer)
{
	if (forceZUp_) {
		vertex = Vector3(vertex.x_, vertex.z_, vertex.y_);
	}
	
	
	WriteLinePair(0, "VERTEX");
	//WriteLinePair(100, "AcDbEntity");
	//WriteLinePair(100, "AcDbVertex");
	WriteLinePair(8, layer);
	WriteLinePair(10, String(vertex.x_));
	WriteLinePair(20, String(vertex.y_));
	WriteLinePair(30, String(vertex.z_));
	WriteLinePair(70, String(192)); //flag that specifies this as mesh or polygon vertex

}

void DxfWriter::WriteIndices(int a, int b, int c, String layer)
{
	//face indices are stored as a vertex structure.
	//here we zero out the position, and just write the indices.

	WriteLinePair(0, "VERTEX");

	WriteLinePair(8, layer);
	WriteLinePair(10, String(0.0));
	WriteLinePair(20, String(0.0));
	WriteLinePair(30, String(0.0));

	//keep the right flag
	WriteLinePair(70, String(128)); //flag that specifies this as mesh or polygon vertex

	//the indices
	WriteLinePair(71, String(a));
	WriteLinePair(72, String(b));
	WriteLinePair(73, String(c));
	WriteLinePair(74, String(c));
}

void DxfWriter::WriteMesh(int id)
{
	if (id >= meshes_.Size())
		return;

	VariantMap* pMap = meshes_[id].GetVariantMapPtr();
	if ((*pMap).Keys().Contains("000_TYPE")) {
		if ((*pMap)["000_TYPE"].GetString() == "MESH")
		{
			int flags = (*pMap).Keys().Contains("Flags") ? (*pMap)["Flags"].GetInt() : 64;
			String layer = (*pMap).Keys().Contains("Layer") ? (*pMap)["Layer"].GetString() : "Default";
			VariantVector verts = (*pMap)["Vertices"].GetVariantVector();
			VariantVector faces = (*pMap)["Faces"].GetVariantVector();

			//make sure that we have some vertices
			if (verts.Empty() || faces.Empty()) {
				return;
			}

			//write header
			WriteLinePair(0, "POLYLINE");
			WriteLinePair(8, layer);
			WriteLinePair(70, String(flags));
			WriteLinePair(66, String(1));
			WriteLinePair(10, String(0.0));
			WriteLinePair(20, String(0.0));
			WriteLinePair(30, String(0.0));

			//WriteLinePair(71, String(3));
			//WriteLinePair(72, String(1));

			//write vertices
			for (int i = 0; i < verts.Size(); i++)
			{
				Vector3 v = verts.At(i).GetVector3();
				WriteVertex(v, layer);
			}

			//write indices
			//assume this is a trimesh
			for (int i = 0; i < faces.Size()/3; i++)
			{
				//face list is 1-based, fools!
				int a = faces[3 * i + 0].GetInt() + 1;
				int b = faces[3 * i + 1].GetInt() + 1;
				int c = faces[3 * i + 2].GetInt() + 1;
				WriteIndices(a, b, c, layer);

			}

			WriteLinePair(0, "SEQEND");
		}
	}
}

void DxfWriter::WritePolyline(int id)
{
	if (id >= polylines_.Size())
		return;

	VariantMap* pMap = polylines_[id].GetVariantMapPtr();
	if ((*pMap).Keys().Contains("000_TYPE")) {
		if ((*pMap)["000_TYPE"].GetString() == "POLYLINE")
		{
			int flags = (*pMap).Keys().Contains("Flags") ? (*pMap)["Flags"].GetInt() : 8;
			String layer = (*pMap).Keys().Contains("Layer") ? (*pMap)["Layer"].GetString() : "Default";
			VariantVector* verts = (*pMap)["Vertices"].GetVariantVectorPtr();

			//make sure that we have some vertices
			if (!verts) {
				return;
			}

			//write header
			WriteLinePair(0, "POLYLINE");
			WriteLinePair(8, layer);


			WriteLinePair(10, String(0.0));
			WriteLinePair(20, String(0.0));
			WriteLinePair(30, String(0.0));

			WriteLinePair(70, String(flags));

			//write vertices
			for (int i = 0; i < verts->Size(); i++)
			{
				Vector3 v = verts->At(i).GetVector3();
				WriteVertex(v, layer);
			}

			WriteLinePair(0, "SEQEND");
		}
	}
}

void DxfWriter::WritePoint(int id)
{
	if (id >= points_.Size())
		return;

	VariantMap pMap = points_[id].GetVariantMap();

	//check that this is a point structure
	if (pMap.Keys().Contains("000_TYPE")) {
		if (pMap["000_TYPE"].GetString() == "POINT")
		{
			Vector3 v = pMap.Keys().Contains("Position") ? pMap["Position"].GetVector3() : Vector3::ZERO;
			String layer = pMap.Keys().Contains("Layer") ? pMap["Layer"].GetString() : "Default";

			if (forceZUp_) {
				v = Vector3(v.x_, v.z_, v.y_);
			}

			//actually write
			WriteLinePair(0, "POINT");
			WriteLinePair(8, layer);
			WriteLinePair(10, String(v.x_));
			WriteLinePair(20, String(v.y_));
			WriteLinePair(30, String(v.z_));
		}
	}
}

//setters
void DxfWriter::SetMesh(Vector<Vector3> vertices, Vector<int> indices, String layer)
{
	VariantMap pMap;
	pMap["000_TYPE"] = "MESH";
	pMap["Layer"] = layer;

	//must be a better way to copy this stuff
	VariantVector verts;
	verts.Resize(vertices.Size());
	for (int i = 0; i < vertices.Size(); i++) {
		verts[i] = vertices[i];
	}

	//must be a better way to copy this stuff
	VariantVector faces;
	faces.Resize(indices.Size());
	for (int i = 0; i < indices.Size(); i++) {
		faces[i] = indices[i];
	}

	pMap["Vertices"] = verts;
	pMap["Faces"] = faces;
	pMap["Flags"] = 64; //only spec that this is a 3d mesh.

	meshes_.Push(pMap);
}

void DxfWriter::SetMesh(VariantVector vertices, VariantVector indices, String layer)
{
	VariantMap pMap;
	pMap["000_TYPE"] = "MESH";
	pMap["Layer"] = layer;
	pMap["Vertices"] = vertices;
	pMap["Faces"] = indices;
	pMap["Flags"] = 64; //only spec that this is a 3d mesh.


	meshes_.Push(pMap);

}

void DxfWriter::SetMesh(VariantVector meshes, String layer)
{

}

void DxfWriter::SetPolyline(Vector<Vector3> vertices, String layer)
{
	VariantMap pMap;
	pMap["000_TYPE"] = "POLYLINE";
	pMap["Layer"] = layer;

	VariantVector verts;
	verts.Resize(vertices.Size());
	for (int i = 0; i < vertices.Size(); i++) {
		verts[i] = vertices[i];
	}

	pMap["Vertices"] = verts;
	pMap["Flags"] = 8; //only spec that this is a 3d polyline.

	polylines_.Push(pMap);

}

void DxfWriter::SetPolyline(VariantVector vertices, String layer)
{
	VariantMap pMap;
	pMap["000_TYPE"] = "POLYLINE";
	pMap["Layer"] = layer;
	pMap["Vertices"] = vertices;
	pMap["Flags"] = 8; //only spec that this is a 3d polyline.

	polylines_.Push(pMap);
}

void DxfWriter::SetPoint(Vector3 point, String layer)
{
	VariantMap pMap;
	pMap["000_TYPE"] = "POINT";
	pMap["Position"] = point;
	pMap["Layer"] = layer;

	points_.Push(pMap);
}

void DxfWriter::SetPoints(Vector<Vector3> points, String layer)
{
	for (int i = 0; i < points.Size(); i++)
	{
		SetPoint(points[i], layer);
	}
}

void DxfWriter::SetPoints(VariantVector points, String layer)
{
	for (int i = 0; i < points.Size(); i++) {
		SetPoint(points[i].GetVector3(), layer);
	}
}