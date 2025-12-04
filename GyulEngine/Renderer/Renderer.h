#pragma once

#include "../Core/Assert.h"

class CameraComponent;

class Renderer final
{
public:

	// static
	static void CreateInstance();

	static Renderer& GetInstance()
	{
		ASSERT(spInstance != nullptr);

		return *spInstance;
	}

	static void Destroy()
	{
		delete spInstance;
	}

private:
	static Renderer* spInstance;

private:
	Renderer();
	virtual ~Renderer();

	CameraComponent* mpMainCamera;

private:
	Renderer(const Renderer& other) = delete;
	Renderer& operator=(const Renderer& other) = delete;
	Renderer(Renderer&& other) noexcept = delete;
	Renderer& operator=(Renderer&& other) noexcept = delete;
};