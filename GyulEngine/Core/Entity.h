#pragma once

#include <string>

class Entity
{
public:
	Entity(const std::wstring& name)
		: mName(name)
	{

	}

	virtual ~Entity() = default;

	const std::wstring& GetName() const
	{
		return mName;
	}

	void SetName(const std::wstring& name)
	{
		mName = name;
	}

private:
	std::wstring mName;

private:
	Entity(const Entity& other) = delete;
	Entity& operator=(const Entity& other) = delete;
	Entity(Entity&& other) noexcept = delete;
	Entity& operator=(Entity&& other) noexcept = delete;
};