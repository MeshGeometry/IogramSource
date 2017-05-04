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


#include "Interop_JsonSchema.h"
#include "TriMesh.h"
#include "Polyline.h"

#include <iostream>

#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/JSONFile.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/ThirdParty/rapidjson/document.h>
#include <Urho3D/ThirdParty/rapidjson/prettywriter.h>
#include <Urho3D/ThirdParty/rapidjson/stringbuffer.h>

using namespace Urho3D;

String Interop_JsonSchema::iconTexture = "Textures/Icons/Interop_JsonSchema.png";

Interop_JsonSchema::Interop_JsonSchema(Context* context) :IoComponentBase(context, 0, 0)
{
	SetName("JsonSchema");
	SetFullName("Create JSON Schema");
	SetDescription("Creats a JSON Document from geometry data.");

	AddInputSlot(
		"Geometry",
		"G",
		"Geometry to parse",
		VAR_VARIANTMAP,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Directory",
		"D",
		"Directory",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Name",
		"N",
		"Name",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Json",
		"J",
		"Json Document",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"File",
		"F",
		"File",
		VAR_STRING,
		DataAccess::ITEM
	);


}

void Interop_JsonSchema::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	FileSystem* fs = GetSubsystem<FileSystem>();
	
	String directory = inSolveInstance[1].GetString();
	String name = inSolveInstance[2].GetString();

	bool isMesh = TriMesh_Verify(inSolveInstance[0]);

	//only support tri meshes
	if (!isMesh)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//get the data
	VariantVector vertices = TriMesh_GetVertexList(inSolveInstance[0]);
	VariantVector faces = TriMesh_GetFaceList(inSolveInstance[0]);

	//don't bother with degereate meshes
	if (vertices.Size() < 3 || faces.Size() < 3)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	//make sure dir exists
	directory = AddTrailingSlash(directory);
	if (!fs->DirExists(directory))
	{
		fs->CreateDir(directory);
	}


	//need more control so we use rapidjson directly
	//rapidjson::Document document;
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

	writer.StartObject();

	//vertices
	writer.String("vertices");
	writer.StartArray();
	for (int i = 0; i < vertices.Size(); i++)
	{
		Vector3 v = vertices[i].GetVector3();
		writer.StartArray();
		writer.Double(v.x_);
		writer.Double(v.y_);
		writer.Double(v.z_);
		writer.EndArray();
	}
	writer.EndArray();

	//faces
	writer.String("faces");
	writer.StartArray();
	for (int i = 0; i < faces.Size() / 3; i++)
	{
		writer.StartArray();
		writer.Int(faces[3 * i + 0].GetInt());
		writer.Int(faces[3 * i + 1].GetInt());
		writer.Int(faces[3 * i + 2].GetInt());
		writer.EndArray();
	}
	writer.EndArray();

	//type
	writer.String("primitive");
	writer.String("mesh");

	//end
	writer.EndObject();

	//push to doc
	//document.Accept(writer);
	
	//create out file
	if (!name.Empty())
	{
		SharedPtr<File> dest(new File(GetContext(), directory + name, FILE_WRITE));
		unsigned size = (unsigned)buffer.GetSize();
		dest->Write(buffer.GetString(), size) == size;
	}

	//String output
	String stringOut;
	BufferToString(stringOut, buffer.GetString(), buffer.GetSize());


	outSolveInstance[0] = stringOut;
	outSolveInstance[1] = directory + name;
}