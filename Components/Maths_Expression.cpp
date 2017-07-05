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


#include "Maths_Expression.h"

#include "Urho3D/AngelScript/Addons.h"
#include "Urho3D/AngelScript/APITemplates.h"
#include "Urho3D/AngelScript/Script.h";
#include "Urho3D/UI/UIEvents.h"
#include "IoGraph.h"

using namespace Urho3D;

String Maths_Expression::iconTexture = "Textures/Icons/Maths_Expression.png";

Maths_Expression::Maths_Expression(Urho3D::Context* context) : IoComponentBase(context, 0, 0)
{
	//set up inputs
	SetName("Expression");
	SetFullName("Evaluate Expression");
	SetDescription("Evaluates a basic Expression.");

	AddInputSlot(
		"X",
		"X",
		"Generic input",
		VAR_FLOAT,
		DataAccess::ITEM,
		1.0f
		);

	AddInputSlot(
		"Y",
		"Y",
		"Generic input",
		VAR_FLOAT,
		DataAccess::ITEM,
		2.0f
		);

	AddOutputSlot(
		"A",
		"A",
		"Generic output",
		VAR_FLOAT,
		DataAccess::ITEM
		);

}

void Maths_Expression::HandleCustomInterface(Urho3D::UIElement* customElement)
{
	expressionEdit_ = customElement->CreateChild<LineEdit>("PropertyEdit");
	expressionEdit_->SetStyle("LineEdit");
	expressionEdit_->SetWidth(100);

	if (expressionEdit_)
	{
		String exp = GetGenericData("Expression").GetString();
		if (!exp.Empty())
		{
			expression_ = exp;
		}
		
		SubscribeToEvent(expressionEdit_, E_TEXTFINISHED, URHO3D_HANDLER(Maths_Expression, HandleLineEdit));
		//TODO read metadata

	}
	
	//going to make sure slots are named sensibly
	SubscribeToEvent("InputSlotAdded", URHO3D_HANDLER(Maths_Expression, HandleInputsChanged));
	SubscribeToEvent("InputSlotRemoved", URHO3D_HANDLER(Maths_Expression, HandleInputsChanged));
}

void Maths_Expression::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	//check if expression is empty
	if (expression_.Empty())
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	
	//evaluate the expression
	String scriptCommand = "";

	for (int i = 0; i < inSolveInstance.Size(); i++)
	{
		VariantType type = inSolveInstance[i].GetType();
		String varName = inputSlots_[i]->GetVariableName();

		//supported types
		float vf;
		int vi;
		Vector3 vv;
		String vs;

		switch (type)
		{
		case VAR_FLOAT:
			vf = inSolveInstance[i].GetFloat();
			scriptCommand.AppendWithFormat("float %s = %f;", varName.CString(), vf);
			break;
		case VAR_INT:
			vi = inSolveInstance[i].GetInt();
			scriptCommand.AppendWithFormat("int %s = %i;", varName.CString(), vi);
			break;
		case VAR_VECTOR3:
			vv = inSolveInstance[i].GetVector3();
			scriptCommand.AppendWithFormat("Vector3 %s = Vector3(%f, %f, %f);", varName.CString(), vv.x_, vv.y_, vv.z_);
			break;
		case VAR_STRING:
			vs = inSolveInstance[i].GetString();
			scriptCommand.AppendWithFormat("String %s = \"%s\";", varName.CString(), vs.CString());
			break;
		default:
			break;
		}
	}
	
	//make sure script command is not empty
	if (scriptCommand.Empty())
	{
		SetAllOutputsNull(outSolveInstance);
	}

	//append the expression
	scriptCommand.Append("Variant result = ");
	scriptCommand.Append(expression_);
	scriptCommand.Append(";");
	scriptCommand.Append("SetGlobalVar(\"" + ID + "\", result);");

	//run the function as a script function
	Script* script_system = GetSubsystem<Script>();
	bool res = script_system->Execute(scriptCommand);

	if (!res)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	Variant result = script_system->GetGlobalVar(ID);

	//export the result
	outSolveInstance[0] = result;


}

void Maths_Expression::HandleLineEdit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace TextFinished;

	if (expressionEdit_)
	{
		expression_ = expressionEdit_->GetText();

		SetGenericData("Expression", expression_);

		solvedFlag_ = 0;
		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
	}

}	

void Maths_Expression::HandleInputsChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{

}