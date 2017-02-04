#include "Sets_ReceiveData.h"

#include "IoGraph.h"

#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/IO/MemoryBuffer.h>

#include <assert.h>

using namespace Urho3D;

String Sets_ReceiveData::iconTexture = "Textures/Icons/Sets_ReceiveData.png";



Sets_ReceiveData::Sets_ReceiveData(Context* context) :
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

	SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(Sets_ReceiveData, HandleNetworkMessage));
}

String Sets_ReceiveData::GetNodeStyle()
{
	return "ExportDataNode";
}

void Sets_ReceiveData::HandleCustomInterface(UIElement* customElement)
{
	exportNameEdit = customElement->CreateChild<LineEdit>("PropertyEdit");
	exportNameEdit->SetStyle("LineEdit");
	exportNameEdit->SetWidth(100);
	if (exportNameEdit)
	{
		SubscribeToEvent(exportNameEdit, E_TEXTFINISHED, URHO3D_HANDLER(Sets_ReceiveData, HandleLineEdit));
		exportPort_ = GetGenericData("SendPort").GetInt();
		exportNameEdit->SetText(String(exportPort_));
	}
}

void Sets_ReceiveData::HandleLineEdit(StringHash eventType, VariantMap& eventData)
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

void Sets_ReceiveData::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	outSolveInstance[0] = inSolveInstance[0];
}

void Sets_ReceiveData::HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	Network* network = GetSubsystem<Network>();

	using namespace NetworkMessage;

	int msgID = eventData[P_MESSAGEID].GetInt();
	if (msgID == MSG_CHAT)
	{
		const PODVector<unsigned char>& data = eventData[P_DATA].GetBuffer();
		// Use a MemoryBuffer to read the message data so that there is no unnecessary copying
		MemoryBuffer msg(data);
		Variant dataVar = msg.ReadVariant();

		// If we are the server, prepend the sender's IP address and port and echo to everyone
		// If we are a client, just display the message
		if (network->IsServerRunning())
		{
			Connection* sender = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());

		}
	}
}
