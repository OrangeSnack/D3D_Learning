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
	ResourceManager::Initialize();
	RenderPipe::Initialize();

	return true;
}

void PBRApp::Update()
{
	ResourceManager::instance->Update();
}

void PBRApp::Render()
{

}
