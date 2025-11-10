#include "Actor.h"

#include <cstring>
#include <algorithm>

#include "Core/Assert.h"
#include "./Components/Component.h"
#include "./Components/ComponentFactory.h"

Actor::Actor(Scene* const pScene, const char* const label)
	: mLabel{ '\0', }
	, mTempBuffer{ '\0', }
	, mbRenaming(false)
	, mbAlive(true)
	, mpScene(pScene)
	, mPosition(0.0f, 0.0f, 0.0f)
	, mScale(1.0f, 1.0f, 1.0f)
	, mRotation(Quaternion::Identity)
	, mComponents()
	, mPendingComponents()
{
	ASSERT(label != nullptr);
	ASSERT(strlen(label) < MAX_LABEL_LENGTH);

	strcpy(mLabel, label);
	mLabel[MAX_LABEL_LENGTH - 1] = '\0';
}

Actor::~Actor()
{
	// Delete all components
	for (Component* pComponent : mComponents)
	{
		delete pComponent;
	}

	for (Component* pComponent : mPendingComponents)
	{
		delete pComponent;
	}
}

Actor::Actor(const Actor& other)
	: mLabel{ '\0', }
	, mTempBuffer{ '\0', }
	, mbRenaming(other.mbRenaming)
	, mbAlive(other.mbAlive)
	, mPosition(other.mPosition)
	, mScale(other.mScale)
	, mRotation(other.mRotation)
	, mComponents()
	, mPendingComponents()
{
	strcpy(mLabel, other.mLabel);
	mLabel[MAX_LABEL_LENGTH - 1] = '\0';

	ComponentFactory& componentFactory = ComponentFactory::GetInstance();
	for (Component* pComponent : other.mComponents)
	{
		Component* pNewComponent = componentFactory.CreateComponentAlloc(pComponent->GetLabel(), this);

		*pNewComponent = *pComponent;
		mComponents.push_back(pNewComponent);
	}
}

Actor& Actor::operator=(const Actor& other)
{
	if (this != &other)
	{
		strcpy(mLabel, other.mLabel);
		mLabel[MAX_LABEL_LENGTH - 1] = '\0';

		mbRenaming = other.mbRenaming;
		mbAlive = other.mbAlive;
		mPosition = other.mPosition;
		mScale = other.mScale;
		mRotation = other.mRotation;

		for (Component* pComponent : mComponents)
		{
			delete pComponent;
		}
		mComponents.clear();

		for (Component* pComponent : other.mComponents)
		{
			Component* pNewComponent = ComponentFactory::GetInstance().CreateComponentAlloc(pComponent->GetLabel(), this);

			*pNewComponent = *pComponent;
			mComponents.push_back(pNewComponent);
		}
	}

	return *this;
}

void Actor::Update(const float deltaTime)
{
	ASSERT(deltaTime > 0.f);

	if (!mbAlive)
	{
		return;
	}

	for (Component* pComponent : mComponents)
	{
		pComponent->Update(deltaTime);
	}
}

Matrix Actor::GetTransform() const
{
	const Matrix scale = Matrix::CreateScale(mScale);
	const Matrix rotation = Matrix::CreateFromQuaternion(mRotation);
	const Matrix translation = Matrix::CreateTranslation(mPosition);

	return scale * rotation * translation;
}

void Actor::AddComponent(Component* const pComponent)
{
	ASSERT(pComponent != nullptr);

#define VECTOR_ITER std::vector<Component*>::iterator

	VECTOR_ITER iter = mComponents.begin();
	while (iter != mComponents.end())
	{
		if (pComponent->GetUpdateOrder() < (*iter)->GetUpdateOrder())
		{
			break;
		}

		++iter;
	}

	mComponents.insert(iter, pComponent);

#undef VECTOR_ITER
}

void Actor::RemoveComponent(Component* const pComponent)
{
	ASSERT(pComponent != nullptr);

#define VECTOR_ITER std::vector<Component*>::iterator

	VECTOR_ITER iter = std::find(mComponents.begin(), mComponents.end(), pComponent);

	if (iter != mComponents.end())
	{
		delete* iter;

		mComponents.erase(iter);

		return;
	}

	iter = std::find(mPendingComponents.begin(), mPendingComponents.end(), pComponent);
	if (iter != mPendingComponents.end())
	{
		delete* iter;

		mPendingComponents.erase(iter);
	}

#undef VECTOR_ITER
}
