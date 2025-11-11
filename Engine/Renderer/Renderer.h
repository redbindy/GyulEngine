#pragma once

#include <vector>

#include <d3d11.h>
#include <dxgi1_2.h>

#include "Core/Assert.h"
#include "Core/MathHelper.h"
#include "PipelineStateType.h"
#include "UI/IEditorUIDrawable.h"

class Mesh;
class Material;

class Renderer final : public IEditorUIDrawable
{
public:
	enum ConstantBufferSlot
	{
		CB_FRAME_SLOT = 0,
		CB_WORLD_MATRIX_SLOT = 1,
		CB_MATERIAL_SLOT = 2
	};

	struct RenderCommand
	{
		Mesh* pMesh;
		Material* pMaterial;

		Matrix worldMatrix;
	};

public:
	void BeginFrame() const;
	void EndFrame() const;

	void Render();

	void BeginUIFrame() const;
	void EndUIFrame() const;

	void OnResize(const int width, const int height);
	void UpdateCBFrame(const Vector3& cameraPos, const Matrix& viewProj);

	virtual void DrawEditorUI() override;

	UINT GetRefreshRate() const
	{
		return mRefreshRate;
	}

	inline ID3D11RasterizerState* GetRasterizerState(const ERasterizerType type)
	{
		return mRasterizerStateMap[type];
	}

	inline ID3D11SamplerState* GetSamplerState(const ESamplerType type)
	{
		return mSamplerStateMap[type];
	}

	inline ID3D11BlendState* GetBlendState(const EBlendStateType type)
	{
		return mBlendStateMap[type];
	}

	inline ID3D11DepthStencilState* GetDepthStencilState(const EDepthStencilType type)
	{
		return mDepthStencilStateMap[type];
	}

	inline void EnqueueRenderCommand(const RenderCommand& command)
	{
		ASSERT(command.pMesh != nullptr);
		ASSERT(command.pMaterial != nullptr);

		mRenderCommandQueue.push_back(command);
	}

	const D3D11_VIEWPORT& GetViewport() const
	{
		return mViewport;
	}

	// static
	static bool TryInitialize(const HWND hWnd);

	static Renderer& GetInstance()
	{
		ASSERT(spInstance != nullptr);

		return *spInstance;
	}

	static void Destroy()
	{
		delete spInstance;
	}

private:
	static Renderer* spInstance;

	ID3D11Device* mpDevice;
	ID3D11DeviceContext* mpDeviceContext;
	IDXGISwapChain1* mpSwapChain;

	ID3D11RenderTargetView* mpBackBufferViewGPU;

	ID3D11RenderTargetView* mpMultiSampleRenderTargetViewGPU;
	ID3D11DepthStencilView* mpMultiSampleDepthStencilViewGPU;

	ID3D11RenderTargetView* mpHDRRenderTargetViewGPU;
	ID3D11ShaderResourceView* mpHDRResourceViewGPU;

	ID3D11RenderTargetView* mpSDRRenderTargetViewGPU;
	ID3D11DepthStencilView* mpCommonDepthStencilViewGPU;

	D3D11_VIEWPORT mViewport;

	UINT mRefreshRate;

	bool mbVSync;
	bool mbMultiSampling;

	float mClearColor[4];

	std::vector<RenderCommand> mRenderCommandQueue;

	std::unordered_map<ERasterizerType, ID3D11RasterizerState*> mRasterizerStateMap;
	std::unordered_map<ESamplerType, ID3D11SamplerState*> mSamplerStateMap;
	std::unordered_map<EBlendStateType, ID3D11BlendState*> mBlendStateMap;
	std::unordered_map<EDepthStencilType, ID3D11DepthStencilState*> mDepthStencilStateMap;

	ID3D11Buffer* mpCBFrameGPU;
	ID3D11Buffer* mpCBWorldMatrixGPU;

private:
	Renderer(
		ID3D11Device* const pDevice,
		ID3D11DeviceContext* const pDeviceContext,
		IDXGISwapChain1* const pSwapChain,
		UINT refreshRate
	);
	~Renderer();

	ID3D11Buffer* createConstantBufferAlloc(const void* const pData, const UINT byteWidth);

private:
	Renderer(const Renderer& other) = delete;
	Renderer& operator=(const Renderer& other) = delete;
	Renderer(Renderer&& other) = delete;
	Renderer& operator=(Renderer&& other) = delete;
};