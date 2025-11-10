#pragma once

class IEditorUIDrawable
{
public:
	virtual ~IEditorUIDrawable() = default;

	virtual void DrawEditorUI() = 0;
};