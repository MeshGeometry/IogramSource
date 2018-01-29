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

#pragma once

#include <Urho3D/AngelScript/ScriptEventListener.h>
#include <Urho3D/AngelScript/Script.h>
#include <Urho3D/AngelScript/ScriptFile.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Scene/Scene.h>

#include "IoComponentBase.h"

class asIScriptFunction;
class asIScriptObject;
class asIScriptContext;


/// Inbuilt scripted component methods.
enum ScriptInstanceMethod
{
	METHOD_START = 0,
	METHOD_STOP,
	METHOD_DEFINEINPUTS,
	METHOD_DEFINEOUTPUTS,
	METHOD_PRELOCALSOLVE,
	METHOD_SOLVEINSTANCE,
	METHOD_UPDATE,
	MAX_SCRIPT_METHODS
};

/// %Script object component.
class URHO3D_API IoScriptInstance : public IoComponentBase, public Urho3D::ScriptEventListener
{
	URHO3D_OBJECT(IoScriptInstance, IoComponentBase);

public:
	/// Construct.
	IoScriptInstance(Urho3D::Context* context);
	/// Destruct.
	virtual ~IoScriptInstance();
	/// Register object factory.
	static void RegisterObject(Urho3D::Context* context);
	/// Register script object
	static void RegisterScriptObject(Urho3D::Context* context);
	/// Add a scripted event handler.
	virtual void AddEventHandler(Urho3D::StringHash eventType, const Urho3D::String& handlerName);
	/// Add a scripted event handler for a specific sender.
	virtual void AddEventHandler(Urho3D::Object* sender, Urho3D::StringHash eventType, const Urho3D::String& handlerName);
	/// Remove a scripted event handler.
	virtual void RemoveEventHandler(Urho3D::StringHash eventType);
	/// Remove a scripted event handler for a specific sender.
	virtual void RemoveEventHandler(Urho3D::Object* sender, Urho3D::StringHash eventType);
	/// Remove all scripted event handlers for a specific sender.
	virtual void RemoveEventHandlers(Urho3D::Object* sender);
	/// Remove all scripted event handlers.
	virtual void RemoveEventHandlers();
	/// Remove all scripted event handlers, except those listed.
	virtual void RemoveEventHandlersExcept(const Urho3D::PODVector<Urho3D::StringHash>& exceptions);
	/// Return whether has subscribed to an event.
	virtual bool HasEventHandler(Urho3D::StringHash eventType) const;
	/// Return whether has subscribed to a specific sender's event.
	virtual bool HasEventHandler(Urho3D::Object* sender, Urho3D::StringHash eventType) const;
	/// create iogram component from script
	bool CreateObjectFromPath(Urho3D::String pathToScript, Urho3D::String className);
	/// Create object of certain class from the script file. Return true if successful.
	bool CreateObject(Urho3D::ScriptFile* scriptFile, const Urho3D::String& className);
	/// Set script file only. Recreate object if necessary.
	void SetScriptFile(Urho3D::ScriptFile* scriptFile);
	/// Set class name only. Recreate object if necessary.
	void SetClassName(const Urho3D::String& className);
	/// Query for a method by declaration and execute. Log an error if not found.
	bool Execute(const Urho3D::String& declaration, const Urho3D::VariantVector& parameters = Urho3D::Variant::emptyVariantVector);
	/// Execute a method.
	bool Execute(asIScriptFunction* method, const Urho3D::VariantVector& parameters = Urho3D::Variant::emptyVariantVector);
	/// Return script file.
	Urho3D::ScriptFile* GetScriptFile() const { return scriptFile_; }
	/// Return script object.
	asIScriptObject* GetScriptObject() const { return scriptObject_; }
	/// Return class name.
	const Urho3D::String& GetClassName() const { return className_; }
	/// Check if the object is derived from a class.
	bool IsA(const Urho3D::String& className) const;
	/// Check if has a method.
	bool HasMethod(const Urho3D::String& declaration) const;

	/// create slots for variant map definition
	IoInputSlot* AddInputSlotFromScript(Urho3D::VariantMap inputMap);
	/// create slots for variant map definition
	IoOutputSlot* AddOutputSlotFromScript(Urho3D::VariantMap inputMap);
	
	virtual void PreLocalSolve();

	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);

	void SolveInstance(const Urho3D::Vector<Urho3D::Variant>& inSolveInstance, Urho3D::Vector<Urho3D::Variant>& outSolveInstance);

private:
	/// (Re)create the script object and check for supported methods if successfully created.
	void CreateObject();
	/// Release the script object.
	void ReleaseObject();
	/// Check for supported script methods.
	void GetScriptMethods();
	/// Clear supported script methods.
	void ClearScriptMethods();
	/// Subscribe/unsubscribe from scene updates as necessary.
	void UpdateEventSubscription();
	/// Handle scene update event.
	void HandleSceneUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	/// Handle an event in script.
	void HandleScriptEvent(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	/// Handle script file reload start.
	void HandleScriptFileReload(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	/// Handle script file reload finished.
	void HandleScriptFileReloadFinished(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	/// Handle script file reload finished.
	void HandleScriptError(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	///ui element for error display
	Urho3D::SharedPtr<Urho3D::Text> errorText_;
	/// Script file.
	Urho3D::WeakPtr<Urho3D::ScriptFile> scriptFile_;
	/// Script object.
	asIScriptObject* scriptObject_;
	/// Class name.
	Urho3D::String className_;
	/// track path
	Urho3D::String scriptPath_;
	///error message
	Urho3D::String errorMessage_;
	/// Pointers to supported inbuilt methods.
	asIScriptFunction* methods_[7];
	/// Subscribed to scene update events flag.
	bool subscribed_;
};


///// Return the active AngelScript context. Provided as a wrapper to the AngelScript API function to avoid undefined symbol error in shared library Urho3D builds.
//URHO3D_API asIScriptContext* GetActiveASContext();
///// Return the Urho3D context of the active AngelScript context.
//URHO3D_API Urho3D::Context* GetScriptContext();
///// Return the ScriptInstance of the active AngelScript context.
//URHO3D_API Urho3D::ScriptInstance* GetScriptContextInstance();
///// Return the scene node of the active AngelScript context.
//URHO3D_API Urho3D::Node* GetScriptContextNode();
///// Return the scene of the active AngelScript context.
//URHO3D_API Urho3D::Scene* GetScriptContextScene();
///// Return the event listener of the active AngelScript context.
//URHO3D_API Urho3D::ScriptEventListener* GetScriptContextEventListener();
///// Return the event listener of the active AngelScript context as an Object pointer.
//URHO3D_API Urho3D::Object* GetScriptContextEventListenerObject();