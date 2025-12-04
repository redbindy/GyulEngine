#pragma once

#include "Actor.h"

class Floor final : public Actor
{
public:
	Floor();
	virtual ~Floor() = default;

	void Initialize() override;
	void Update() override;
	void LateUpdate() override;
	void Render() const override;

private:
	Floor(const Floor& other) = delete;
	Floor& operator=(const Floor& other) = delete;
	Floor(Floor&& other) noexcept = delete;
	Floor& operator=(Floor&& other) noexcept = delete;
};