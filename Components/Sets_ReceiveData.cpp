#include "Sets_ReceiveData.h"

#include "IoGraph.h"

#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/IO/Compression.h>

#include <assert.h>

using namespace Urho3D;

String Sets_ReceiveData::iconTexture = "Textures/Icons/Sets_ReceiveData.png";



Sets_ReceiveData::Sets_ReceiveData(Context* context) :
	IoComponentBase(context, 0, 0),
	importPort_(2345),
	sourceAddress_("localhost")
{
	SetName("ExportViewData");
	SetFullName("Export View Data");
	SetDescription("Exports data from a view to be used in another view.");


	AddInputSlot(
		"Data",
		"Out",
		"Data tree to export",
		VAR_VARIANTVECTOR,
		DataAccess::LIST
	);

	AddOutputSlot(
		"ExportedData",
		"Out",
		"Exported Data",
		VAR_VARIANTVECTOR,
		DataAccess::LIST
	);


	SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(Sets_ReceiveData, HandleNetworkMessage));
}

String Sets_ReceiveData::GetNodeStyle()
{
	return "ExportDataNode";
}

void Sets_ReceiveData::HandleCustomInterface(UIElement* customElement)
{
	importPortEdit_ = customElement->CreateChild<LineEdit>("ImportPort");
	importPortEdit_->SetStyle("LineEdit");
	importPortEdit_->SetWidth(100);
	importPort_ = GetGenericData("ImportPort").GetInt();
	importPortEdit_->SetText(String(importPort_));

	addressEdit_ = customElement->CreateChild<LineEdit>("AddressEdit");
	addressEdit_->SetStyle("LineEdit");
	addressEdit_->SetWidth(100);
	sourceAddress_ = GetGenericData("SourceAddress").GetString();
	addressEdit_->SetText(sourceAddress_);

	SubscribeToEvent(importPortEdit_, E_TEXTFINISHED, URHO3D_HANDLER(Sets_ReceiveData, HandleLineEdit));
	SubscribeToEvent(addressEdit_, E_TEXTFINISHED, URHO3D_HANDLER(Sets_ReceiveData, HandleLineEdit));

	//connect
	Network* network = GetSubsystem<Network>();
	bool res = network->Connect(sourceAddress_, importPort_, 0);
}

void Sets_ReceiveData::HandleLineEdit(StringHash eventType, VariantMap& eventData)
{
	using namespace TextFinished;

	LineEdit* le = (LineEdit*)eventData[P_ELEMENT].GetPtr();

	if (le == importPortEdit_)
	{
		importPort_ = ToInt(importPortEdit_->GetText());
		//set as metadata
		SetGenericData("ImportPort", importPort_);

		//reconnect
		Network* network = GetSubsystem<Network>();
		bool res = network->Connect(sourceAddress_, importPort_, 0);

		solvedFlag_ = 0;
		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();

	}

	if (le == addressEdit_)
	{
		sourceAddress_ = addressEdit_->GetText();
		SetGenericData("SourceAddress", sourceAddress_);

		//reconnect
		Network* network = GetSubsystem<Network>();
		bool res = network->Connect(sourceAddress_, importPort_, 0);

		solvedFlag_ = 0;
		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
	}
}

void Sets_ReceiveData::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	outSolveInstance[0] = incomingData_;
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
		VectorBuffer vb(data);
		VectorBuffer decompMsg = DecompressVectorBuffer(vb);
		incomingData_ = decompMsg.ReadVariantVector();

		int size = incomingData_.Size();

		solvedFlag_ = 0;
		GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
	}
}
