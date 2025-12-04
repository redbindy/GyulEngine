#include "ResourceManager.h"

enum
{
	DEFAULT_BUFFER_SIZE = 128
};

ResourceManager* ResourceManager::spInstance = nullptr;

ResourceManager::ResourceManager()
	: mResource()
{
	mResource.reserve(DEFAULT_BUFFER_SIZE);
}

ResourceManager::~ResourceManager()
{
	for (auto& pair : mResource)
	{
		delete pair.second;
	}
}

void ResourceManager::CreateInstance()
{
	ASSERT(spInstance == nullptr);

	spInstance = new ResourceManager();
}