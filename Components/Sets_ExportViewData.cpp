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


#include "Sets_ExportViewData.h"

#include "IoGraph.h"

#include <Urho3D/UI/UIEvents.h>

#include <assert.h>

using namespace Urho3D;

String Sets_ExportViewData::iconTexture = "Textures/Icons/Sets_ExportViewData.png";

Sets_ExportViewData::Sets_ExportViewData(Context* context) :
	IoComponentBase(context, 0, 0)
{
	SetName("ExportViewData");
	SetFullName("Export View Data");
	SetDescription("Exports data from a view to be used in another view.");


	AddInputSlot(
		"Data",
		"Out",
		"Data tree to export",
		VAR_NONE,
		DataAccess::TREE
		);

	AddOutputSlot(
		"ExportedData",
		"Out",
		"Exported Data",
		VAR_VARIANTMAP,
		DataAccess::TREE
		);

}

String Sets_ExportViewData::GetNodeStyle()
{
	return "ExportDataNode";
}

void Sets_ExportViewData::HandleCustomInterface(UIElement* customElement)
{
	exportNameEdit = customElement->CreateChild<LineEdit>("PropertyEdit");
	exportNameEdit->SetStyle("LineEdit");
	exportNameEdit->SetWidth(100);
	if (exportNameEdit)
	{
		SubscribeToEvent(exportNameEdit, E_TEXTFINISHED, URHO3D_HANDLER(Sets_ExportViewData, HandleLineEdit));
		exportName = GetGenericData("ExportVariableName").GetString();
		exportNameEdit->SetText(exportName);
	}
}

void Sets_ExportViewData::HandleLineEdit(StringHash eventType, VariantMap& eventData)
{
	using namespace TextFinished;

	if (exportNameEdit)
	{
		exportName = exportNameEdit->GetText();
		solvedFlag_ = 0;

		//set as metadata
		SetGenericData("ExportVariableName", exportName);

		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();

	}
}

int Sets_ExportViewData::LocalSolve()
{
	exportName = GetGenericData("ExportVariableName").GetString();
		
	if (inputSlots_[0]->HasNoData()) {
		solvedFlag_ = 0;
		return 0;
	}

	//check that the name is actually valid
	if (exportName.Empty())
	{
		solvedFlag_ = 0;
		return 0;
	}

	//the idea is to register a graph component with a unique name.
	//when the import data component runs, it will create a graph link based on this component
	VariantMap viewExportsMap = GetGlobalVar("ViewExportsMap").GetVariantMap();
	viewExportsMap[exportName] = this->ID;
	SetGlobalVar("ViewExportsMap", viewExportsMap);

	//pass the data straight through
	IoDataTree inputIoDataTree = inputSlots_[0]->GetIoDataTree();
	outputSlots_[0]->SetIoDataTree(inputIoDataTree);

	solvedFlag_ = 1;
	return 1;
}