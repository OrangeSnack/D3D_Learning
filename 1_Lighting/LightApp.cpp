#include "LightApp.h"
#include "../BaseEngine/Helper.h"
#include <d3dcompiler.h>


#pragma comment (lib, "d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

LightApp::LightApp(HINSTANCE hInstance) : GameApp(hInstance)
{
	
}

LightApp::~LightApp()
{
	UninitImGUI();
	UninitScene();
	UninitD3D();
}

bool LightApp::Initialize(UINT Width, UINT Height)
{
	__super::Initialize(Width, Height);

	if (!InitD3D())
		return false;

	if (!InitImGUI())
		return false;

	if (!InitScene())
		return false;

	return true;
}

void LightApp::Update()
{
	__super::Update();
}

void LightApp::Render()
{
	__super::Render();
}

bool LightApp::InitD3D()
{
	HRESULT hr;

	// 스왑체인 속성 설정 구조체 생성.
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 1;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = m_hWnd;	// 스왑체인 출력할 창 핸들 값.
	swapDesc.Windowed = true;		// 창 모드 여부 설정.
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 백버퍼(텍스처)의 가로/세로 크기 설정.
	swapDesc.BufferDesc.Width = m_ClientWidth;
	swapDesc.BufferDesc.Height = m_ClientHeight;
	// 화면 주사율 설정.
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	// 샘플링 관련 설정.
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	UINT creationFlags = 0;

	HR_T(D3D11CreateDevice(m_pDxgiAdapter, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, ));
	
}

void LightApp::UninitD3D()
{

}

bool LightApp::InitScene()
{

}

void LightApp::UninitScene()
{

}

bool LightApp::InitImGUI()
{

}

void LightApp::UninitImGUI()
{

}

void LightApp::RenderGUI()
{

}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK LightApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}
