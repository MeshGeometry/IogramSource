#include "Sets_SendData.h"

#include "IoGraph.h"

#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/IO/MemoryBuffer.h>

#include <assert.h>

using namespace Urho3D;

String Sets_SendData::iconTexture = "Textures/Icons/Sets_SendData.png";

Sets_SendData::Sets_SendData(Context* context) :
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

	exportPort_ = 2345;
	SubscribeToEvent(E_SERVERCONNECTED, URHO3D_HANDLER(Sets_SendData, HandleConnectionStatus));

	//send this data via udp
	Network* network = GetSubsystem<Network>();
	bool res = network->StartServer(CHAT_SERVER_PORT);
	Connection* serverConnection = network->GetServerConnection();
	bool serverRunning = network->IsServerRunning();
}

String Sets_SendData::GetNodeStyle()
{
	return "ExportDataNode";
}

void Sets_SendData::HandleCustomInterface(UIElement* customElement)
{
	exportNameEdit = customElement->CreateChild<LineEdit>("PropertyEdit");
	exportNameEdit->SetStyle("LineEdit");
	exportNameEdit->SetWidth(100);
	if (exportNameEdit)
	{
		SubscribeToEvent(exportNameEdit, E_TEXTFINISHED, URHO3D_HANDLER(Sets_SendData, HandleLineEdit));
		exportPort_ = GetGenericData("SendPort").GetInt();
		exportNameEdit->SetText(String(exportPort_));
	}
}

void Sets_SendData::HandleLineEdit(StringHash eventType, VariantMap& eventData)
{
	using namespace TextFinished;

	if (exportNameEdit)
	{
		//exportPort_ = exportNameEdit->GetText();
		solvedFlag_ = 0;

		//set as metadata
		SetGenericData("ExportVariableName", exportPort_);

		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();

	}
}

void Sets_SendData::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	Network* network = GetSubsystem<Network>();

	Connection* serverConnection = network->GetServerConnection();

	if (serverConnection)
	{
		// A VectorBuffer object is convenient for constructing a message to send
		VectorBuffer msg;
		msg.WriteVariant(outSolveInstance[0]);
		// Send the chat message as in-order and reliable
		serverConnection->SendMessage(32, true, true, msg);
	}
}

void Sets_SendData::HandleConnectionStatus(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	String hello = "";
}
