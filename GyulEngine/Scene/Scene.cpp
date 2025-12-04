#include "Scene.h"

#include "Actor.h"
#include "Layer.h"

Scene::Scene(const std::wstring& name)
	: Entity(name)
	, mpLayers()
{
	mpLayers.reserve(GetLayerTypeCount());

	for (int i = 0; i < GetLayerTypeCount(); ++i)
	{
		const TCHAR* const layerName = GetLayerTypeName(static_cast<ELayerType>(i));

		Layer* const pLayer = new Layer(layerName);

		mpLayers.push_back(pLayer);
	}
}

Scene::~Scene()
{
	for (Layer* const pLayer : mpLayers)
	{
		delete pLayer;
	}
}

void Scene::Initialize()
{

}

void Scene::Update()
{
	for (Layer* const pLayer : mpLayers)
	{
		pLayer->Update();
	}
}

void Scene::LateUpdate()
{
	for (Layer* const pLayer : mpLayers)
	{
		pLayer->LateUpdate();
	}
}

void Scene::Render() const
{
	for (Layer* const pLayer : mpLayers)
	{
		pLayer->Render();
	}
}

void Scene::OnEnter()
{

}

void Scene::OnExit()
{

}

void Scene::AddActor(const ELayerType type, Actor* const pActor)
{
	ASSERT(pActor != nullptr);

	mpLayers[static_cast<int>(type)]->AddActor(pActor);
}

void Scene::RemoveActor(Actor* const pActor)
{
	const ELayerType layerType = pActor->GetLayerType();

	mpLayers[static_cast<int>(layerType)]->RemoveActor(pActor);
}