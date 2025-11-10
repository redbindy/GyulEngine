#pragma once

#include <wrl.h>

using namespace Microsoft::WRL;

template<class COM>
void SafeRelease(COM*& pCom)
{
	if (pCom != nullptr)
	{
		pCom->Release();
		pCom = nullptr;
	}
}