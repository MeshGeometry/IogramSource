#pragma once

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>

class ComponentRegistration
{
public:
	static void RegisterCoreComponents(Urho3D::Context* context);
	template <class T> static void RegisterIogramType(Urho3D::Context* context);
	template <class T, class V> static void RegisterIogramType(Urho3D::Context* context);
};