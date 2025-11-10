#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

#include "Component.h"

class Actor;

class ComponentFactory final
{
public:
	Component* CreateComponentAlloc(const std::string& typeName, Actor* const pOwner);

	static void Initialize();

	static ComponentFactory& GetInstance()
	{
		return *spInstance;
	}

	static void Destroy()
	{
		delete spInstance;
	}

private:
	static ComponentFactory* spInstance;

	std::vector<const char*> mComponentNames;

	using ComponentConstructor = std::function<Component* (Actor* const)>;
	std::unordered_map<std::string, ComponentConstructor> mComponentConstructors;

private:
	ComponentFactory();
	~ComponentFactory() = default;

private:
	ComponentFactory(const ComponentFactory& other) = delete;
	ComponentFactory(ComponentFactory&& other) = delete;
	ComponentFactory& operator=(const ComponentFactory& other) = delete;
	ComponentFactory& operator=(ComponentFactory&& other) = delete;
};