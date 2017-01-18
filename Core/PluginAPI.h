
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


