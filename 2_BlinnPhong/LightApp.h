#pragma once
#include <d3d11_4.h>
#include "../BaseEngine/GameApp.h"

#include <imgui.h>
#include <wrl/client.h>
#include <directxtk/SimpleMath.h>
#include <string>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <Psapi.h>

#pragma comment (lib, "d3d11.lib")

using namespace DirectX::SimpleMath;
using namespace DirectX;

struct Vertex
{
	Vector3 Pos;		// 정점 위치 정보.
	Vector3 Normal;
	Vector2 Tex;
};

struct ConstantBuffer
{
	Matrix mWorld;
	Matrix mView;
	Matrix mProjection;
	Matrix mNormalMatrix;

	Vector4 vLightDir;
	Vector4 vLightColor;
	Vector4 vOutputColor;
};

class LightApp : public GameApp
{
public:
	LightApp(HINSTANCE hInstance);
	~LightApp();

	Vector4 m_ClearColor = Vector4(0.45f, 0.55f, 0.60f, 1.00f);

	// Directional 라이트
	XMFLOAT4 m_LightColors = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);				// 라이트 색상
	XMFLOAT4 m_InitialLightDirs = XMFLOAT4(-0.577f, 0.577f, -0.577f, 1.0f);	// 초기 라이트 방향
	XMFLOAT4 m_CurrLightDirs = m_InitialLightDirs;							// 현재 라이트 방향
	XMFLOAT4 m_LightDirsEvaluated = {};										// 계산된 라이트 방향

	// VS 전달용 매트릭스
	Matrix m_World;			// 월드좌표계 변환행렬
	Matrix m_View;			// 카메라좌표계 변환행렬
	Matrix m_Projection;	// ndc좌표계 변환행렬

	// ImGui --------

	// 카메라
	float camFov = 45.0f;
	float camFarZ[2] = { 0.01f, 100.0f };

	// 오브젝트
	float cbRotation[3] = {};		// pitch, yaw, roll

	// 라이트
	float lightDir[3] = {
		m_InitialLightDirs.x,
		m_InitialLightDirs.y,
		m_InitialLightDirs.z
	};

	// End ---------

	// 렌더링 인터페이스
	ID3D11Device* m_pDevice = nullptr;						// 디바이스
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// 디바이스 컨텍스트
	IDXGISwapChain* m_pSwapChain = nullptr;				// 스왑체인
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// 렌더링 타겟뷰
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;  // 깊이값 처리를 위한 뎊스스텐실 뷰

	// 렌더링 정보
	ID3D11VertexShader* m_pVertexShader = nullptr;	// 정점 쉐이더
	ID3D11PixelShader* m_pPixelShader = nullptr;	// 픽셀 쉐이더
	ID3D11PixelShader* m_pPLightShader = nullptr;	// 라이트 쉐이더
	ID3D11InputLayout* m_pInputLayout = nullptr;	// 입력 레이아웃
	ID3D11Buffer* m_pVertexBuffer = nullptr;		// 버텍스 버퍼
	ID3D11Buffer* m_pIndexBuffer = nullptr;			// 인덱스 버퍼
	ID3D11Buffer* m_pConstantBuffer = nullptr;		// 상수 버퍼
	ID3D11ShaderResourceView* m_pTextureRV = nullptr;	// 텍스처 리소스 뷰.
	ID3D11SamplerState* m_pSamplerLinear = nullptr;		// 샘플러 상태.
	ID3D11RasterizerState* m_defaultRS = nullptr;		// 기본 RS
	
	// 스카이박스
	ID3D11VertexShader* m_pSkyVertexShader = nullptr;	// 스카이박스 정점 쉐이더
	ID3D11PixelShader* m_pSkyPixelShader = nullptr;	// 스카이박스 픽셀 쉐이더
	ID3D11InputLayout* m_pSkyInputLayout = nullptr;	// 스카이박스입력 레이아웃
	ID3D11RasterizerState* m_SkyboxRS = nullptr;	// 스카이박스 전용 RS
	ID3D11ShaderResourceView* m_pSkyTextureRV = nullptr;	// 스카이박스 텍스처 리소스 뷰.

	UINT m_VertexBufferStride = 0;					// 버텍스 하나의 크기
	UINT m_VertexBufferOffset = 0;					// 버텍스 버퍼의 오프셋

	int m_nIndices = 0;								// 인덱스 개수

	bool Initialize(UINT Width, UINT Height) override;
	void Update() override;
	void Render() override;

	bool InitD3D();
	void UninitD3D();

	bool InitScene();		// 쉐이더,버텍스,인덱스
	void UninitScene();

	//IMGUI
	bool InitImGUI();
	void UninitImGUI();
	void RenderGUI();

public:
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

