#pragma once

#include <unordered_map>

#include "../Core/Assert.h"
#include "Resource.h"

class ResourceManager final
{
public:
	template<typename T>
	T* Find(const std::wstring& name)
	{
		auto iter = mResource.find(name);
		if (iter == mResource.end())
		{
			return nullptr;
		}

		return dynamic_cast<T*>(iter->second);
	}

	template<typename T>
	T* Load(const std::wstring& key, const std::wstring& path)
	{
		T* pResource = Find<T>(key);
		if (pResource != nullptr)
		{
			return pResource;
		}

		T* const pNewResource = new T(key);
		if (FAILED(pNewResource->Load(path)))
		{
			ASSERT(false);
		}
		pNewResource->SetPath(path);

		mResource.insert({ key, pNewResource });

		return pNewResource;
	}

	void Insert(const std::wstring& key, Resource* const pResource)
	{
		if (key.empty() || pResource == nullptr)
		{
			return;
		}

		mResource.insert({ key, pResource });
	}

	// static
	static void CreateInstance();

	static ResourceManager& GetInstance()
	{
		ASSERT(spInstance != nullptr);

		return *spInstance;
	}

	static void Destroy()
	{
		delete spInstance;
	}

private:
	static ResourceManager* spInstance;

	std::unordered_map<std::wstring, Resource*> mResource;

private:
	ResourceManager();
	virtual ~ResourceManager();

private:
	ResourceManager(const ResourceManager& other) = delete;
	ResourceManager& operator=(const ResourceManager& other) = delete;
	ResourceManager(ResourceManager&& other) noexcept = delete;
	ResourceManager& operator=(ResourceManager&& other) noexcept = delete;
};