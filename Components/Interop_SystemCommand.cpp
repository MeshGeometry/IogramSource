#include "Interop_SystemCommand.h"

#include <iostream>

#include <Urho3D/IO/FileSystem.h>

using namespace Urho3D;

String Interop_SystemCommand::iconTexture = "Textures/Icons/Interop_SystemCommand.png";

Interop_SystemCommand::Interop_SystemCommand(Context* context) :IoComponentBase(context, 0, 0)
{
	SetName("SystemCommand");
	SetFullName("Calls a program from the OS");
	SetDescription("Calls a program from the OS");

	AddInputSlot(
		"Executable",
		"C",
		"The executable to call",
		VAR_STRING,
		DataAccess::ITEM
	);

	AddInputSlot(
		"Arguments",
		"A",
		"Arguments",
		VAR_STRING,
		DataAccess::LIST
	);

	AddOutputSlot(
		"ExitCode",
		"R",
		"Result of the call",
		VAR_STRING,
		DataAccess::ITEM
	);


}

void Interop_SystemCommand::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	// Debug output
	std::cout << "... Entered Interop_SystemCommand::SolveInstance ...\n";

	FileSystem* fs = GetSubsystem<FileSystem>();

	String command = inSolveInstance[0].GetString();
	VariantVector args = inSolveInstance[1].GetVariantVector();

	if (command.Empty())
	{
		URHO3D_LOGWARNING("Empty command at Interop_SystemCommand component!");
		outSolveInstance[0] = Variant();
		return;
	}

	if (fs->FileExists(command))
	{		
		StringVector argList;
		for (int i = 0; i < args.Size(); i++)
		{
			argList.Push(args[i].GetString());
		}

		// Debug output
		std::cout << "calling: fs->SystemRun(command, argList);\n";
		std::cout << "command = " << command.CString() << "\n";
		std::cout << "argList =";
		for (int i = 0; i < argList.Size(); ++i) {
			std::cout << " " << argList[i].CString();
		}
		std::cout << "\n";
		
		int res = fs->SystemRun(command, argList);
		outSolveInstance[0] = res;
		return;
	}
	else
	{
		// Debug output
		std::cout << "calling: fs->SystemCommand(command);\n";
		std::cout << "command = " << command.CString() << "\n";

		int res = fs->SystemCommand(command);
		outSolveInstance[0] = res;
		return;
	}

	outSolveInstance[0] = Variant();
}