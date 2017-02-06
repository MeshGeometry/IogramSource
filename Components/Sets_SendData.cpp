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
	IoComponentBase(context, 0, 0),
	exportPort_(2345)
{
	SetName("ExportViewData");
	SetFullName("Export View Data");
	SetDescription("Exports data from a view to be used in another view.");


	AddInputSlot(
		"Data",
		"In",
		"Data tree to export",
		VAR_VARIANTVECTOR,
		DataAccess::LIST
	);

	AddOutputSlot(
		"ExportedData",
		"Out",
		"Exported Data",
		VAR_VARIANTMAP,
		DataAccess::LIST
	);

	AddOutputSlot(
		"IP",
		"IP Address",
		"IP Address",
		VAR_INT,
		DataAccess::ITEM
		);


	SubscribeToEvent(E_SERVERCONNECTED, URHO3D_HANDLER(Sets_SendData, HandleConnectionStatus));

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
		exportPort_ = ToInt(exportNameEdit->GetText());

		//set as metadata
		SetGenericData("SendPort", exportPort_);

		//change server setup
		Network* network = GetSubsystem<Network>();
		if (network->IsServerRunning())
		{
			network->StopServer();
		}

		bool res = network->StartServer(exportPort_);
		bool serverRunning = network->IsServerRunning();

		solvedFlag_ = 0;
		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();

	}
}

void Sets_SendData::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	Network* network = GetSubsystem<Network>();

	//create server if necessary
	if (!network->IsServerRunning())
	{
		Network* network = GetSubsystem<Network>();
		bool res = network->StartServer(exportPort_);
	}

	if (network->IsServerRunning())
	{
		// A VectorBuffer object is convenient for constructing a message to send
		VectorBuffer msg;

		//A message is always preceded by the variant type
		msg.WriteVariantVector(inSolveInstance[0].GetVariantVector());
		network->BroadcastMessage(MSG_CHAT, true, true, msg);


		//push to output
		outSolveInstance[0] = inSolveInstance[0];
		outSolveInstance[1] = "localhost";
	}
	else
	{
		SetAllOutputsNull(outSolveInstance);
	}

	//connect
	//String address = "localhost"; //TODO: enable send connections to non local host address
	//bool res = network->Connect(address, exportPort_, 0);
	//Connection* serverConnection = network->GetServerConnection();
	//if (serverConnection)
	//{
	//	// A VectorBuffer object is convenient for constructing a message to send
	//	VectorBuffer msg;

	//	//A message is always preceded by the variant type
	//	msg.WriteVariantVector(inSolveInstance[0].GetVariantVector());

	//	if (network->IsServerRunning())
	//	{
	//		serverConnection->SendMessage(MSG_CHAT, true, true, msg);
	//		network->BroadcastMessage(MSG_CHAT, true, true, msg);

	//		//push to output
	//		outSolveInstance[0] = inSolveInstance[0];
	//		outSolveInstance[1] = serverConnection->GetAddress();
	//	}
	//}
}

void Sets_SendData::HandleConnectionStatus(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{

}
