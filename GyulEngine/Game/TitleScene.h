#pragma once

#include "../Scene/Scene.h"

class TitleScene final : public Scene
{
public:
	TitleScene(const std::wstring name);
	virtual ~TitleScene() = default;

	void Initialize() override;
	void Update() override;
	void LateUpdate() override;
	void Render() const override;

	void OnEnter() override;
	void OnExit() override;

private:
};