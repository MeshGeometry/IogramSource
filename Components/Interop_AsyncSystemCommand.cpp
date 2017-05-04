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
		for (int i = 0; i < args.Size(); i++)
		{
			argList.Push(args[i].GetString());
		}

		int res = -1;
		
		if (argList.Size() > 0)
		{
			res = fs->SystemRunAsync(command, argList);
		}
		else
		{
			res = fs->SystemCommandAsync(command);
		}
		
		
		outSolveInstance[0] = res;
		return;
	}
	else
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

}