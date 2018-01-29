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

	AddInputSlot(
		"Command",
		"C",
		"Command",
		VAR_BOOL,
		ITEM,
		true
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
	bool systemCommand = inSolveInstance[2].GetBool();

	if (command.Empty())
	{
		URHO3D_LOGWARNING("Empty command at Interop_SystemCommand component!");
		outSolveInstance[0] = Variant();
		return;
	}

	if (!systemCommand)
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

		int res = fs->SystemCommand(command, false);
		outSolveInstance[0] = res;
		return;
	}

	outSolveInstance[0] = Variant();
}