#include "TutorialApp.h"
#include "windows.h"
#include <directxtk/SimpleMath.h>
#include <d3dcompiler.h>
#include "../BaseEngine/Helper.h"
#include <wrl/client.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX::SimpleMath;

// ���ؽ� ����
struct Vertex {
	Vector3 position;	// ��ġ����
	Vector4 color = { 1.0f, 0.0f, 0.0f, 1.0f };		// ��������

	Vertex(float x, float y, float z) : position(x, y, z) {}
	Vertex(Vector3 position) : position(position) {}

	Vertex(Vector3 position, Vector4 color)
		: position(position), color(color) {
	}
};

TutorialApp::TutorialApp(HINSTANCE hInstance)
	:GameApp(hInstance)
{
	
}

TutorialApp::~TutorialApp()
{
	UninitScene();
	UninitImGUI();
	UninitD3D();
}

bool TutorialApp::Initialize(UINT Width, UINT Height)
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

void TutorialApp::Update()
{
	__super::Update();
 	
	for (int i = 0; i < objWorlds.size(); i++) {
		SetMatrix(objWorlds[i], i);
	}
	SetCamMat();
}

void TutorialApp::Render()
{
	float color[4] = { 0.0f, 0.5f, 0.5f, 1.0f };

	// ���� Ÿ�� ����
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	// ȭ�� �ʱ�ȭ�ϱ�
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0); // �������� 1.0f�� �ʱ�ȭ.

	// Draw�迭 �Լ��� ȣ���ϱ����� ������ ���������ο� �ʼ� �������� ������ �ؾ��Ѵ�.	
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // ������ �̾ �׸� ��� ����.
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);
	m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

	// cb1 ����
	for (int i = 0; i < objWorlds.size(); i++) {
		SetCBPos(objWorlds[i], i);
		m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);
		m_pDeviceContext->DrawIndexed(m_nIndices, 0, 0);
	}

	RenderGUI();

	// Present the information rendered to the back buffer to the front buffer (the screen)
	m_pSwapChain->Present(0, 0);
}

bool TutorialApp::InitD3D()
{
	// �����.
	HRESULT hr = 0;

	// Create DXGI factory
	HR_T(CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)m_pDXGIFactory.GetAddressOf()));
	HR_T(m_pDXGIFactory->EnumAdapters(0, reinterpret_cast<IDXGIAdapter**>(m_pDXGIAdapter.GetAddressOf())));

	// ����ü�� �Ӽ� ���� ����ü ����.
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 1;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = m_hWnd;	// ����ü�� ����� â �ڵ� ��.
	swapDesc.Windowed = true;		// â ��� ���� ����.
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// �����(�ؽ�ó)�� ����/���� ũ�� ����.
	swapDesc.BufferDesc.Width = m_ClientWidth;
	swapDesc.BufferDesc.Height = m_ClientHeight;
	// ȭ�� �ֻ��� ����.
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	// ���ø� ���� ����.
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	UINT creationFlags = 0;
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// 1. ��ġ ����.   2.����ü�� ����. 3.��ġ ���ؽ�Ʈ ����.
	HR_T(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL,
		D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext));

	// 4. ����Ÿ�ٺ� ����.  (����۸� �̿��ϴ� ����Ÿ�ٺ�)	
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	HR_T(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture));
	HR_T(m_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pRenderTargetView));  // �ؽ�ó�� ���� ���� ����
	SAFE_RELEASE(pBackBufferTexture);	//�ܺ� ���� ī��Ʈ�� ���ҽ�Ų��.

	// ����Ʈ ����.	
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)m_ClientWidth;
	viewport.Height = (float)m_ClientHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_pDeviceContext->RSSetViewports(1, &viewport);

	//6. �X��&���ٽ� �� ����
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = m_ClientWidth;
	descDepth.Height = m_ClientHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTextureDepthStencil;
	HR_T(m_pDevice->CreateTexture2D(&descDepth, nullptr, pTextureDepthStencil.GetAddressOf()));

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	HR_T(m_pDevice->CreateDepthStencilView(pTextureDepthStencil.Get(), &descDSV, &m_pDepthStencilView));
	
	return true;
}

void TutorialApp::UninitD3D()
{
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pDevice);
}

bool TutorialApp::InitScene()
{
	HRESULT hr = 0; // �����.

	// ���ؽ� ���̴� ������
	ID3D10Blob* vertexShaderBuffer = nullptr;
	HR_T(CompileShaderFromFile(L"BasicVertexShader.hlsl", "main", "vs_4_0", &vertexShaderBuffer));

	// Render() ���� ���������ο� ���ε��� InputLayout ���� 	
	D3D11_INPUT_ELEMENT_DESC layout[] = // �Է� ���̾ƿ�.
	{   // SemanticName , SemanticIndex , Format , InputSlot , AlignedByteOffset , InputSlotClass , InstanceDataStepRate	
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	HR_T(m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pInputLayout));

	// Render() ���� ���������ο� ���ε���  ���ؽ� ���̴� ����
	HR_T(m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader));

	SAFE_RELEASE(vertexShaderBuffer);	// ���� ����.

	// Render() ���� ���������ο� ���ε��� ���ؽ� ���۹� ���� ���� �غ�
	Vertex vertices[] =
	{
		{ Vector3(-1.0f, 1.0f, -1.0f),	Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ Vector3(1.0f, 1.0f, -1.0f),	Vector4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ Vector3(1.0f, 1.0f, 1.0f),	Vector4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ Vector3(-1.0f, 1.0f, 1.0f),	Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ Vector3(-1.0f, -1.0f, -1.0f), Vector4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ Vector3(1.0f, -1.0f, -1.0f),	Vector4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ Vector3(1.0f, -1.0f, 1.0f),	Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ Vector3(-1.0f, -1.0f, 1.0f),	Vector4(0.0f, 0.0f, 0.0f, 1.0f) },
		{ Vector3(0.0f, 0.0f, 0.0f),	Vector4(0.5f, 0.5f, 0.5f, 1.0f) }
	};

	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = vertices;	// �迭 ������ �Ҵ�.
	HR_T(m_pDevice->CreateBuffer(&vbDesc, &vbData, &m_pVertexBuffer));
	m_VertexBufferStride = sizeof(Vertex);		// ���ؽ� ���� ũ��
	m_VertexBufferOffset = 0;	// ���ؽ� ���� �ּҿ��� ���� ������ �ּ�

	// Render() ���� ���������ο� ���ε��� �ε��� ���� ����
	//    0------1
	//	 /|     /|
	//  3------2 |
	//	| | 8  | |
	//	| 4----|-5
	//	|/     |/
	//	7------6
	WORD indices[] =
	{
		// �� �ﰢ��
		0, 1, 8,
		1, 2, 8,
		2, 3, 8,
		3, 0, 8,

		// �Ʒ� �ﰢ��
		4, 8, 5,
		5, 8, 6,
		6, 8, 7,
		7, 8, 4,

		// ���� �Ѳ�
		3, 1, 0,
		2, 1, 3,

		// �Ʒ��� �Ѳ�
		4, 5, 6,
		4, 6, 7
	};

	m_nIndices = ARRAYSIZE(indices);	// �ε��� ���� ����.
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);

	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = indices;
	HR_T(m_pDevice->CreateBuffer(&ibDesc, &ibData, &m_pIndexBuffer));

	// Render() ���� ���������ο� ���ε��� �ȼ� ���̴� ����
	ID3D10Blob* pixelShaderBuffer = nullptr;
	HR_T(CompileShaderFromFile(L"BasicPixelShader.hlsl", "main", "ps_4_0", &pixelShaderBuffer));
	HR_T(m_pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader));
	SAFE_RELEASE(pixelShaderBuffer);	// �ȼ� ���̴� ���� ���̻� �ʿ����.

	// �ܽ�źƮ ���� ����
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.ByteWidth = sizeof(ConstantBuffer);
	HR_T(m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pConstantBuffer));

	// ��Ʈ���� ����
	cb.mWorld = XMMatrixIdentity();

	// ������Ʈ ��Ʈ���� 3�� ����
	objWorlds.emplace_back(XMMatrixIdentity());
	objWorlds.emplace_back(XMMatrixIdentity());
	objWorlds.emplace_back(XMMatrixIdentity());

	// �� ���ʼ���
	XMVECTOR Eye = XMVectorSet(camPos[0], camPos[1], camPos[2], 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = XMMatrixLookAtLH(Eye, At, Up);

	// �������� ��Ʈ���� ���ʼ���
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 100.0f);

	return true;
}

void TutorialApp::UninitScene()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pConstantBuffer);
}

bool TutorialApp::InitImGUI()
{
	/*
		ImGui �ʱ�ȭ.
	*/
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();


	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(this->m_pDevice, this->m_pDeviceContext);

	return true;
}

void TutorialApp::UninitImGUI()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void TutorialApp::RenderGUI()
{
	//�Ʒ����ʹ� ImGUI
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Controller");


		ImGui::PushID(0);
		ImGui::Text("Camera");
		ImGui::DragFloat3("Position", camPos, 0.05f, -1000.0f, 1000.0f);

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
		ImGui::DragFloat("##MinFarZ", &camFarZ[0], 0.1f, 0.01f, camFarZ[1] - 10.0f);
		ImGui::SameLine();
		ImGui::DragFloat("FarZ", &camFarZ[1], 0.1f, camFarZ[0] + 10.0f, 500.0f);
		ImGui::PopItemWidth();
		ImGui::DragFloat("Fov", &camFov, 0.05f, 10.0f, 170.0f);
		if (ImGui::Button("Reset")) {
			camPos[0] = 0.0f;
			camPos[1] = 0.0f;
			camPos[2] = -10.0f;
			
			camFarZ[0] = 0.01f;
			camFarZ[1] = 100.0f;

			camFov = 90.0f;
		}
		ImGui::PopID();
		ImGui::NewLine();

		ImGui::PushID(1);
		ImGui::Text("Parent");
		ImGui::DragFloat3("Position", cbPos[0], 0.05f, -1000.0f, 1000.0f);
		ImGui::DragFloat3("Scale", cbScale[0], 0.05f, -1000.0f, 1000.0f);
		if (ImGui::Button("Reset")) {
			for (auto& val : cbPos[0])
				val = 0.0f;
			for (auto& val : cbScale[0])
				val = 1.0f;
		}
		ImGui::PopID();
		ImGui::NewLine();

		ImGui::PushID(2);
		ImGui::Text("Child1");
		ImGui::DragFloat3("Position", cbPos[1], 0.05f, -1000.0f, 1000.0f);
		ImGui::DragFloat3("Scale", cbScale[1], 0.05f, -1000.0f, 1000.0f);
		if (ImGui::Button("Reset")) {
			cbPos[1][0] = -5.0f;
			cbPos[1][1] = 0.0f;
			cbPos[1][2] = 0.0f;
			for (auto& val : cbScale[1])
				val = 0.5f;
		}
		ImGui::PopID();
		ImGui::NewLine();

		ImGui::PushID(3);
		ImGui::Text("Child2");
		ImGui::DragFloat3("Position", cbPos[2], 0.05f, -1000.0f, 1000.0f);
		ImGui::DragFloat3("Scale", cbScale[2], 0.05f, -1000.0f, 1000.0f);
		if (ImGui::Button("Reset")) {
			cbPos[2][0] = -5.0f;
			cbPos[2][1] = 0.0f;
			cbPos[2][2] = 0.0f;
			for (auto& val : cbScale[2])
				val = 0.5f;
		}
		ImGui::PopID();
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void TutorialApp::GetDisplayMemoryInfo(std::string& out)
{
	DXGI_ADAPTER_DESC desc;
	m_pDXGIAdapter->GetDesc(&desc);

	DXGI_QUERY_VIDEO_MEMORY_INFO local, nonLocal;
	m_pDXGIAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &local);
	m_pDXGIAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &nonLocal);

	out = std::to_string((desc.DedicatedVideoMemory + desc.SharedSystemMemory) / 1024 / 1024) + " MB\n";
	out += "Dedicated Video Memory : " + std::to_string(desc.DedicatedVideoMemory / 1024 / 1024) + " MB\n";
	out += "Shared System Memory : " + std::to_string(desc.SharedSystemMemory / 1024 / 1024) + " MB\n";

	out += "Local Video Memory: ";
	out += std::to_string(local.Budget / 1024 / 1024) + "MB" + " / " + std::to_string(local.CurrentUsage / 1024 / 1024) + " MB\n";
	out += "NonLocal Video Memory: ";
	out += std::to_string(nonLocal.Budget / 1024 / 1024) + "MB" + " / " + std::to_string(nonLocal.CurrentUsage / 1024 / 1024) + " MB";
}

void TutorialApp::GetVirtualMemoryInfo(std::string& out)
{
	HANDLE hProcess = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS_EX pmc;
	pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS_EX);
	GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	out = "PrivateUsage: " + std::to_string((pmc.PrivateUsage) / 1024 / 1024) + " MB\n";
	out += "WorkingSetSize: " + std::to_string((pmc.WorkingSetSize) / 1024 / 1024) + " MB\n";
	out += "PagefileUsage: " + std::to_string((pmc.PagefileUsage) / 1024 / 1024) + " MB";
}


void TutorialApp::SetMatrix(Matrix mat, int index)
{
	float totalTime = GameTimer::m_Instance->TotalTime();

	Matrix cb_Local = XMMatrixScaling(cbScale[index][0], cbScale[index][1], cbScale[index][2]) *
		XMMatrixRotationRollPitchYaw(0.0f, totalTime * 2.0f, 0.0f) *
		XMMatrixTranslation(cbPos[index][0], cbPos[index][1], cbPos[index][2]);

	Matrix cb_World = (index != 0) ? cb_Local * objWorlds[index - 1] : cb_Local;

	objWorlds[index] = cb_World;
}

void TutorialApp::SetCBPos(Matrix mat, int index)
{
	cb.mWorld = XMMatrixTranspose(mat);
	cb.mView = XMMatrixTranspose(m_View);
	cb.mProjection = XMMatrixTranspose(m_Projection);
}

void TutorialApp::SetCamMat()
{
	// �� ����
	/*float movePos[3] = { -(camPos[0] - camPrevpos[0]), -(camPos[1] - camPrevpos[1]), -(camPos[2] - camPrevpos[2]) };
	m_View *= XMMatrixTranslation(movePos[0], movePos[1], movePos[2]);
	camPrevpos[0] = camPos[0];
	camPrevpos[1] = camPos[1];
	camPrevpos[2] = camPos[2];*/

	XMVECTOR eyePos = XMVectorSet(camPos[0], camPos[1], camPos[2], 0.0f);
	XMVECTOR lookAt = XMVectorAdd(eyePos, XMVectorSet(0, 0, 1, 0));
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = XMMatrixLookAtLH(eyePos, lookAt, up);


	// �������� ��Ʈ���� ����
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2 * (camFov/90.0f), m_ClientWidth / (FLOAT)m_ClientHeight, camFarZ[0], camFarZ[1]);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK TutorialApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}