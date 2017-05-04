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


#include "Graphics_CurveToModel.h"
#include "Polyline.h"

#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>

using namespace Urho3D;

String Graphics_CurveToModel::iconTexture = "Textures/Icons/Graphics_CurveToModel.png";

Graphics_CurveToModel::Graphics_CurveToModel(Urho3D::Context* context) : IoComponentBase(context, 4, 1)
{
	SetName("CurveToModel");
	SetFullName("Curve To Model");
	SetDescription("Converts a curve to a model on disk with a pointer to the model");

	inputSlots_[0]->SetName("Polyline");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Polyline structure");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("File");
	inputSlots_[1]->SetVariableName("F");
	inputSlots_[1]->SetDescription("Optional path to save Model");
	inputSlots_[1]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue("");
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Thickness");
	inputSlots_[2]->SetVariableName("T");
	inputSlots_[2]->SetDescription("Line thickness");
	inputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(0.01f);
	inputSlots_[2]->DefaultSet();

	inputSlots_[3]->SetName("VertexColors");
	inputSlots_[3]->SetVariableName("C");
	inputSlots_[3]->SetDescription("VertexColors");
	inputSlots_[3]->SetVariantType(VariantType::VAR_VARIANTVECTOR);
	inputSlots_[3]->SetDataAccess(DataAccess::LIST);
	inputSlots_[3]->SetDefaultValueSafe(Color::WHITE);
	inputSlots_[3]->DefaultSet();

	outputSlots_[0]->SetName("ModelPointer");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Void Pointer to Model");
	outputSlots_[0]->SetVariantType(VariantType::VAR_STRING); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

Graphics_CurveToModel::~Graphics_CurveToModel()
{

}

void Graphics_CurveToModel::PreLocalSolve()
{
	trackedItems.Clear();
	autoNameCounter = 0;
}

void Graphics_CurveToModel::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	//some checks
	if (inSolveInstance[0].GetType() != VAR_VARIANTMAP)
	{
		URHO3D_LOGERROR("Expected a VariantMap and was given a " + inSolveInstance[0].GetTypeName());
		return;
	}

	Variant poly = inSolveInstance[0];
	VariantVector vColors = inSolveInstance[3].GetVariantVector();

	if (Polyline_Verify(poly))
	{


		float thickness = inSolveInstance[2].GetFloat();

		SharedPtr<Model> mdl;

		mdl = Polyline_GetRenderMesh(poly, GetContext(), vColors, thickness);

		String filePath = inSolveInstance[1].GetString();
		if (!filePath.Empty());
		{
			//add folder prefix
			filePath = GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Models/" + filePath;

			File* file = new File(GetContext(), filePath, FileMode::FILE_WRITE);
			mdl->Save(*file);
			file->Close();
		}

		trackedItems.Push(mdl);
		outSolveInstance[0] = mdl;
	}
	else
	{
		URHO3D_LOGERROR("Incomplete data given in poly paramater.");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
}