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