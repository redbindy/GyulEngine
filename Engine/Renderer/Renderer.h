#pragma once

#include <vector>
#include <unordered_map>

#include <d3d11.h>
#include <dxgi1_2.h>

#include "Core/Assert.h"
#include "Core/MathHelper.h"
#include "PipelineStateType.h"
#include "UI/IEditorUIDrawable.h"

class Mesh;
class Material;
class CameraComponent;
class MeshComponent;

class Renderer final : public IEditorUIDrawable
{
public:
	enum ConstantBufferSlot
	{
		CB_FRAME_SLOT = 0,
		CB_WORLD_MATRIX_SLOT = 1,
		CB_MATERIAL_SLOT = 2
	};

	// 구조체에 대해서 초기화 경고 끄기
#pragma warning(push)
#pragma warning(disable : 26495)
	struct RenderCommand
	{
		Mesh* pMesh;
		Material* pMaterial;

		Matrix worldMatrix;
	};
#pragma warning(pop)

public:
	void BeginFrame() const;
	void EndFrame() const;

	void RenderScene(const std::string& sceneName);

	void BeginUIFrame() const;
	void EndUIFrame() const;

	void OnResize(const int width, const int height);
	void UpdateCBFrame(const Vector3& cameraPos, const Matrix& viewProj);

	void AddMeshComponentList(const std::string& sceneName);
	void RemoveMeshComponentList(const std::string& sceneName);

	void AddMeshComponent(const std::string& sceneName, MeshComponent* const pMeshComponent);
	void RemoveMeshComponent(const std::string& sceneName, MeshComponent* const pMeshComponent);

	virtual void DrawEditorUI() override;

	Vector3 Unproject(const Vector3 v) const;

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

	void SetEditorCameraComponent(CameraComponent* const pCameraComponent)
	{
		mpEditorCameraComponent = pCameraComponent;
		mpMainCameraComponent = pCameraComponent;
	}

	void SetMainCameraComponent(CameraComponent* const pCameraComponent)
	{
		mpMainCameraComponent = pCameraComponent;
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

	std::unordered_map<ERasterizerType, ID3D11RasterizerState*> mRasterizerStateMap;
	std::unordered_map<ESamplerType, ID3D11SamplerState*> mSamplerStateMap;
	std::unordered_map<EBlendStateType, ID3D11BlendState*> mBlendStateMap;
	std::unordered_map<EDepthStencilType, ID3D11DepthStencilState*> mDepthStencilStateMap;

	UINT mRefreshRate;

	bool mbVSync;
	bool mbMultiSampling;

	float mClearColor[4];

	std::vector<RenderCommand> mRenderCommandQueue;

	ID3D11Buffer* mpCBFrameGPU;
	ID3D11Buffer* mpCBWorldMatrixGPU;

	CameraComponent* mpEditorCameraComponent;
	CameraComponent* mpMainCameraComponent;
	std::unordered_map<std::string, std::vector<MeshComponent*>> mSceneComponents;

private:
	Renderer(
		ID3D11Device* const pDevice,
		ID3D11DeviceContext* const pDeviceContext,
		IDXGISwapChain1* const pSwapChain,
		const UINT refreshRate
	);
	~Renderer();

	ID3D11Buffer* createConstantBufferAlloc(const void* const pData, const UINT byteWidth);

private:
	Renderer(const Renderer& other) = delete;
	Renderer& operator=(const Renderer& other) = delete;
	Renderer(Renderer&& other) = delete;
	Renderer& operator=(Renderer&& other) = delete;
};