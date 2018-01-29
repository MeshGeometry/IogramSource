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

#include "IoScriptInstance.h"

#include <Urho3D/AngelScript/ScriptEventListener.h>
#include <Urho3D/AngelScript/Script.h>
#include <Urho3D/AngelScript/ScriptFile.h>
#include <Urho3D/Core/Condition.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/ResourceEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Core/StringUtils.h>

#include "IoGraph.h"
#include <AngelScript/angelscript.h>

using namespace Urho3D;

static const char* methodDeclarations[] = {
	"void Start()",
	"void Stop()",
	"void DefineInputs()",
	"void DefineOutputs()",
	"void PreLocalSolve()",
	"void SolveInstance(VariantMap& inputs, VariantMap& outputs)",
	"void Update(float)"
};

namespace {

	Context* globalContext;


	asIScriptContext* GetActiveASContext()
	{
		return asGetActiveContext();
	}

	Context* GetScriptContext()
	{
		asIScriptContext* context = asGetActiveContext();
		if (context)
			return static_cast<Script*>(context->GetEngine()->GetUserData())->GetContext();
		else
			return 0;
	}

	IoScriptInstance* GetScriptContextInstance()
	{
		asIScriptContext* context = asGetActiveContext();
		asIScriptObject* object = context ? static_cast<asIScriptObject*>(context->GetThisPointer()) : 0;
		if (object)
			return static_cast<IoScriptInstance*>(object->GetUserData());
		else
			return 0;
	}

	ScriptEventListener* GetScriptContextEventListener()
	{
		// If the context has an object and that object has user data set, try and get the IoScriptInstance, otherwise try and get a ScriptFile.
		asIScriptContext* context = asGetActiveContext();
		if (context)
		{
			asIScriptObject* object = static_cast<asIScriptObject*>(context->GetThisPointer());
			if (object && object->GetUserData())
				return GetScriptContextInstance();
			else
				return GetScriptContextFile();
		}
		else
			return 0;
	}

	Object* GetScriptContextEventListenerObject()
	{
		return dynamic_cast<Object*>(GetScriptContextEventListener());
	}

	Urho3D::Scene* GetActiveScene()
	{
		Scene* scene = (Scene*)globalContext->GetGlobalVar("Scene").GetPtr();
		return scene;
	}

	Urho3D::Viewport* GetActiveViewport()
	{
		Viewport* vp = (Viewport*)globalContext->GetGlobalVar("activeViewport").GetPtr();
		return vp;
	}

	void CreateInputSlot(String variable, String name, int access, Variant value, String description)
	{
		IoScriptInstance* scriptInstance = GetScriptContextInstance();

		if (scriptInstance)
		{
			VariantMap input;
			input["Name"] = name;
			input["VariableName"] = variable;
			input["DataAccess"] = access;

			if (!description.Empty()) {
				input["Description"] = description;
			}

			if (value.GetType() != VAR_NONE) {
				input["DefaultValue"] = value;
			}

			scriptInstance->AddInputSlotFromScript(input);
		}
	}

	void CreateOutputSlot(String variable, String name, int access, String description)
	{
		IoScriptInstance* scriptInstance = GetScriptContextInstance();

		if (scriptInstance)
		{
			VariantMap output;
			output["Name"] = name;
			output["VariableName"] = variable;
			output["DataAccess"] = access;

			if (!description.Empty()) {
				output["Description"] = description;
			}

			scriptInstance->AddOutputSlotFromScript(output);
		}
	}
}

IoScriptInstance::IoScriptInstance(Context* context) :
	IoComponentBase(context, 0, 0),
	scriptObject_(0),
	subscribed_(false)
{
	ClearScriptMethods();

	SubscribeToEvent(E_SCRIPTERROR, URHO3D_HANDLER(IoScriptInstance, HandleScriptError));



	//try to load
	bool loadScript = GetGenericData("LoadScript").GetBool();
	if (loadScript) {
		String scriptPath = GetGenericData("ScriptPath").GetString();
		String className = GetGenericData("ClassName").GetString();

		CreateObjectFromPath(scriptPath, className);
	}

	globalContext = context;

}

bool IoScriptInstance::CreateObjectFromPath(Urho3D::String pathToScript, Urho3D::String className)
{
	//remember the path for error handling
	scriptPath_ = pathToScript;

	//subscribe
	SubscribeToEvent(E_SCRIPTERROR, URHO3D_HANDLER(IoScriptInstance, HandleScriptError));

	//load a script file
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	ScriptFile* sf = rc->GetResource<ScriptFile>(pathToScript);
	bool res = false;
	if (sf) {
		res = CreateObject(sf, className);

		//clear inputs
		inputSlots_.Clear();
		outputSlots_.Clear();

		if (res) {
			//create the inputs
			if (methods_[METHOD_DEFINEINPUTS]) {
				Execute(methods_[METHOD_DEFINEINPUTS]);
			}
			//create the outputs
			if (methods_[METHOD_DEFINEOUTPUTS]) {
				Execute(methods_[METHOD_DEFINEOUTPUTS]);
			}
		}
	}

	//check that at least one input has been created.
	if (GetNumInputs() == 0) {
		AddInputSlot(
			"Default",
			"D",
			"Default",
			VAR_STRING,
			ITEM,
			"Hello"
		);
	}

	//mandatory output slot "out"
	AddOutputSlot(
		"out",
		"out",
		"output",
		VAR_STRING,
		ITEM
	);


	//either way, remember this script file
	SetGenericData("LoadScript", true);
	SetGenericData("ScriptPath", pathToScript);
	SetGenericData("ClassName", className);

	return res;
}

IoScriptInstance::~IoScriptInstance()
{
	ReleaseObject();
}

void IoScriptInstance::RegisterObject(Context* context)
{
	//not sure if we need this


}

void IoScriptInstance::RegisterScriptObject(Urho3D::Context* context)
{
	//register this class to the engine
	Script* script_system = context->GetSubsystem<Script>();
	asIScriptEngine* engine = script_system->GetScriptEngine();

	engine->RegisterEnum("DataAccess");
	engine->RegisterEnumValue("DataAccess", "DA_ITEM", DataAccess::ITEM);
	engine->RegisterEnumValue("DataAccess", "DA_LIST", DataAccess::LIST);
	engine->RegisterEnumValue("DataAccess", "DA_TREE", DataAccess::TREE);

	engine->RegisterGlobalFunction("Scene@+ GetActiveScene()", asFUNCTION(GetActiveScene), asCALL_CDECL);
	engine->RegisterGlobalFunction("Viewport@+ GetActiveViewport()", asFUNCTION(GetActiveViewport), asCALL_CDECL);
	engine->RegisterGlobalFunction("void CreateInputSlot(String, String, int, Variant, String)", asFUNCTION(CreateInputSlot), asCALL_CDECL);
	engine->RegisterGlobalFunction("void CreateOutputSlot(String, String, int, String)", asFUNCTION(CreateOutputSlot), asCALL_CDECL);
}

void IoScriptInstance::HandleCustomInterface(Urho3D::UIElement* customElement)
{

	//errorText_ = SharedPtr<Text>(customElement->CreateChild<Text>("ErrorText"));
	//errorText_->SetStyle("SmallLabelText");
	//errorText_->SetColor(Color::RED);
	//errorText_->SetWordwrap(true);
	//errorText_->SetMaxHeight(300);
	////errorText_->GetParent()->SetMaxHeight(100);

	//if (!errorMessage_.Empty()) {
	//	errorText_->SetText(errorMessage_);
	//	errorText_->SetMaxHeight(100);
	//	errorText_->GetParent()->SetMaxHeight(100);
	//}
}

IoInputSlot* IoScriptInstance::AddInputSlotFromScript(Urho3D::VariantMap inputMap)
{
	bool check = inputMap.Keys().Contains("Name") &&
		inputMap.Keys().Contains("VariableName") &&
		inputMap.Keys().Contains("DataAccess");

	if (check) {

		String name = inputMap["Name"].GetString();
		String varName = inputMap["VariableName"].GetString();
		VariantType type = VAR_STRING;
		int daVal = inputMap["DataAccess"].GetInt();
		DataAccess da = static_cast<DataAccess>(daVal);

		//check that such a slot does not already exist
		int numSlots = GetNumInputs();
		bool isExistingSlot = false;
		for (int i = 0; i < numSlots; i++)
		{
			if (varName == inputSlots_[i]->GetVariableName())
			{
				isExistingSlot = true;

				//update accordingly
				inputSlots_[i]->SetName(name);
				inputSlots_[i]->SetVariableName(varName);
				inputSlots_[i]->SetDataAccess(da);


				if (inputMap.Keys().Contains("DefaultValue")) {
					Variant val = inputMap["DefaultValue"];
					inputSlots_[i]->SetDefaultValue(val);
				}

				String description = inputSlots_[i]->GetDescription();
				if (inputMap.Keys().Contains("Description")) {
					inputSlots_[i]->SetDescription(inputMap["Description"].GetString());
				}


				break;
			}
		}

		if (!isExistingSlot)
		{
			String description = "--No Description--";

			if (inputMap.Keys().Contains("Description")) {
				description = inputMap["Description"].GetString();
			}


			if (inputMap.Keys().Contains("DefaultValue")) {

				Variant val = inputMap["DefaultValue"];

				IoInputSlot* slot = AddInputSlot(
					name,
					varName,
					description,
					val.GetType(),
					da,
					val
				);

				return slot;
			}
			else {

				IoInputSlot* slot = AddInputSlot(
					name,
					varName,
					description,
					type,
					da
				);
				return slot;
			}
		}

	}

	return 0;
}

IoOutputSlot* IoScriptInstance::AddOutputSlotFromScript(Urho3D::VariantMap outputMap)
{
	bool check = outputMap.Keys().Contains("Name") &&
		outputMap.Keys().Contains("VariableName") &&
		outputMap.Keys().Contains("DataAccess");

	if (check) {

		String name = outputMap["Name"].GetString();
		String varName = outputMap["VariableName"].GetString();
		VariantType type = VAR_STRING;
		int daVal = outputMap["DataAccess"].GetInt();
		DataAccess da = static_cast<DataAccess>(daVal);


		//check that such a slot does not already exist
		int numSlots = GetNumOutputs();
		bool isExistingSlot = false;
		for (int i = 0; i < numSlots; i++)
		{
			if (varName == outputSlots_[i]->GetVariableName())
			{
				isExistingSlot = true;

				//update accordingly
				outputSlots_[i]->SetName(name);
				outputSlots_[i]->SetVariableName(varName);
				outputSlots_[i]->SetDataAccess(da);

				String description = outputSlots_[i]->GetDescription();
				if (outputMap.Keys().Contains("Description")) {
					outputSlots_[i]->SetDescription(outputMap["Description"].GetString());
				}


				break;
			}
		}

		if (!isExistingSlot)
		{
			String description = "--No Description--";

			if (outputMap.Keys().Contains("Description")) {
				description = outputMap["Description"].GetString();
			}

			IoOutputSlot* slot = AddOutputSlot(
				name,
				varName,
				description,
				type,
				da
			);

			return slot;
		}
	}

	return 0;
}


void IoScriptInstance::PreLocalSolve()
{
	if (methods_[METHOD_PRELOCALSOLVE]) {
		Execute(methods_[METHOD_PRELOCALSOLVE]);
	}
}

void IoScriptInstance::SolveInstance(const Urho3D::Vector<Urho3D::Variant>& inSolveInstance, Urho3D::Vector<Urho3D::Variant>& outSolveInstance)
{

	//make sure that user has defined a solve instance function
	if (!methods_[METHOD_SOLVEINSTANCE]) {

		SetAllOutputsNull(outSolveInstance);
		outSolveInstance[outSolveInstance.Size() - 1] = errorMessage_;
		return;
	}

	//create a map between variable names and input values
	VariantMap inArgs;
 	for (int i = 0; i < inSolveInstance.Size(); i++)
	{
		Variant inVar = inSolveInstance[i];
		String varName = inputSlots_[i]->GetVariableName();
		inArgs[varName] = inSolveInstance[i];
	}

	//create a map between variable names and output values
	VariantMap outArgs;
	for (int i = 0; i < outSolveInstance.Size(); i++)
	{
		outArgs[outputSlots_[i]->GetVariableName()] = 0.0f;
	}

	//push these to a vector for passing to the script
	VariantVector vars;
	vars.Push(inArgs);
	vars.Push(outArgs);

	//finally, call the script
	bool res = Execute(methods_[METHOD_SOLVEINSTANCE], vars);

	//make sure call was successful
	if (!res)
	{
		SetAllOutputsNull(outSolveInstance);
		outSolveInstance[outSolveInstance.Size() - 1] = errorMessage_;
		return;
	}

	//recover the output map and push to outSolveInstance
	VariantMap outMap = vars[1].GetVariantMap();

	for (int i = 0; i < outSolveInstance.Size(); i++)
	{
		String keyName = outputSlots_[i]->GetVariableName();
		if (keyName == "out")
		{
			continue;
		}

		if (outMap.Contains(keyName))
		{
			outSolveInstance[i] = outMap[keyName];
		}
		else
		{
			outSolveInstance[i] = Variant();
		}
	}
}

bool IoScriptInstance::CreateObject(ScriptFile* scriptFile, const String& className)
{
	className_ = String::EMPTY; // Do not create object during SetScriptFile()
	SetScriptFile(scriptFile);
	SetClassName(className);
	return scriptObject_ != 0;
}

void IoScriptInstance::SetScriptFile(ScriptFile* scriptFile)
{
	if (scriptFile == scriptFile_ && scriptObject_)
		return;

	ReleaseObject();

	// Unsubscribe from the reload event of previous script file (if any), then subscribe to the new
	if (scriptFile_)
	{
		UnsubscribeFromEvent(scriptFile_, E_RELOADSTARTED);
		UnsubscribeFromEvent(scriptFile_, E_RELOADFINISHED);
	}
	if (scriptFile)
	{
		SubscribeToEvent(scriptFile, E_RELOADSTARTED, URHO3D_HANDLER(IoScriptInstance, HandleScriptFileReload));
		SubscribeToEvent(scriptFile, E_RELOADFINISHED, URHO3D_HANDLER(IoScriptInstance, HandleScriptFileReloadFinished));
		SubscribeToEvent(E_SCRIPTERROR, URHO3D_HANDLER(IoScriptInstance, HandleScriptError));
	}

	scriptFile_ = scriptFile;
	CreateObject();

}

void IoScriptInstance::SetClassName(const String& className)
{
	if (className == className_ && scriptObject_)
		return;

	ReleaseObject();

	className_ = className;
	CreateObject();

}

bool IoScriptInstance::Execute(const String& declaration, const VariantVector& parameters)
{
	if (!scriptObject_)
		return false;

	asIScriptFunction* method = scriptFile_->GetMethod(scriptObject_, declaration);
	if (!method)
	{
		URHO3D_LOGERROR("Method " + declaration + " not found in class " + className_);
		return false;
	}

	return scriptFile_->Execute(scriptObject_, method, parameters);
}

bool IoScriptInstance::Execute(asIScriptFunction* method, const VariantVector& parameters)
{
	if (!method || !scriptObject_)
		return false;

	return scriptFile_->Execute(scriptObject_, method, parameters);
}

void IoScriptInstance::AddEventHandler(StringHash eventType, const String& handlerName)
{
	if (!scriptObject_)
		return;

	String declaration = "void " + handlerName + "(StringHash, VariantMap&)";
	asIScriptFunction* method = scriptFile_->GetMethod(scriptObject_, declaration);
	if (!method)
	{
		// Retry with parameterless signature
		method = scriptFile_->GetMethod(scriptObject_, handlerName);
		if (!method)
		{
			URHO3D_LOGERROR("Event handler method " + handlerName + " not found in " + scriptFile_->GetName());
			return;
		}
	}

	SubscribeToEvent(eventType, URHO3D_HANDLER_USERDATA(IoScriptInstance, HandleScriptEvent, (void*)method));
}

void IoScriptInstance::AddEventHandler(Object* sender, StringHash eventType, const String& handlerName)
{
	if (!scriptObject_)
		return;

	if (!sender)
	{
		URHO3D_LOGERROR("Null event sender for event " + String(eventType) + ", handler " + handlerName);
		return;
	}

	String declaration = "void " + handlerName + "(StringHash, VariantMap&)";
	asIScriptFunction* method = scriptFile_->GetMethod(scriptObject_, declaration);
	if (!method)
	{
		// Retry with parameterless signature
		method = scriptFile_->GetMethod(scriptObject_, handlerName);
		if (!method)
		{
			URHO3D_LOGERROR("Event handler method " + handlerName + " not found in " + scriptFile_->GetName());
			return;
		}
	}

	SubscribeToEvent(sender, eventType, URHO3D_HANDLER_USERDATA(IoScriptInstance, HandleScriptEvent, (void*)method));
}

void IoScriptInstance::RemoveEventHandler(StringHash eventType)
{
	UnsubscribeFromEvent(eventType);
}

void IoScriptInstance::RemoveEventHandler(Object* sender, StringHash eventType)
{
	UnsubscribeFromEvent(sender, eventType);
}

void IoScriptInstance::RemoveEventHandlers(Object* sender)
{
	UnsubscribeFromEvents(sender);
}

void IoScriptInstance::RemoveEventHandlers()
{
	UnsubscribeFromAllEventsExcept(PODVector<StringHash>(), true);
}

void IoScriptInstance::RemoveEventHandlersExcept(const PODVector<StringHash>& exceptions)
{
	UnsubscribeFromAllEventsExcept(exceptions, true);
}

bool IoScriptInstance::HasEventHandler(StringHash eventType) const
{
	return HasSubscribedToEvent(eventType);
}

bool IoScriptInstance::HasEventHandler(Object* sender, StringHash eventType) const
{
	return HasSubscribedToEvent(sender, eventType);
}

bool IoScriptInstance::IsA(const String& className) const
{
	// Early out for the easiest case where that's what we are
	if (className_ == className)
		return true;
	if (scriptObject_)
	{
		// Start immediately at the first base class because we already checked the early out
		asITypeInfo* currentType = scriptObject_->GetObjectType()->GetBaseType();
		while (currentType)
		{
			if (className == currentType->GetName())
				return true;
			currentType = currentType->GetBaseType();
		}
	}
	return false;
}

bool IoScriptInstance::HasMethod(const String& declaration) const
{
	if (!scriptFile_ || !scriptObject_)
		return false;
	else
		return scriptFile_->GetMethod(scriptObject_, declaration) != 0;
}

void IoScriptInstance::CreateObject()
{
	if (!scriptFile_ || className_.Empty())
		return;

	//URHO3D_PROFILE(CreateScriptObject);
	Script* script = GetSubsystem<Script>();
	scriptObject_ = scriptFile_->CreateObject(className_);
	if (scriptObject_)
	{
		// Map script object to script instance with userdata
		scriptObject_->SetUserData(this);
		asIScriptContext* sc = script->GetScriptFileContext();

		GetScriptMethods();
		UpdateEventSubscription();

		if (methods_[METHOD_START])
			scriptFile_->Execute(scriptObject_, methods_[METHOD_START]);
	}

}

void IoScriptInstance::ReleaseObject()
{
	if (scriptObject_)
	{

		//run prelocal solve
		if (methods_[METHOD_PRELOCALSOLVE]) {
			scriptFile_->Execute(scriptObject_, methods_[METHOD_PRELOCALSOLVE]);
		}

		if (methods_[METHOD_STOP])
			scriptFile_->Execute(scriptObject_, methods_[METHOD_STOP]);

		PODVector<StringHash> exceptions;
		exceptions.Push(E_RELOADSTARTED);
		exceptions.Push(E_RELOADFINISHED);
		exceptions.Push(E_SCRIPTERROR);
		UnsubscribeFromAllEventsExcept(exceptions, false);

		subscribed_ = false;

		ClearScriptMethods();

		scriptObject_->SetUserData(0);
		scriptObject_->Release();
		scriptObject_ = 0;

		//remove all inputs and outputs??
	}
}

void IoScriptInstance::ClearScriptMethods()
{
	for (unsigned i = 0; i < MAX_SCRIPT_METHODS; ++i)
		methods_[i] = 0;
}

void IoScriptInstance::GetScriptMethods()
{
	for (unsigned i = 0; i < MAX_SCRIPT_METHODS; ++i) {
		methods_[i] = scriptFile_->GetMethod(scriptObject_, methodDeclarations[i]);
	}

}

void IoScriptInstance::UpdateEventSubscription()
{
	Scene* scene = (Scene*)GetGlobalVar("Scene").GetPtr();
	if (!scene)
	{
		URHO3D_LOGWARNING("Node is detached from scene, can not subscribe script object to update events");
		return;
	}

	bool enabled = scriptObject_;

	if (enabled)
	{
		if (!subscribed_ && methods_[METHOD_UPDATE])
		{
			SubscribeToEvent(scene, E_SCENEUPDATE, URHO3D_HANDLER(IoScriptInstance, HandleSceneUpdate));
			subscribed_ = true;
		}

	}
	else
	{
		if (subscribed_)
		{
			UnsubscribeFromEvent(scene, E_SCENEUPDATE);
			subscribed_ = false;
		}

	}
}

void IoScriptInstance::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{
	if (!scriptObject_)
		return;

	using namespace SceneUpdate;

	float timeStep = eventData[P_TIMESTEP].GetFloat();

	if (methods_[METHOD_UPDATE])
	{
		VariantVector parameters;
		parameters.Push(timeStep);
		scriptFile_->Execute(scriptObject_, methods_[METHOD_UPDATE], parameters);
	}
}

void IoScriptInstance::HandleScriptEvent(StringHash eventType, VariantMap& eventData)
{
	if (!scriptFile_ || !scriptObject_)
		return;

	asIScriptFunction* method = static_cast<asIScriptFunction*>(GetEventHandler()->GetUserData());

	VariantVector parameters;
	if (method->GetParamCount() > 0)
	{
		parameters.Push(Variant((void*)&eventType));
		parameters.Push(Variant((void*)&eventData));
	}

	scriptFile_->Execute(scriptObject_, method, parameters);
}

void IoScriptInstance::HandleScriptFileReload(StringHash eventType, VariantMap& eventData)
{
	ReleaseObject();
}

void IoScriptInstance::HandleScriptFileReloadFinished(StringHash eventType, VariantMap& eventData)
{
	if (!className_.Empty() && !scriptPath_.Empty())
	{
		CreateObjectFromPath(scriptPath_, className_);
		SendEvent("NodeRedrawRequest");
	}
}

void IoScriptInstance::HandleScriptError(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	using namespace ScriptError;

	String scriptPath = eventData[P_SCRIPTPATH].GetString();
	int row = eventData[P_ROW].GetInt();
	int col = eventData[P_COL].GetInt();
	String msg = eventData[P_MSG].GetString();

	if (scriptPath == scriptPath_) {
		String message;
		message.AppendWithFormat("%s:%d,%d %s", scriptPath.CString(), row, col, msg.CString());
		errorMessage_ = message;

		//force the error message in to output slot.
		if (outputSlots_.Size() > 0)
			outputSlots_[outputSlots_.Size() - 1]->SetIoDataTree(IoDataTree(GetContext(), errorMessage_));

		SendEvent("GraphNodeError");
	}
}



