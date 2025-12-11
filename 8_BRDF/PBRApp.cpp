#include "PBRApp.h"
#include "ResourceManager.h"
#include "RenderPipe.h"
#include "../BaseEngine/Helper.h"

using namespace Microsoft;
using namespace Microsoft::WRL;

Microsoft::WRL::ComPtr<ID3D11Device5> PBRApp::m_pDevice;
Camera* PBRApp::mainCamera = nullptr;

PBRApp::~PBRApp()
{
}

bool PBRApp::Initialize(UINT Width, UINT Height)
{
	__super::Initialize(Width, Height);

	// 카메라 인스턴스 등록
	mainCamera = &m_Camera;

	// 디바이스 생성
	ComPtr<ID3D11Device> device;
	D3D_FEATURE_LEVEL featureLevel;
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, 
		0, nullptr, 0, D3D11_SDK_VERSION,
		device.GetAddressOf(), &featureLevel, nullptr);

	HR_T(device.As(&m_pDevice));

	// 객체 초기화
	ResourceManager::GetInstance()->Initialize();
	RenderPipe::GetInstance()->Initialize(m_pDevice.Get(), &m_hWnd, m_ClientWidth, m_ClientHeight);
	GameTimer::m_Instance->Reset();
	GameTimer::m_Instance->Start();

	return true;
}

void PBRApp::Update()
{
	__super::Update();
	ResourceManager::GetInstance()->Update();
}

void PBRApp::Render()
{

}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK PBRApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}
