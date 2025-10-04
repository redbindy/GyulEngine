#pragma once

template<class COM>
void SafeRelease(COM*& pCom)
{
	if (pCom != nullptr)
	{
		pCom->Release();
		pCom = nullptr;
	}
}