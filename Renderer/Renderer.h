#pragma once

#include <unordered_map>
#include <string>
#include <queue>

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "DebugHelper.h"
#include "Vertex.h"
#include "IUIDrawable.h"
#include "ETypeForRenderer.h"
#include "CommonDefs.h"

#define GET_PADDING_SIZE_CBUFFER(type) (GET_PADDING_SIZE(16, type))

enum
{
	MAX_LIGHTS = 8
};

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
	uint8_t padding[GET_PADDING_SIZE_CBUFFER(Vector3)];

	Matrix viewProj;
};
static_assert(sizeof(CBFrame) % 16 == 0);

struct CBWorldMatrix
{
	Matrix world;
	Matrix invTransNormal;
};
static_assert(sizeof(CBWorldMatrix) % 16 == 0);

struct CBDirectionalLight
{
	Vector3 dir;
	uint8_t padding0[GET_PADDING_SIZE_CBUFFER(Vector3)];

	Vector3 strength;
	uint8_t padding1[GET_PADDING_SIZE_CBUFFER(Vector3)];
};
static_assert(sizeof(CBDirectionalLight) % 16 == 0);

struct Light
{
	Vector3 position;
	float fallOffStart;
	Vector3 strength;
	float fallOffEnd;
	Vector3 direction;
	float spotPower;
};
static_assert(sizeof(Light) % 16 == 0);

struct CBLight
{
	Light lights[MAX_LIGHTS];
};
static_assert(sizeof(CBLight) % 16 == 0);
#pragma warning(pop)

enum SlotNumber
{
	FRAME = 0,
	WORLD_MATRIX,
	DIRECTIONAL_LIGHT,
	MATERIAL,
	MATERIAL_ADDITIONAL,
	LIGHT_ADDITIONAL
};

class Renderer final : IUIDrawable
{
public:
	Renderer(const Renderer& other) = delete;
	Renderer& operator=(const Renderer& other) = delete;
	Renderer(Renderer&& other) = delete;
	Renderer& operator=(Renderer&& other) = delete;

	void UpdateCBFrame(const Vector3 cameraPos, const Matrix viewProj);
	void UpdateCBWorldMatrix(const Matrix& world);

	void BeginFrame();
	void RenderScene();
	void BeginUI() const;
	void DrawUI() override;
	void EndUI() const;
	void EndFrame();

	void OnResize(const int width, const int height);
	void SwitchWireframeMode();

	bool TryCreateVertexShaderAndInputLayout(const std::string& path, const EVertexType type);
	bool TryCreatePixelShader(const std::string& path);
	bool TryCreateTextureView(const std::string& path);

	ID3D11VertexShader* GetVertexShaderOrNull(const std::string& path) const;
	ID3D11PixelShader* GetPixelShaderOrNull(const std::string& path) const;

	Mesh* GetMeshOrNull(const std::string& path) const;
	Material* GetMaterialOrNull(const std::string& path) const;
	ID3D11ShaderResourceView* GetTextureViewOrNull(const std::string& path) const;
	ID3D11BlendState* GetBlendState() const;

	Light* AcquireLightOrNull();
	void ReturnLight(Light* const pLight);

	bool TryCreateBuffer(
		const EBufferType type,
		const void* const pData,
		const UINT byteSize,
		const UINT stride,
		ID3D11Buffer*& pOutBuffer
	) const;

	void AddMeshComponent(MeshComponent* const pMeshComponent);
	void RemoveMeshComponent(MeshComponent* const pMeshComponent);

	void AddCameraComponent(CameraComponent* const pCameraComponent);
	void RemoveCameraComponent(CameraComponent* const pCameraComponent);

	void SetDebugSphere(const Vector3 center, const float radius);

	Vector3 Unproject(const Vector3 v) const;

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

	inline ID3D11RasterizerState* GetWireframeState() const
	{
		return mpWireframeState;
	}

	inline ID3D11SamplerState* GetSamplerState(const ESamplerType type) const
	{
		ASSERT(static_cast<uint8_t>(type) < static_cast<uint8_t>(ESamplerType::COUNT));

		return mSamplerStateMap[static_cast<uint8_t>(type)];
	}

	inline ID3D11DepthStencilState* GetDepthStencilState() const
	{
		return mpDepthReadOnlyState;
	}

	inline const std::unordered_map<std::string, Mesh*>& GetMeshMap() const
	{
		return mMeshMap;
	}

	inline ID3D11RasterizerState* GetNonCullingState() const
	{
		return mpNonCullingState;
	}

private:
	static Renderer* spInstance;

	ID3D11Device* mpDevice;
	ID3D11DeviceContext* mpDeviceContext;
	IDXGISwapChain* mpSwapChain;

	// ia
	ID3D11InputLayout* mInputLayoutMap[static_cast<int>(EVertexType::COUNT)];

	// vs
	std::unordered_map<std::string, ID3D11VertexShader*> mVertexShaderMap;

	// rs
	D3D11_VIEWPORT mViewport;
	ID3D11RasterizerState* mpWireframeState;
	bool mbWireframe;

	ID3D11RasterizerState* mpNonCullingState;

	// ps
	std::unordered_map<std::string, ID3D11PixelShader*> mPixelShaderMap;

	// om
	ID3D11RenderTargetView* mpRenderTargetViewGPU;
	ID3D11DepthStencilView* mpDepthStencilViewGPU;
	ID3D11BlendState* mpBlendState;
	ID3D11DepthStencilState* mpDepthReadOnlyState;

	ID3D11RenderTargetView* mpMultiSampleRendereTargetViewGPU;
	ID3D11DepthStencilView* mpMultiSampleDepthStencilViewGPU;

	// resources
	ID3D11SamplerState* mSamplerStateMap[static_cast<int>(ESamplerType::COUNT)];

	std::unordered_map<std::string, Mesh*> mMeshMap;
	std::unordered_map<std::string, Material*> mMaterialMap;
	std::unordered_map<std::string, ID3D11ShaderResourceView*> mTextureViewMap;

	CBFrame mCBFrame;
	ID3D11Buffer* mpCBFrameGPU;

	ID3D11Buffer* mpCBWorldMatrixGPU;

	CBDirectionalLight mCBDirectionalLight;
	ID3D11Buffer* mpCBDirectionalLightGPU;
	bool mbOnDirectionalLight;

	CBLight mCBLight;
	std::queue<Light*> mLightPool;
	ID3D11Buffer* mpCBLightGPU;

	// properties
	HRESULT mErrorCode;

	int mWidth;
	int mHeight;

	UINT mTargetFrameRate;
	bool mbVSync;
	bool mbAntiAliasing;

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