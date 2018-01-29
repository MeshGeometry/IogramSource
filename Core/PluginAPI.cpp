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


#include "Win.h"
#include "PluginAPI.h"
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Resource/JSONFile.h>
#include <Urho3D/Container/Str.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>

#ifndef WIN32
#include <dlfcn.h>
#endif

using namespace Urho3D;


/// @todo Move to SystemInfo?
static String GetErrorString(int error)
{
#ifdef WIN32
    void *lpMsgBuf = 0;

    HRESULT hresult = HRESULT_FROM_WIN32(error);
    if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        0, hresult, 0 /*Default language*/, (LPWSTR)&lpMsgBuf, 0, 0) == 0)
    {
        return ""; /**< @todo return error message describing the failure of this function and/or LogError() print?*/
    }

    // Copy message to C++ -style string, since the data need to be freed before return.
    String ret;
    ret += String(WString((wchar_t*)lpMsgBuf));
    ret += "(";
    ret += error;
    ret += ")";
    LocalFree(lpMsgBuf);
    return ret;
#else
    return String(strerror(error)) + "(" + String(error) + ")";
#endif

	return "Not implemented...";
}

/// Signature for Tundra plugins
typedef void (*PluginMainSignature)(Context *owner);

PluginAPI::PluginAPI(Context* context) :
    Object(context)
{
}

bool PluginAPI::IsPluginLoaded(String name)
{
	for (unsigned i = 0; i < plugins.Size(); i++)
	{
		if (plugins[i].name.ToLower() == name.ToLower())
		{
			return true;
		}
	}

	return false;
}

void PluginAPI::LoadPlugin(const String &filename)
{
#ifdef WIN32
#ifdef _DEBUG
const String pluginSuffix = "_d.dll";
#else
const String pluginSuffix = ".dll";
#endif
#elif defined(__APPLE__)

    #ifdef _DEBUG
        const String pluginSuffix = "_d.dylib";
    #else
        const String pluginSuffix = ".dylib";
    #endif

#else
	const String pluginSuffix = ".so";
#endif
	String path;
#ifdef ANDROID
	/// \todo Should not hardcode the package name, but transmit it from Java to native code
	// Note that using just dlopen() with no path name will not succeed
	path = "/data/data/" + owner->PackageName() + "/lib/lib" + filename.Trimmed() + pluginSuffix;
#else
	path = GetNativePath(filename.Trimmed() + pluginSuffix);
#endif
	FileSystem* fs = GetSubsystem<FileSystem>();
	if (!fs->FileExists(path))
	{
		URHO3D_LOGWARNING("Cannot load plugin \"" + path + "\" as the file does not exist.");
		return;
	}
	URHO3D_LOGINFO("  " + filename);
	//owner->App()->SetSplashMessage("Loading plugin " + filename);
#ifdef WIN32
	HMODULE module = LoadLibraryW(WString(path).CString());
	if (module == NULL)
	{
		DWORD errorCode = GetLastError();
		URHO3D_LOGINFO("Failed to load plugin from \"" + path + "\": " + GetErrorString(errorCode) + " (Missing dependencies?)");
		return;
	}
	PluginMainSignature mainEntryPoint = (PluginMainSignature)GetProcAddress(module, "IogramPluginMain");
	if (mainEntryPoint == NULL)
	{
		DWORD errorCode = GetLastError();
		URHO3D_LOGINFO("Failed to find plugin startup function 'IogramPluginMain' from plugin file \"" + path + "\": " + GetErrorString(errorCode));
		return;
	}
#else
	const char *dlerrstr;
	dlerror();

	void *module = dlopen(path.CString(), RTLD_GLOBAL | RTLD_LAZY | RTLD_LOCAL);

	if ((dlerrstr = dlerror()) != 0)
	{
		URHO3D_LOGERROR("Failed to load plugin from file \"" + path + "\": Error " + String(dlerrstr) + "!");
		return;
	}
	dlerror();
    void* symbol = dlsym(module, "GetMeshFromVoronoiCell");
	PluginMainSignature mainEntryPoint = (PluginMainSignature)dlsym(module, "IogramPluginMain");
	if ((dlerrstr = dlerror()) != 0)
	{
		URHO3D_LOGERROR("Failed to find plugin startup function 'IogramPluginMain' from plugin file \"" + path + "\": Error " + String(dlerrstr) + "!");
		return;
	}
#endif
	Plugin p = { module, filename, path };
	plugins.Push(p);
	mainEntryPoint(context_);
}

void PluginAPI::UnloadPlugins()
{
    for(unsigned i = 0; i < plugins.Size(); i++)
    {
#ifdef WIN32
        FreeLibrary((HMODULE)plugins[i].handle);
#else
        dlclose(plugins[i].handle);
#endif
    }
    plugins.Clear();
}

void PluginAPI::ListPlugins() const
{
    URHO3D_LOGINFO("Loaded Plugins");
	for (unsigned i = 0; i < plugins.Size(); i++)
	{
		URHO3D_LOGINFO("  " + plugins[i].name);
	}
}

Vector<String> PluginAPI::ConfigurationFiles() const
{
    Vector<String> configs;
    //Vector<String> cmdLineParams = owner->CommandLineParameters("--config");
    //if (cmdLineParams.Size() > 0)
    //{
    //    foreach(const String &config, cmdLineParams)
    //        configs.Push(owner->LookupRelativePath(config));
    //}
    return configs;
}

void PluginAPI::LoadPluginsFromDirectory(String folderPath)
{
	FileSystem* fs = GetSubsystem<FileSystem>();
	if (!fs->DirExists(folderPath))
	{
		URHO3D_LOGERROR("No folder found at path: " + folderPath);
		return;
	}

	//add trailing slash
	folderPath = AddTrailingSlash(folderPath);


	Vector<String> results;
	fs->ScanDir(results, folderPath, "", SCAN_FILES, true);

	for (int i = 0; i < results.Size(); i++)
	{
		String currPath = results[i];
		String fileName = GetFileName(folderPath + currPath);

		//check for debug suffix
		if (fileName.Contains("_d"))
		{
			fileName = fileName.Split('_')[0];
		}

		//try to load this plugin
		LoadPlugin(folderPath + fileName);
	}
}

void PluginAPI::LoadPluginsFromFile(String pluginConfigurationFile)
{
	FileSystem* fs = GetSubsystem<FileSystem>();
	if (!fs->FileExists(pluginConfigurationFile)) {
		URHO3D_LOGWARNING("No plugin config file found at path: " + pluginConfigurationFile);
		return;
	}

	File* f = new File(GetContext(), pluginConfigurationFile);
	JSONFile* jFile = new JSONFile(GetContext());
	jFile->Load(*f);

	JSONValue& root = jFile->GetRoot();

	if (!root["Plugins"].IsNull())
	{
		JSONArray pArray = root["Plugins"].GetArray();
		for (unsigned i = 0; i < pArray.Size(); i++)
		{
			JSONValue& nameVal = pArray[i]["Name"];
			JSONValue& editorVals = pArray[i]["Editor"];

			if (!nameVal.IsNull() && !editorVals.IsNull())
			{
				//TODO: detect what platform we are in
				String pluginName = nameVal.GetString();

				//this function will append the right suffix, so make sure the name is actually just the name
				LoadPlugin(pluginName);
			}
		}
	}

}

void PluginAPI::LoadPluginsFromCommandLine()
{
    //if (!owner->HasCommandLineParameter("--plugin"))
    //    return;

    //LogInfo("");
    //LogInfo("Loading");

    //Vector<String> plugins = owner->CommandLineParameters("--plugin");
    //foreach(String plugin, plugins)
    //{
    //    plugin = plugin.Trimmed();
    //    if (!plugin.Contains(";"))
    //        LoadPlugin(plugin);
    //    else
    //    {
    //        Vector<String> entries = plugin.Split(';');
    //        foreach(const String& entry, entries)
    //            LoadPlugin(entry);
    //    }
    //}
}

