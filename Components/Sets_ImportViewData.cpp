#include "Sets_ImportViewData.h"
#include "IoGraph.h"
#include <assert.h>

#include <Urho3D/UI/UIEvents.h>

using namespace Urho3D;

String Sets_ImportViewData::iconTexture = "Textures/Icons/Sets_ImportViewData.png";

Sets_ImportViewData::Sets_ImportViewData(Context* context) :
	IoComponentBase(context, 0, 0)
{
	SetName("ImportViewData");
	SetFullName("Import View Data");
	SetDescription("Imports data from a view to be used in this view.");


	AddInputSlot(
		"DataIn",
		"In",
		"Data in",
		VAR_VARIANTMAP,
		DataAccess::TREE
		);

	AddOutputSlot(
		"ImportedData",
		"In",
		"Exported Data",
		VAR_VARIANTMAP,
		DataAccess::TREE
		);

}

String Sets_ImportViewData::GetNodeStyle()
{
	return "ImportDataNode";
}

void Sets_ImportViewData::HandleCustomInterface(UIElement* customElement)
{
	importNameEdit = customElement->CreateChild<LineEdit>("PropertyEdit");
	importNameEdit->SetStyle("LineEdit");
	importNameEdit->SetWidth(100);
	if (importNameEdit)
	{
		SubscribeToEvent(importNameEdit, E_TEXTFINISHED, URHO3D_HANDLER(Sets_ImportViewData, HandleLineEdit));
		importName = GetGenericData("ImportVariableName").GetString();
		importNameEdit->SetText(importName);
	}
}

void Sets_ImportViewData::HandleLineEdit(StringHash eventType, VariantMap& eventData)
{
	using namespace TextFinished;

	if (importNameEdit)
	{
		importName = importNameEdit->GetText();
		solvedFlag_ = 0;

		//set as metadata
		SetGenericData("ImportVariableName", importName);

		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();

	}
}


int Sets_ImportViewData::LocalSolve()
{
	importName = GetGenericData("ImportVariableName").GetString();
	
	if (importName.Empty())
	{
		solvedFlag_ = 0;
		return 0;
	}

	//Try to look up the component from the map
	VariantMap viewExportsMap = GetGlobalVar("ViewExportsMap").GetVariantMap();

	if (!viewExportsMap.Keys().Contains(importName))
	{
		solvedFlag_ = 0;
		return 0;
	}

	String sourceID = viewExportsMap[importName].GetString();
	IoGraph* graph = GetSubsystem<IoGraph>();
	int sourceIndex = graph->GetComponentIndex(sourceID);
	if (sourceIndex < 0)
	{
		solvedFlag_ = 0;
		return 0;
	}

	//create an internal link to the source comps output
	int targetIndex = graph->GetComponentIndex(this->ID);	

	//check that the connection doesn't existing.
	Vector<unsigned int> connections = graph->GetDownstreamComponentIndices(sourceIndex);
	if (!connections.Contains(targetIndex))
	{
		//create the connection and call solve to update the outputs
		graph->AddConnection(sourceIndex, 0, targetIndex, 0);
		solvedFlag_ = 0;

		//pretty dangerouse to call solve within solve
		//couldn't get the conection created above to "stick" without it, though.
		graph->QuickTopoSolveGraph();
	}
	else
	{
		//otherwise pass the data straight through
		IoDataTree inputIoDataTree = inputSlots_[0]->GetIoDataTree();
		outputSlots_[0]->SetIoDataTree(inputIoDataTree);
	}

	solvedFlag_ = 1;
	return 1;
}