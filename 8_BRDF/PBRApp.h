#pragma once
#include "../BaseEngine/GameApp.h"

#include <imgui.h>
#include <wrl/client.h>
#include <directxtk/SimpleMath.h>
#include <string>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <Psapi.h>
#include <memory>

#include "GameObject.h"
#include "framework.h"
#include "RenderStruct.h"

using namespace Microsoft::WRL;

class PBRApp : public GameApp
{
public:
	PBRApp(HINSTANCE hInstance) : GameApp(hInstance) {};
	~PBRApp();

	// 버퍼 기본색상
	Vector4 m_ClearColor = Vector4(0.45f, 0.55f, 0.60f, 1.00f);

	// VS 전달용 매트릭스 (카메라가 여기있기 때문)
	CamBuffer m_camMat;

	// 기본 렌더 인터페이스
	ComPtr<ID3D11Device> m_pDevice;						// 디바이스
	ComPtr<ID3D11DeviceContext> m_pDeviceContext;		// 디바이스 컨텍스트
	ComPtr<IDXGISwapChain> m_pSwapChain;				// 스왑체인
	
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView = nullptr;		// 렌더링 타겟뷰
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView = nullptr;		// 깊이값 처리를 위한 뎊스스텐실 뷰

	ComPtr<ID3D11SamplerState> m_pDafaultSamplerLinear = nullptr;		// 샘플러 상태.
	ComPtr<ID3D11RasterizerState> m_pDefaultRS = nullptr;				// 기본 RS

	ID3D11BlendState* m_pDefaultBS = nullptr;					// 기본블랜드 스테이트
	D3D11_VIEWPORT m_defaultViewport;							// 기본 뷰포트
};

