#include "Interop_AsyncSystemCommand.h"

#include <Urho3D/IO/FileSystem.h>

using namespace Urho3D;

String Interop_AsyncSystemCommand::iconTexture = "Textures/Icons/Interop_AsyncSystemCommand.png";

Interop_AsyncSystemCommand::Interop_AsyncSystemCommand(Context* context) :IoComponentBase(context, 0, 0)
{
	SetName("AsyncSystemCommand");
	SetFullName("Calls a program from the OS");
	SetDescription("Calls several programs from the OS");

	AddInputSlot(
		"Executable",
		"C",
		"The executable to call asynchronously",
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

void Interop_AsyncSystemCommand::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	FileSystem* fs = GetSubsystem<FileSystem>();

	String command = inSolveInstance[0].GetString();
	VariantVector args = inSolveInstance[1].GetVariantVector();

	if (command.Empty())
	{
		outSolveInstance[0] = Variant();
		return;
	}

	if (fs->FileExists(command))
	{
		StringVector argList;
		for (int i = 0; i << args.Size(); i++)
		{
			argList.Push(args[i].GetString());
		}

		int res = fs->SystemRunAsync(command, argList);
		outSolveInstance[0] = res;
		return;
	}
	else
	{
		int res = fs->SystemCommandAsync(command);
		outSolveInstance[0] = res;
		return;
	}

	outSolveInstance[0] = Variant();
}