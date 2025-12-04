#pragma once

#include "../Scene/Scene.h"

class PlayScene final : public Scene
{
public:
	PlayScene(const std::wstring& name);
	virtual ~PlayScene() = default;

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() const override;

	virtual void OnEnter() override;
	virtual void OnExit() override;

private:
	PlayScene(const PlayScene& other) = delete;
	PlayScene& operator=(const PlayScene& other) = delete;
	PlayScene(PlayScene&& other) noexcept = delete;
	PlayScene& operator=(PlayScene&& other) noexcept = delete;
};