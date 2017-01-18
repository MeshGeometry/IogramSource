#pragma once

#include "IoGraph.h"
#include "IoComponentBase.h"
#include "IoDataTree.h"

class IoSerialization
{
private:
	static Urho3D::Context* context_;
	static Urho3D::File* destinaton_;

public:
	static IoGraph * currentGraph_;
	static void SaveGraph(IoGraph const & graph, Urho3D::String path);
	static void LoadGraph(IoGraph & graph, Urho3D::String path);
	static void LoadGraph(IoGraph & graph, Urho3D::File* file);
	static void SetContext(Urho3D::Context* context) { context_ = context; };
	static Urho3D::Context* GetContext() { return context_; };
	static void SaveDataTree(IoDataTree& tree, Urho3D::JSONValue& treeVal);
	static void LoadDataTree(IoDataTree& tree, const Urho3D::JSONValue& treeVal);
	static void SaveMetaData(Urho3D::HashMap<Urho3D::String, Urho3D::Pair<Urho3D::String, Urho3D::Variant>>& data, Urho3D::JSONValue& treeVal);
	static void LoadMetaData(Urho3D::HashMap<Urho3D::String, Urho3D::Pair<Urho3D::String, Urho3D::Variant>>& data, const Urho3D::JSONValue& treeVal);
};