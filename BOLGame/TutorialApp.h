#pragma once

#include <d3d11_4.h>
#include "../BaseEngine/GameApp.h"


class TutorialApp : public GameApp
{
public:
	TutorialApp(HINSTANCE hInstance);
	~TutorialApp();

	// 렌더링 인터페이스
	ID3D11Device* m_pDevice = nullptr;						// 디바이스
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// 디바이스 컨텍스트
	IDXGISwapChain* m_pSwapChain = nullptr;				// 스왑체인
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// 렌더링 타겟뷰

	// 렌더링 정보
	ID3D11VertexShader* m_pVertexShader = nullptr;	// 정점 쉐이더
	ID3D11PixelShader* m_pPixelShader = nullptr;	// 픽셀 쉐이더
	ID3D11InputLayout* m_pInputLayout = nullptr;	// 입력 레이아웃
	ID3D11Buffer* m_pVertexBuffer = nullptr;		// 버텍스 버퍼
	UINT m_VertexBufferStride = 0;					// 버텍스 하나의 크기
	UINT m_VertexBufferOffset = 0;					// 버텍스 버퍼의 오프셋
	UINT m_VertexCount = 0;							// 버텍스 갯수

	virtual bool Initialize(UINT Width, UINT Height);
	virtual void Update();
	virtual void Render();

	bool InitD3D();
	void UninitD3D();

	bool InitScene();		// 쉐이더,버텍스,인덱스
	void UninitScene();
};

