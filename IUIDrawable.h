#pragma once

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

class IUIDrawable
{
public:
	virtual void DrawUI() = 0;
};