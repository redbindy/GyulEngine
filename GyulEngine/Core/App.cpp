#include "App.h"

#include "Input.h"
#include "Time.h"
#include "../Scene/SceneManager.h"
#include "../Resources/ResourceManager.h"
#include "../Scene/CollisionManager.h"
#include "../UI/UIManager.h"

App::App()
	: mhWnd(nullptr)
{

}

App::~App()
{
	UIManager::Destroy();
	CollisionManager::Destroy();
	SceneManager::Destroy();
	ResourceManager::Destroy();
	Time::Destroy();
	Input::Destroy();
}

void App::Initialize(const HWND hWnd)
{
	ASSERT(hWnd != nullptr);

	mhWnd = hWnd;

	Time::CreateInstance();
	Input::CreateInstance();
	ResourceManager::CreateInstance();
	SceneManager::CreateInstance();
	CollisionManager::CreateInstance();
	UIManager::CreateInstance();
}

void App::Run()
{
	Input& input = Input::GetInstance();
	Time& time = Time::GetInstance();
	SceneManager& sceneManager = SceneManager::GetInstance();
	CollisionManager& collisionManager = CollisionManager::GetInstance();
	UIManager& uiManager = UIManager::GetInstance();

	// update
	{
		input.Update();
		time.Update();

		collisionManager.Update();
		uiManager.Update();

		sceneManager.Update();
	}

	// last update
	{
		collisionManager.LateUpdate();
		uiManager.LateUpdate();

		sceneManager.LateUpdate();
	}

	// render
	{
		collisionManager.Render();
		uiManager.Render();

		sceneManager.Render();
	}
}
