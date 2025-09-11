#pragma once

#include <d3d11_4.h>
#include "../BaseEngine/GameApp.h"

#include <dxgi1_4.h>
#include <imgui.h>
#include <wrl/client.h>
#include <directxtk/SimpleMath.h>
#include <string>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <Psapi.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment(lib,"dxgi.lib")

struct ConstantBuffer
{
	Matrix mWorld;
	Matrix mView;
	Matrix mProjection;
};

class TutorialApp : public GameApp
{
public:
	TutorialApp(HINSTANCE hInstance);
	~TutorialApp();

	//IMGUI
	Microsoft::WRL::ComPtr<IDXGIFactory4> m_pDXGIFactory;		// DXGI팩토리
	Microsoft::WRL::ComPtr<IDXGIAdapter3> m_pDXGIAdapter;		// 비디오카드 정보에 접근 가능한 인터페이스

	Vector4 m_ClearColor = Vector4(0.45f, 0.55f, 0.60f, 1.00f);

	float camPos[3] = { 0.0f, 0.0f, -10.0f };
	float camFov = 90.0f;
	float camFarZ[2] = { 0.01f, 100.0f };
	float camPrevpos[3] = { camPos[0],camPos[1],camPos[2] };

	float cb1Pos[3] = { 0.0f, 0.0f, 0.0f };
	float cb1Scale[3] = { 1.0f, 1.0f, 1.0f };

	float cb2Pos[3] = { -5.0f, 0.0f, 0.0f };
	float cb2Scale[3] = { 0.5f, 0.5f, 0.5f };

	float cb3Pos[3] = { -5.0f, 0.0f, 0.0f };
	float cb3Scale[3] = { 0.5f, 0.5f, 0.5f };

	// 렌더링 인터페이스
	ID3D11Device* m_pDevice = nullptr;						// 디바이스
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// 디바이스 컨텍스트
	IDXGISwapChain* m_pSwapChain = nullptr;				// 스왑체인
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// 렌더링 타겟뷰
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;  // 깊이값 처리를 위한 뎊스스텐실 뷰

	// 렌더링 정보
	ID3D11VertexShader* m_pVertexShader = nullptr;	// 정점 쉐이더
	ID3D11PixelShader* m_pPixelShader = nullptr;	// 픽셀 쉐이더
	ID3D11InputLayout* m_pInputLayout = nullptr;	// 입력 레이아웃
	ID3D11Buffer* m_pVertexBuffer = nullptr;		// 버텍스 버퍼
	UINT m_VertexBufferStride = 0;					// 버텍스 하나의 크기
	UINT m_VertexBufferOffset = 0;					// 버텍스 버퍼의 오프셋

	ID3D11Buffer* m_pIndexBuffer = nullptr;			// 인덱스 버퍼
	ID3D11Buffer* m_pConstantBuffer = nullptr;		// 상수 버퍼
	int m_nIndices = 0;								// 인덱스 개수

	// 쉐이더 전달용 매트릭스
	Matrix m_View;			// 카메라좌표계 행렬
	Matrix m_Projection;	// ndc좌표계 행렬

	// 버퍼
	ConstantBuffer cb1;
	ConstantBuffer cb2;
	ConstantBuffer cb3;

	virtual bool Initialize(UINT Width, UINT Height);
	virtual void Update();
	virtual void Render();

	bool InitD3D();
	void UninitD3D();

	bool InitScene();		// 쉐이더,버텍스,인덱스
	void UninitScene();

	//IMGUI
	bool InitImGUI();
	void UninitImGUI();
	void RenderGUI();

	void GetDisplayMemoryInfo(std::string& out);
	void GetVirtualMemoryInfo(std::string& out);

public:
	void SetCBPos();
	void SetCamMat();

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

