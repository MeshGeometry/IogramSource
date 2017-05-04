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

#include <Urho3D/Container/List.h>
#include <Urho3D/Core/Object.h>

using namespace Urho3D;

/// Implements plugin loading functionality.
class URHO3D_API PluginAPI : public Object
{
    URHO3D_OBJECT(PluginAPI, Object)

public:
	PluginAPI(Context* context);
	virtual ~PluginAPI() {};
    /// Returns list of plugin configuration files that were used to load the plugins at startup.
    Vector<String> ConfigurationFiles() const;

    /// Loads and executes the given shared library plugin.
    void LoadPlugin(const String &filename);

    /// Parses the specified .xml file and loads and executes all plugins specified in that file.
    void LoadPluginsFromFile(String pluginListFilename);

	void LoadPluginsFromDirectory(String folderPath);

    /// Loads plugins specified on command line with --plugin
    void LoadPluginsFromCommandLine();

    void UnloadPlugins();

    /// Prints the list of loaded plugins to the console.
    void ListPlugins() const;

	bool IsPluginLoaded(String name);

private:
    //friend class Framework;

    /// @note Framework takes ownership of the object.
   // explicit PluginAPI(Framework *framework);

    struct Plugin
    {
        void *handle;
        String name;
        String filename;
    };
    Vector<Plugin> plugins;

    //Framework *owner;
};


