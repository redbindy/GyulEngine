#include "PlayScene.h"

#include "../Scene/Actor.h"
#include "../Components/TransformComponent.h"
#include "../Core/Input.h"

PlayScene::PlayScene(const std::wstring& name)
	: Scene(name)
{
}

void PlayScene::Initialize()
{

}

void PlayScene::Update()
{
	Scene::Update();
}

void PlayScene::LateUpdate()
{
	Scene::LateUpdate();
}

void PlayScene::Render() const
{
	Scene::Render();
}

void PlayScene::OnEnter()
{

}

void PlayScene::OnExit()
{

}
