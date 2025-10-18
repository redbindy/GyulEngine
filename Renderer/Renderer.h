#pragma once

#include <unordered_map>
#include <string>

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "DebugHelper.h"
#include "Vertex.h"
#include "IUIDrawable.h"
#include "ETypeForRenderer.h"

class Mesh;
class Material;
class MeshComponent;
class CameraComponent;
class DebugSphere;

// 구조체에 대해서 초기화 경고 끄기
#pragma warning(push)
#pragma warning(disable : 26495)
struct CBFrame
{
	Vector3 cameraPos;
	float dummy;
	Matrix viewProj;
};
static_assert(sizeof(CBFrame) % 16 == 0);

struct CBWorldMatrix
{
	Matrix world;
};
static_assert(sizeof(CBWorldMatrix) % 16 == 0);
#pragma warning(pop)

class Renderer final : IUIDrawable
{
public:
	Renderer(const Renderer& other) = delete;
	Renderer& operator=(const Renderer& other) = delete;
	Renderer(Renderer&& other) = delete;
	Renderer& operator=(Renderer&& other) = delete;

	void UpdateCBFrame(const CBFrame& buffer);
	void UpdateCBWorldMatrix(const CBWorldMatrix& buffer);

	void BeginFrame();
	void RenderScene();
	void BeginUI() const;
	void DrawUI() override;
	void EndUI() const;
	void EndFrame();

	void OnResize(const int width, const int height);
	void SwitchWireframeMode();

	bool TryCreateVertexShaderAndInputLayout(const std::wstring& path, const EVertexType type);
	bool TryCreatePixelShader(const std::wstring& path);
	bool TryCreateTextureView(const std::wstring& path);

	ID3D11VertexShader* GetVertexShaderOrNull(const std::wstring& path) const;
	ID3D11PixelShader* GetPixelShaderOrNull(const std::wstring& path) const;

	Mesh* GetMeshOrNull(const std::wstring& path) const;
	Material* GetMaterialOrNull(const std::wstring& path) const;
	ID3D11ShaderResourceView* GetTextureViewOrNull(const std::wstring& path) const;
	ID3D11BlendState* GetBlendState() const;

	bool TryCreateBuffer(
		const EBufferType type,
		const void* const pData,
		const UINT byteSize,
		const UINT stride,
		ID3D11Buffer*& pOutBuffer
	) const;

	void AddMeshComponent(MeshComponent* pMeshComponent);
	void RemoveMeshComponent(MeshComponent* pMeshComponent);

	void AddCameraComponent(CameraComponent* const pCameraComponent);
	void RemoveCameraComponent(CameraComponent* const pCameraComponent);

	void SetDebugSphere(const Vector3 center, const float radius);

	bool TryGetMouseRay(const Vector2 mousePos, Ray& ray) const;

	static bool TryInitialize(const HWND hWnd);

	inline static Renderer& GetInstance()
	{
		ASSERT(spInstance != nullptr, "Please call Initialize first");

		return *spInstance;
	}

	inline static void Destroy()
	{
		if (spInstance == nullptr)
		{
			return;
		}

		delete spInstance;
	}

	inline HRESULT GetErrorCode() const
	{
		return mErrorCode;
	}

	inline int GetWidth() const
	{
		return mWidth;
	}

	inline int GetHeight() const
	{
		return mHeight;
	}

	inline UINT GetTargetFrameRate() const
	{
		return mTargetFrameRate;
	}

	inline ID3D11InputLayout* GetInputLayout(const EVertexType type) const
	{
		ASSERT(static_cast<uint8_t>(type) < static_cast<uint8_t>(EVertexType::COUNT));

		return mInputLayoutMap[static_cast<uint8_t>(type)];
	}

	inline ID3D11SamplerState* GetSamplerState(const ESamplerType type) const
	{
		ASSERT(static_cast<uint8_t>(type) < static_cast<uint8_t>(ESamplerType::COUNT));

		return mSamplerStateMap[static_cast<uint8_t>(type)];
	}

private:
	static Renderer* spInstance;

	ID3D11Device* mpDevice;
	ID3D11DeviceContext* mpDeviceContext;
	IDXGISwapChain* mpSwapChain;

	// ia
	ID3D11InputLayout* mInputLayoutMap[static_cast<int>(EVertexType::COUNT)];

	// vs
	std::unordered_map<std::wstring, ID3D11VertexShader*> mVertexShaderMap;

	// rs
	D3D11_VIEWPORT mViewport;
	ID3D11RasterizerState* mpWireframeState;
	bool mbWireframe;

	// ps
	std::unordered_map<std::wstring, ID3D11PixelShader*> mPixelShaderMap;

	// om
	ID3D11RenderTargetView* mpRenderTargetViewGPU;
	ID3D11DepthStencilView* mpDepthStencilViewGPU;
	ID3D11BlendState* mpBlendState;

	// resources
	ID3D11SamplerState* mSamplerStateMap[static_cast<int>(ESamplerType::COUNT)];

	std::unordered_map<std::wstring, Mesh*> mMeshMap;
	std::unordered_map<std::wstring, Material*> mMaterialMap;
	std::unordered_map<std::wstring, ID3D11ShaderResourceView*> mTextureViewMap;

	CBFrame mCBFrame;
	ID3D11Buffer* mpCBFrameGPU;

	CBWorldMatrix mCBWorldMatrix;
	ID3D11Buffer* mpCBWorldMatrixGPU;

	// properties
	HRESULT mErrorCode;

	int mWidth;
	int mHeight;

	UINT mTargetFrameRate;
	bool mbVSync;

	float mClearColor[4];

	std::vector<MeshComponent*> mMeshComponents;

	std::vector<CameraComponent*> mCameraComponents;
	int mSelectedNumber;

	DebugSphere* mpDebugSphere;
	bool mbDrawDebugSphere;

private:
	Renderer(const HWND hWnd);
	~Renderer();

	bool tryCompileShader(const TCHAR* const path, const EShaderType type, ID3DBlob*& pOutShaderBlob);
};