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


#include "Interop_ReceiveData.h"

#ifdef URHO3D_NETWORK
#include "IoGraph.h"
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/IO/Compression.h>

#include <assert.h>

using namespace Urho3D;

String Interop_ReceiveData::iconTexture = "Textures/Icons/Interop_ReceiveData.png";



Interop_ReceiveData::Interop_ReceiveData(Context* context) :
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

	SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(Interop_ReceiveData, HandleNetworkMessage));
}

String Interop_ReceiveData::GetNodeStyle()
{
	return "ExportDataNode";
}

void Interop_ReceiveData::HandleCustomInterface(UIElement* customElement)
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

	SubscribeToEvent(importPortEdit_, E_TEXTFINISHED, URHO3D_HANDLER(Interop_ReceiveData, HandleLineEdit));
	SubscribeToEvent(addressEdit_, E_TEXTFINISHED, URHO3D_HANDLER(Interop_ReceiveData, HandleLineEdit));

	//connect
	Network* network = GetSubsystem<Network>();
	bool res = network->Connect(sourceAddress_, importPort_, 0);
}

void Interop_ReceiveData::HandleLineEdit(StringHash eventType, VariantMap& eventData)
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

void Interop_ReceiveData::SolveInstance(
	const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
)
{
	outSolveInstance[0] = incomingData_;
}

void Interop_ReceiveData::HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
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
#endif