#pragma once

#include <winerror.h>

#include "../Core/Entity.h"
#include "EResourceType.h"

class Resource : public Entity
{
public:
	Resource(const std::wstring& name);
	virtual ~Resource() = default;

	virtual HRESULT Load(const std::wstring& path) = 0;

	virtual EResourceType GetResourceType() const = 0;

	const std::wstring& GetPath() const
	{
		return mPath;
	}

	void SetPath(const std::wstring& path)
	{
		mPath = path;
	}

private:
	std::wstring mPath;

private:
	Resource(const Resource& other) = delete;
	Resource& operator=(const Resource& other) = delete;
	Resource(Resource&& other) noexcept = delete;
	Resource& operator=(Resource&& other) noexcept = delete;
};