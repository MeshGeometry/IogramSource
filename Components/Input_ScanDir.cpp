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


#include "Input_ScanDir.h"

#include <Urho3D/IO/FileSystem.h>

using namespace Urho3D;

String Input_ScanDir::iconTexture = "Textures/Icons/Input_ScanDir.png";

Input_ScanDir::Input_ScanDir(Context* context) : IoComponentBase(context, 0, 0)
{
	SetName("ScanDir");
	SetFullName("ScanDir");
	SetDescription("Scans a directory for files.");

	AddInputSlot(
		"Path",
		"P",
		"Directory path",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Filter",
		"F",
		"Optional filter",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Recursive",
		"R",
		"Search directory recursively",
		VAR_BOOL,
		DataAccess::ITEM
	);

	AddOutputSlot(
		"Results",
		"R",
		"Scanned files",
		VAR_STRING,
		DataAccess::LIST
	);

	AddOutputSlot(
		"FullPaths",
		"F",
		"Full paths to scanned files",
		VAR_STRING,
		DataAccess::LIST
		);
}

void Input_ScanDir::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{

	String dirPath = inSolveInstance[0].GetString();
	String filter = inSolveInstance[1].GetString();
	bool recursive = inSolveInstance[2].GetBool();

	if (dirPath.Empty() || !GetSubsystem<FileSystem>()->DirExists(dirPath))
	{
		VariantVector t;
		t.Push(Variant());
		//outSolveInstance[0] = t;
		outSolveInstance[0] = Variant();
		return;
	}

	Vector<String> results;
	GetSubsystem<FileSystem>()->ScanDir(results, dirPath, filter, SCAN_FILES, recursive);

	if (results.Empty())
	{
		VariantVector t;
		t.Push(Variant());
		//outSolveInstance[0] = t;
		outSolveInstance[0] = Variant();
		return;
	}

	VariantVector pathsOut;
	for (int i = 0; i < results.Size(); i++)
	{
		pathsOut.Push(results[i]);
	}

	VariantVector fullPathsOut;
	for (int i = 0; i < results.Size(); i++)
	{
		fullPathsOut.Push(dirPath + "/" + results[i]);
	}


	outSolveInstance[0] = pathsOut;
	outSolveInstance[1] = fullPathsOut;
}