#pragma once
#include <windows.h>
#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "../BaseEngine/GameApp.h"
#include <vector>

using namespace DirectX::SimpleMath;
using namespace DirectX;

struct Vertex
{
	Vector3 Pos;		// 정점 위치 정보.
	Vector3 Normal;
};

struct ConstantBuffer
{
	Matrix mWorld;
	Matrix mView;
	Matrix mProjection;
	Matrix mNormalMatrix;

	Vector4 vLightDir[2];
	Vector4 vLightColor[2];
	Vector4 vOutputColor;
};

class LightApp : public GameApp
{
public:
	LightApp(HINSTANCE hInstance);
	~LightApp();

	Vector4 m_ClearColor = Vector4(0.45f, 0.55f, 0.60f, 1.00f);

	// ImGui 변수
	float camPos[3] = { 0.0f, 0.0f, -10.0f };
	float camFov = 90.0f;
	float camFarZ[2] = { 0.01f, 100.0f };
	float camPrevpos[3] = { camPos[0],camPos[1],camPos[2] };

	float cbPos[3][3] = {
		{0.0f, 0.0f, 0.0f},
		{ -5.0f, 0.0f, 0.0f },
		{ -5.0f, 0.0f, 0.0f } };
	float cbScale[3][3] = {
		{ 1.0f, 1.0f, 1.0f },
		{ 0.5f, 0.5f, 0.5f },
		{ 0.5f, 0.5f, 0.5f } };

	// DXGI
	IDXGIFactory2* m_pDxgiFactory;
	IDXGIAdapter2* m_pDxgiAdapter;

	// 렌더링 인터페이스
	ID3D11Device* m_pDevice = nullptr;						// 디바이스
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// 디바이스 컨텍스트
	IDXGISwapChain* m_pSwapChain = nullptr;				// 스왑체인
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// 렌더링 타겟뷰
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;  // 깊이값 처리를 위한 뎊스스텐실 뷰

	// 렌더링 정보
	ID3D11VertexShader* m_pVertexShader = nullptr;	// 정점 쉐이더
	ID3D11PixelShader* m_pPixelShader = nullptr;	// 픽셀 쉐이더
	ID3D11PixelShader* m_pPLightShader = nullptr;	// 픽셀 라이트 쉐이더
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
	ConstantBuffer cb;

	// 오브젝트 월드 매트릭스
	std::vector<Matrix> objWorlds;

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

public:
	void SetMatrix(Matrix mat, int index);
	void SetCBPos(Matrix mat, int index);
	void SetCamMat();

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

