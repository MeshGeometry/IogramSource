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