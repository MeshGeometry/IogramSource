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


#include "Interop_SendData.h"
#ifdef URHO3D_NETWORK
#include "IoGraph.h"

#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/IO/Compression.h>

#include <assert.h>

using namespace Urho3D;

String Interop_SendData::iconTexture = "Textures/Icons/Interop_SendData.png";

Interop_SendData::Interop_SendData(Context* context) :
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


	SubscribeToEvent(E_SERVERCONNECTED, URHO3D_HANDLER(Interop_SendData, HandleConnectionStatus));

}

String Interop_SendData::GetNodeStyle()
{
	return "ExportDataNode";
}

void Interop_SendData::HandleCustomInterface(UIElement* customElement)
{
	exportNameEdit = customElement->CreateChild<LineEdit>("PropertyEdit");
	exportNameEdit->SetStyle("LineEdit");
	exportNameEdit->SetWidth(100);
	if (exportNameEdit)
	{
		SubscribeToEvent(exportNameEdit, E_TEXTFINISHED, URHO3D_HANDLER(Interop_SendData, HandleLineEdit));
		exportPort_ = GetGenericData("SendPort").GetInt();
		exportNameEdit->SetText(String(exportPort_));
	}
}

void Interop_SendData::HandleLineEdit(StringHash eventType, VariantMap& eventData)
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

void Interop_SendData::SolveInstance(
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

		//compress
		VectorBuffer compressedMsg = CompressVectorBuffer(msg);

		network->BroadcastMessage(MSG_CHAT, true, true, compressedMsg);


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

void Interop_SendData::HandleConnectionStatus(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{

}
#endif