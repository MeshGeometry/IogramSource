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


#include "IoSerialization.h"
#include "IoScriptInstance.h"
#include <Urho3D/IO/File.h>

using namespace Urho3D;

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

Urho3D::Context* IoSerialization::context_;
IoGraph* IoSerialization::currentGraph_;

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

void IoSerialization::SaveGraph(IoGraph const & graph, String path)
{
	//track the context
	context_ = graph.GetContext();

	//create the file
	File* dest = new File(context_, path, Urho3D::FileMode::FILE_WRITE);

	//create the json file
	SharedPtr<JSONFile> json(new JSONFile(context_));
	JSONValue& rootElem = json->GetRoot();

	//create json value for the graph
	JSONValue graphVal;

	//add components
	JSONValue compVal;
	JSONArray compArray;

	for (unsigned i = 0; i < graph.components_.Size(); i++)
	{
		SharedPtr<IoComponentBase> node = graph.components_[i];

		//create a json value for this node
		JSONValue nodeVal;

		//write base data
		nodeVal.Set("ID", node->ID);
		nodeVal.Set("Name", node->Name);
		nodeVal.Set("type", node->GetTypeName());
		nodeVal.Set("num_inputs", node->GetNumInputs());
		nodeVal.Set("num_outputs", node->GetNumOutputs());
		nodeVal.Set("preview", node->IsPreviewEnabled());
		nodeVal.Set("enabled", node->IsSolveEnabled());
		nodeVal.Set("unique_view_name", node->GetUniqueViewName());
		nodeVal.Set("coordinates", node->GetCoordinates().ToString());
		nodeVal.Set("view_size", node->GetViewSize().ToString());

		//write meta data
		JSONValue metaValue;
		SaveMetaData(node->metaData_, metaValue);
		nodeVal.Set("metadata", metaValue);

		//write input slot array
		JSONArray inputArray;
		for (unsigned j = 0; j < node->inputSlots_.Size(); j++)
		{
			SharedPtr<IoInputSlot> slot = node->inputSlots_[j];
			JSONValue slotVal;

			slotVal.Set("home_component", slot->homeComponent_->ID);
			slotVal.Set("slot_index", j);
			slotVal.Set("data_access", slot->dataAccess_);
			slotVal.Set("slot_var_label", slot->variableName_);
			slotVal.Set("slot_label", slot->name_);
			slotVal.Set("protected", slot->GetIsProtected());

			SharedPtr<IoOutputSlot> linkedSlot = slot->linkedOutputSlot_;
			if (linkedSlot.NotNull())
			{
				int slotIndex = linkedSlot->homeComponent_->GetOutputSlotIndex(linkedSlot);
				slotVal.Set("linked_component", linkedSlot->homeComponent_->ID);
				slotVal.Set("linked_slot_index", slotIndex);
			}
			else
			{
				slotVal.Set("linked_component", "");
				slotVal.Set("linked_slot_index", -1);

				//store input data tree
				JSONValue treeVal;
				SaveDataTree(slot->ioDataTree_, treeVal);

				slotVal.Set("input_tree", treeVal);
			}

			//push to array
			inputArray.Push(slotVal);
		}

		//write input array to node val
		nodeVal.Set("input_slots", inputArray);

		//write output slot array
		JSONArray outputArray;
		for (unsigned j = 0; j < node->outputSlots_.Size(); j++)
		{
			SharedPtr<IoOutputSlot> slot = node->outputSlots_[j];
			JSONValue slotVal;

			slotVal.Set("home_component", slot->homeComponent_->ID);
			slotVal.Set("slot_index", j);

			JSONArray linkedInputArray;
			for (unsigned k = 0; k < slot->linkedInputSlots_.Size(); k++)
			{
				SharedPtr<IoInputSlot> linkedSlot = slot->linkedInputSlots_[k];
				JSONValue linkedVal;
				if (linkedSlot.NotNull())
				{
					int slotIndex = linkedSlot->homeComponent_->GetInputSlotIndex(linkedSlot);
					linkedVal.Set("linked_component", linkedSlot->homeComponent_->ID);
					linkedVal.Set("linked_slot_index", slotIndex);
				}
				else
				{
					linkedVal.Set("linked_component", "");
					linkedVal.Set("linked_slot_index", -1);
				}

				linkedInputArray.Push(linkedVal);
			}

			slotVal.Set("linked_inputs", linkedInputArray);

			//push to array
			outputArray.Push(slotVal);
		}

		nodeVal.Set("output_slots", outputArray);

		//push this node to the component array
		compArray.Push(nodeVal);
	}

	//push the component array to the root
	graphVal.Set("components", compArray);

	//push the graph to the root object
	rootElem.Set("graph", graphVal);

	//write to json file
	json->Save(*dest, "\t");

	//flush stream
	dest->Close();
}

void IoSerialization::LoadGraph(IoGraph & graph, File* source)
{
	//track the context
	context_ = graph.GetContext();

	//source->Seek(0);

	if (source == NULL)
	{
		URHO3D_LOGINFO("Could not open file");
		return;
	}

	//create the json file
	SharedPtr<JSONFile> json(new JSONFile(context_));

	//load file in to json
	json->Load(*source);

	//get the graph
	const JSONValue& graphVal = json->GetRoot().Get("graph");

	if (graphVal.IsNull())
	{
		URHO3D_LOGINFO("could not load graph at path");
		return;
	}

	//loop through the components array to instantiate
	const JSONArray& compArray = graphVal.Get("components").GetArray();
	Vector<Pair<int, int>> loadedCompID;

	for (unsigned i = 0; i < compArray.Size(); i++)
	{
		const JSONValue& compVal = compArray[i];

		//get some relevant data for instantiation
		int numInputs = compVal.Get("num_inputs").GetInt();
		int numOutputs = compVal.Get("num_outputs").GetInt();
		String ID = compVal.Get("ID").GetString();
		String Name = compVal.Get("Name").GetString();
		String type = compVal.Get("type").GetString();
		Variant posVar(VAR_VECTOR2, compVal.Get("screen_pos").GetString());
		Variant colVar(VAR_COLOR, compVal.Get("color").GetString());
		bool preview = compVal.Get("preview").GetBool();
		bool enabled = compVal.Get("enabled").GetBool();
		String viewName = compVal.Get("unique_view_name").GetString();
		Variant coords(VAR_INTVECTOR2, compVal.Get("coordinates").GetString());
		Variant viewSize(VAR_INTVECTOR2, compVal.Get("view_size").GetString());

		//create the component
		// First check if factory for type exists
		StringHash typeHash(type);
		if (!graph.GetContext()->GetTypeName(typeHash).Empty())
		{
			StringHash typeHash(type);
			SharedPtr<Object> newObj = graph.GetContext()->CreateObject(typeHash);
			SharedPtr<IoComponentBase> newComp = DynamicCast<IoComponentBase>(newObj);

			//read metadata
			const JSONValue metaVal = compVal.Get("metadata");
			HashMap<String, Pair<String, Variant>> data;
			LoadMetaData(data, metaVal);

			if (!data.Empty()) {
				newComp->metaData_ = data;
			}

			//handle special case of script file
			bool loadScript = newComp->GetGenericData("LoadScript").GetBool();
			if (loadScript) {
				String scriptPath = newComp->GetGenericData("ScriptPath").GetString();
				String className = newComp->GetGenericData("ClassName").GetString();
				SharedPtr<IoScriptInstance> scriptInstance = DynamicCast<IoScriptInstance>(newComp);
				scriptInstance->CreateObjectFromPath(scriptPath, className);
			}

			//if there is a mismatch between input or output count in file and current version of component, continue
			if (numInputs < newComp->GetNumInputs()
				|| numOutputs < newComp->GetNumOutputs())
			{
				URHO3D_LOGINFO("Version mismatch in component of type: " + type);
				//continue;
			}


			//make sure that num inputs in file matches default num inputs
			if (numInputs > newComp->GetNumInputs())
			{
				int orgNumInputs = newComp->GetNumInputs();
				for (int i = 0; i < numInputs - orgNumInputs; i++)
				{
					JSONValue slotVal = compVal.Get("input_slots")[orgNumInputs + i];
					String sName = "Custom" + String(i);
					String varLabel = "CS" + String(i);
					DataAccess da = DataAccess::ITEM;
					bool isProtected = slotVal["protected"].GetBool();
					if (!slotVal["data_access"].IsNull())
					{

					}
					if (!slotVal["slot_var_label"].IsNull())
					{
						varLabel = slotVal["slot_var_label"].GetString();
					}
					if (!slotVal["slot_label"].IsNull())
					{
						sName = slotVal["slot_label"].GetString();
					}

					IoInputSlot* iSlot = newComp->AddInputSlot(
						sName,
						varLabel,
						"A Custom Slot",
						VAR_STRING,
						DataAccess::ITEM
					);

					iSlot->SetIsProtected(isProtected);
	
				}
			}

			newComp->ID = ID;
			newComp->Name = Name;
			newComp->type = type;

			(preview) ? newComp->EnablePreview() : newComp->DisablePreview();
			(enabled) ? newComp->EnableSolve() : newComp->DisableSolve();
			if (enabled) { newComp->ClearOutputs(); }

			//set view name and coords
			newComp->SetUniqueViewName(viewName);
			newComp->SetCoordinates(coords.GetIntVector2());
			newComp->SetViewSize(viewSize.GetIntVector2());

			graph.AddNewComponent(newComp);
			int id = graph.GetComponentIndex(newComp->ID);
			Pair<int, int> p(id, i);
			loadedCompID.Push(p);
		}
		else
		{
			URHO3D_LOGINFO("No Iogram component of that type currently registered: " + type);
			continue;
		}

	}

	//loop to link the pointers
	//TODO: loop only through successfully created components
	for (unsigned i = 0; i < loadedCompID.Size(); i++)
	{
		int id = loadedCompID[i].first_;
		SharedPtr<IoComponentBase> node = graph.components_[id]; //maybe better to look from ID?
		const JSONArray& inputs = compArray[loadedCompID[i].second_].Get("input_slots").GetArray();

		if (node->GetTypeName() == "Mesh_FillPolygon")
		{
			int test = 5;
		}

		for (unsigned j = 0; j < inputs.Size(); j++)
		{
			int linkedIndex = inputs[j].Get("linked_slot_index").GetInt();
			int slotIndex = inputs[j].Get("slot_index").GetInt();
			if (linkedIndex == -1)
			{
				//set the value from file
				const JSONValue inTreeVal = inputs[j].Get("input_tree");
				IoDataTree inTree(context_);
				LoadDataTree(inTree, inTreeVal);
				Vector<int> path = inTree.Begin();
				if (path.Size() > 0)
				{
					if (inTree.GetNumItemsAtBranch(path, DataAccess::ITEM) > 0)
					{
						node->InputHardSet(slotIndex, inTree);
					}
				}
			}
			else
			{
				//create a new link via the graph
				String linkedID = inputs[j].Get("linked_component").GetString();
				int parentIndex = graph.GetComponent(linkedID);

				if (parentIndex >= 0)
				{
					SharedPtr<IoComponentBase> linkedNode = graph.GetComponentPtr(parentIndex);
					if (linkedNode.NotNull())
					{
						graph.AddConnection(parentIndex, linkedIndex, id, slotIndex);
					}
				}
			}
		}
	}

	//close the file
	source->Close();

	//init calcs
	graph.UpdateRoots();
}

void IoSerialization::LoadGraph(IoGraph & graph, String path)
{
	//read the file
	File* source = new File(graph.GetContext(), path, FileMode::FILE_READ);

	if (source == NULL)
	{
		URHO3D_LOGINFO("could not read file at path: " + path);
		return;
	}

	LoadGraph(graph, source);

	source->Close();
}

void IoSerialization::SaveDataTree(IoDataTree& tree, JSONValue& treeVal)
{
	//create branches array
	JSONArray branchArr;
	HashMap<String, IoBranch*>::ConstIterator itr;
	for (itr = tree.branches_.Begin(); itr != tree.branches_.End(); itr++)
	{
		JSONValue bVal;
		JSONArray bItems;
		int numItems = itr->second_->data.Size();
		for (int i = 0; i < numItems; i++)
		{
			//TODO: only store basic types
			Variant var = itr->second_->data[i];

			if (var.GetType() == VAR_VARIANTVECTOR || var.GetType() == VAR_VARIANTMAP)
				continue;

			JSONValue varVal;
			varVal.Set("var_type", var.GetTypeName());
			varVal.Set("var_value", var.ToString());

			bItems.Push(varVal);


		}

		bVal.Set("path", itr->first_);
		bVal.Set("items", bItems);
		branchArr.Push(bVal);
	}

	treeVal.Set("branches", branchArr);
}


void IoSerialization::LoadDataTree(IoDataTree& tree, const JSONValue& treeVal)
{
	//iterate over the branches value
	const JSONArray& branches = treeVal.Get("branches").GetArray();
	for (unsigned i = 0; i < branches.Size(); i++)
	{
		String pathString = branches[i]["path"].GetString();
		Vector<int> path = tree.PathFromUniqueString(pathString);

		const JSONArray& bItems = branches[i]["items"].GetArray();
		for (unsigned j = 0; j < bItems.Size(); j++)
		{
			String type = bItems[j]["var_type"].GetString();
			String val = bItems[j]["var_value"].GetString();

			Variant var(type, val);
			tree.Add(path, var);
		}
	}
}

void IoSerialization::SaveMetaData(HashMap<String, Pair<String, Variant>>& data, JSONValue& treeVal)
{
	HashMap<String, Pair<String, Variant>>::ConstIterator itr;
	for (itr = data.Begin(); itr != data.End(); itr++)
	{
		const String keyName = itr->second_.first_;
		const Variant var = itr->second_.second_;

		if (keyName == "")
			continue;

		JSONValue varVal;
		varVal.Set("type", var.GetTypeName());
		varVal.Set("value", var.ToString());
		treeVal.Set(keyName, varVal);
	}
}

void IoSerialization::LoadMetaData(HashMap<String, Pair<String, Variant>>& data, const JSONValue& treeVal)
{
	if (treeVal.IsNull())
	{
		return;
	}

	ConstJSONObjectIterator itr;
	for (itr = treeVal.Begin(); itr != treeVal.End(); itr++)
	{
		String keyName = itr->first_;
		const JSONValue& jVal = itr->second_;

		String type = jVal["type"].GetString();
		String value = jVal["value"].GetString();
		Variant val(type, value);

		Pair<String, Variant> p(keyName, val);
		data[keyName] = p;
	}
}