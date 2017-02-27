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


#include "Mesh_PerVertexEval.h"

#include "Urho3D/AngelScript/Script.h"
#include "Urho3D/AngelScript/ScriptFile.h"

#include "TriMesh.h"

using namespace Urho3D;

String Mesh_PerVertexEval::iconTexture = "";

Mesh_PerVertexEval::Mesh_PerVertexEval(Urho3D::Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("PerVertexEval");
	SetFullName("Per-Vertex Evaluate Function");
	SetDescription("Evaluates function on vertices of TriMesh");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("TriMesh input");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Function");
	inputSlots_[1]->SetVariableName("FN");
	inputSlots_[1]->SetDescription("Function definition in Script");
	inputSlots_[1]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[1]->SetDefaultValue("X");
	inputSlots_[1]->DefaultSet();
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("ResultList");
	outputSlots_[0]->SetVariableName("RL");
	outputSlots_[0]->SetDescription("List of results evaluated on each vertex");
	outputSlots_[0]->SetVariantType(VariantType::VAR_FLOAT);
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);
}

void Mesh_PerVertexEval::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Variant tri_mesh = inSolveInstance[0];
	if (!TriMesh_Verify(tri_mesh)) {
		URHO3D_LOGWARNING("M must be a valid TriMesh");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	if (inSolveInstance[1].GetType() != VariantType::VAR_STRING) {
		URHO3D_LOGWARNING("FN must be a string");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	String function = inSolveInstance[1].GetString();

	VariantVector vertex_list = TriMesh_GetVertexList(tri_mesh);
	VariantVector per_vertex_float_values;
	for (int i = 0; i < vertex_list.Size(); ++i) {
		String current_function = function;
		Vector3 v = vertex_list[i].GetVector3();
		current_function.Replace("X", String(v.x_), false);
		current_function.Replace("Y", String(v.y_), false);
		current_function.Replace("Z", String(v.z_), false);

		Script* script_system = GetSubsystem<Script>();
		String line = "float result =" + current_function + "; SetGlobalVar(\"" + ID + "\", result);";
		bool res = script_system->Execute(line);
		if (!res) {
			URHO3D_LOGWARNING("PerVertexEval --- function evaluation failed on vertex");
			SetAllOutputsNull(outSolveInstance);
			return;
		}

		float result = script_system->GetGlobalVar(ID).GetFloat();
		per_vertex_float_values.Push(result);
	}

	outSolveInstance[0] = per_vertex_float_values;
}