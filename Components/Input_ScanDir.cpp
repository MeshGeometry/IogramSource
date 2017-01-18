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