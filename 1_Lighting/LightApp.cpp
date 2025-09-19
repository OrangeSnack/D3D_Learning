#include "LightApp.h"
#include "../BaseEngine/Helper.h"
#include <d3dcompiler.h>
#include <Directxtk/DDSTextureLoader.h>

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

	//float t = GameTimer::m_Instance->TotalTime();
	m_World = XMMatrixRotationRollPitchYaw(cbRotation[0], cbRotation[1], cbRotation[2]);

	// 라이트 방향
	m_CurrLightDirs.x = lightDir[0];
	m_CurrLightDirs.y = lightDir[1];
	m_CurrLightDirs.z = lightDir[2];

	m_LightDirsEvaluated = m_CurrLightDirs;

	// 프로젝션 매트릭스 설정
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4 * (camFov / 45.0f), m_ClientWidth / (FLOAT)m_ClientHeight, camFarZ[0], camFarZ[1]);
	m_Camera.GetViewMatrix(m_View);
}

void LightApp::Render()
{
	float color[4] = { 0.0f, 0.5f, 0.5f, 1.0f };

	// Update matrix variables and lighting variables
	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(m_World);
	cb1.mView = XMMatrixTranspose(m_View);
	cb1.mProjection = XMMatrixTranspose(m_Projection);
	cb1.vLightDir = m_LightDirsEvaluated;
	cb1.vLightColor = m_LightColors;
	cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
	m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb1, 0, 0);

	// Clear 
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Render Setting
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);
	m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	// ----- 스카이박스 렌더링 -----

	// 스카이박스용 렌더타겟 설정
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

	// 스카이박스 렌더링
	m_pDeviceContext->IASetInputLayout(m_pSkyInputLayout);
	m_pDeviceContext->VSSetShader(m_pSkyVertexShader, nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pSkyPixelShader, nullptr, 0);
	m_pDeviceContext->PSSetShaderResources(0, 1, &m_pSkyTextureRV);
	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	m_pDeviceContext->RSSetState(m_SkyboxRS);

	m_pDeviceContext->DrawIndexed(m_nIndices, 0, 0);

	// ----- 큐브 렌더링 -----

	// 렌더타겟 설정
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	// Render Cube
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTextureRV);
	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	m_pDeviceContext->RSSetState(m_defaultRS);

	m_pDeviceContext->DrawIndexed(m_nIndices, 0, 0);


	// Render light	
	XMMATRIX mLight = XMMatrixTranslationFromVector(5.0f *  XMVector3Normalize(XMLoadFloat4(&m_LightDirsEvaluated)));
	XMMATRIX mLightScale = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	mLight = mLightScale * mLight;

	// Update the world variable to reflect the current light
	cb1.mWorld = XMMatrixTranspose(mLight);
	cb1.vOutputColor = m_LightColors;
	m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb1, 0, 0);

	m_pDeviceContext->PSSetShader(m_pPLightShader, nullptr, 0);
	m_pDeviceContext->DrawIndexed(m_nIndices, 0, 0);

	// GUI Render
	RenderGUI();

	// Present our back buffer to our front buffer
	m_pSwapChain->Present(0, 0);
}

bool LightApp::InitD3D()
{
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

	// 장치, 스왑체인, 컨텍스트 생성
	HR_T(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL,
		D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext));
	
	// 렌더타겟 생성
	ID3D11Texture2D* pBackBuffer = nullptr;
	HR_T(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer));
	HR_T(m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView));
	SAFE_RELEASE(pBackBuffer);

	// 뷰포트
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(m_ClientWidth);
	viewport.Height = static_cast<float>(m_ClientHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_pDeviceContext->RSSetViewports(1, &viewport);

	// 뎊스 생성
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

	ID3D11Texture2D* textureDepthStencil = nullptr;
	HR_T(m_pDevice->CreateTexture2D(&descDepth, nullptr, &textureDepthStencil));

	// 스탠실 뷰 생성
	D3D11_DEPTH_STENCIL_VIEW_DESC dsv = {};
	dsv.Format = descDepth.Format;
	dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsv.Texture2D.MipSlice = 0;
	HR_T(m_pDevice->CreateDepthStencilView(textureDepthStencil, &dsv, &m_pDepthStencilView));
	SAFE_RELEASE(textureDepthStencil);

	return true;
}

void LightApp::UninitD3D()
{
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pRenderTargetView);
}

bool LightApp::InitScene()
{
	// 버텍스 쉐이더 컴파일
	ID3D10Blob* vertexShader = nullptr;
	HR_T(CompileShaderFromFile(L"BasicVertexShader.hlsl", "main", "vs_4_0", &vertexShader));
	HR_T(m_pDevice->CreateVertexShader(vertexShader->GetBufferPointer(),
		vertexShader->GetBufferSize(), NULL, &m_pVertexShader));

	// 인풋 레이아웃 생성
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	HR_T(m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShader->GetBufferPointer()
		, vertexShader->GetBufferSize(), &m_pInputLayout));
	SAFE_RELEASE(vertexShader);

	// 스카이박스 레이아웃 생성
	HR_T(CompileShaderFromFile(L"SkyBoxVertexShader.hlsl", "main", "vs_4_0", &vertexShader));
	HR_T(m_pDevice->CreateVertexShader(vertexShader->GetBufferPointer(), vertexShader->GetBufferSize(),
		NULL, &m_pSkyVertexShader));

	D3D11_INPUT_ELEMENT_DESC sky_layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	HR_T(m_pDevice->CreateInputLayout(sky_layout, ARRAYSIZE(sky_layout), vertexShader->GetBufferPointer(),
		vertexShader->GetBufferSize(), &m_pSkyInputLayout));
	SAFE_RELEASE(vertexShader);

	// 픽셀 쉐이더 컴파일
	ID3D10Blob* pixelShader = nullptr;
	HR_T(CompileShaderFromFile(L"BasicPixelShader.hlsl", "main", "ps_4_0", &pixelShader));
	HR_T(m_pDevice->CreatePixelShader(pixelShader->GetBufferPointer(),
		pixelShader->GetBufferSize(), NULL, &m_pPixelShader));
	SAFE_RELEASE(pixelShader);

	HR_T(CompileShaderFromFile(L"SolidPixelShader.hlsl", "main", "ps_4_0", &pixelShader));
	HR_T(m_pDevice->CreatePixelShader(pixelShader->GetBufferPointer(),
		pixelShader->GetBufferSize(), NULL, &m_pPLightShader));
	SAFE_RELEASE(pixelShader);

	HR_T(CompileShaderFromFile(L"SkyBoxPixelShader.hlsl", "main", "ps_4_0", &pixelShader));
	HR_T(m_pDevice->CreatePixelShader(pixelShader->GetBufferPointer(),
		pixelShader->GetBufferSize(), NULL, &m_pSkyPixelShader));
	SAFE_RELEASE(pixelShader);

	// 버텍스 정보
	Vertex vertices[] =
	{
		{ Vector3(-1.0f, 1.0f, -1.0f),	Vector3(0.0f, 1.0f, 0.0f),	Vector2(1.0f, 0.0f) },// Normal Y +	 
		{ Vector3(1.0f, 1.0f, -1.0f),	Vector3(0.0f, 1.0f, 0.0f),	Vector2(0.0f, 0.0f) },
		{ Vector3(1.0f, 1.0f, 1.0f),	Vector3(0.0f, 1.0f, 0.0f),	Vector2(0.0f, 1.0f) },
		{ Vector3(-1.0f, 1.0f, 1.0f),	Vector3(0.0f, 1.0f, 0.0f),	Vector2(1.0f, 1.0f) },

		{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f),	Vector2(0.0f, 0.0f) },// Normal Y -		
		{ Vector3(1.0f, -1.0f, -1.0f),	Vector3(0.0f, -1.0f, 0.0f),	Vector2(1.0f, 0.0f) },
		{ Vector3(1.0f, -1.0f, 1.0f),	Vector3(0.0f, -1.0f, 0.0f),	Vector2(1.0f, 1.0f) },
		{ Vector3(-1.0f, -1.0f, 1.0f),	Vector3(0.0f, -1.0f, 0.0f),	Vector2(0.0f, 1.0f) },

		{ Vector3(-1.0f, -1.0f, 1.0f),	Vector3(-1.0f, 0.0f, 0.0f),	Vector2(0.0f, 1.0f) },//	Normal X -
		{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(-1.0f, 0.0f, 0.0f),	Vector2(1.0f, 1.0f) },
		{ Vector3(-1.0f, 1.0f, -1.0f),	Vector3(-1.0f, 0.0f, 0.0f),	Vector2(1.0f, 0.0f) },
		{ Vector3(-1.0f, 1.0f, 1.0f),	Vector3(-1.0f, 0.0f, 0.0f),	Vector2(0.0f, 0.0f) },

		{ Vector3(1.0f, -1.0f, 1.0f),	Vector3(1.0f, 0.0f, 0.0f),	Vector2(1.0f, 1.0f) },// Normal X +
		{ Vector3(1.0f, -1.0f, -1.0f),	Vector3(1.0f, 0.0f, 0.0f),	Vector2(0.0f, 1.0f) },
		{ Vector3(1.0f, 1.0f, -1.0f),	Vector3(1.0f, 0.0f, 0.0f),	Vector2(0.0f, 0.0f) },
		{ Vector3(1.0f, 1.0f, 1.0f),	Vector3(1.0f, 0.0f, 0.0f),	Vector2(1.0f, 0.0f) },

		{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f),	Vector2(0.0f, 1.0f) }, // Normal Z -
		{ Vector3(1.0f, -1.0f, -1.0f),	Vector3(0.0f, 0.0f, -1.0f),	Vector2(1.0f, 1.0f) },
		{ Vector3(1.0f, 1.0f, -1.0f),	Vector3(0.0f, 0.0f, -1.0f),	Vector2(1.0f, 0.0f) },
		{ Vector3(-1.0f, 1.0f, -1.0f),	Vector3(0.0f, 0.0f, -1.0f),	Vector2(0.0f, 0.0f) },

		{ Vector3(-1.0f, -1.0f, 1.0f),	Vector3(0.0f, 0.0f, 1.0f),	Vector2(1.0f, 1.0f) },// Normal Z +
		{ Vector3(1.0f, -1.0f, 1.0f),	Vector3(0.0f, 0.0f, 1.0f),	Vector2(0.0f, 1.0f) },
		{ Vector3(1.0f, 1.0f, 1.0f),	Vector3(0.0f, 0.0f, 1.0f),	Vector2(0.0f, 0.0f) },
		{ Vector3(-1.0f, 1.0f, 1.0f),	Vector3(0.0f, 0.0f, 1.0f),	Vector2(1.0f, 0.0f) },
	};

	// 버텍스 버퍼 생성.
	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = vertices;
	HR_T(m_pDevice->CreateBuffer(&bd, &vbData, &m_pVertexBuffer));

	// 버텍스 버퍼 바인딩.
	m_VertexBufferStride = sizeof(Vertex);
	m_VertexBufferOffset = 0;

	// 4. Render() 에서 파이프라인에 바인딩할 인덱스 버퍼 생성
	WORD indices[] =
	{
		3,1,0, 2,1,3,
		6,4,5, 7,4,6,
		11,9,8, 10,9,11,
		14,12,13, 15,12,14,
		19,17,16, 18,17,19,
		22,20,21, 23,20,22
	};

	// 인덱스 개수 저장.
	m_nIndices = ARRAYSIZE(indices);

	bd = {};
	bd.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = indices;
	HR_T(m_pDevice->CreateBuffer(&bd, &ibData, &m_pIndexBuffer));

	// 6. Render() 에서 파이프라인에 바인딩할 상수 버퍼 생성	
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer));

	// 텍스쳐 로딩
	HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resources/Erpin_Icon.dds", nullptr, &m_pTextureRV));
	HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resources/CubeMap.dds", nullptr, &m_pSkyTextureRV));

	// 샘플러 생성
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR_T(m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear));

	// 초기값설정
	m_World = XMMatrixIdentity();
	XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -10.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = XMMatrixLookAtLH(Eye, At, Up);
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 100.0f);

	// 래스터라이저 속성 생성
	D3D11_RASTERIZER_DESC skyRsDesc = {};
	skyRsDesc.FillMode = D3D11_FILL_SOLID;
	skyRsDesc.CullMode = D3D11_CULL_BACK;
	skyRsDesc.FrontCounterClockwise = TRUE;
	skyRsDesc.DepthBias = 0;
	skyRsDesc.DepthBiasClamp = 0.0f;
	skyRsDesc.SlopeScaledDepthBias = 0.0f;
	skyRsDesc.DepthClipEnable = TRUE;
	skyRsDesc.ScissorEnable = FALSE;
	skyRsDesc.MultisampleEnable = FALSE;
	skyRsDesc.AntialiasedLineEnable = FALSE;
	HR_T(m_pDevice->CreateRasterizerState(&skyRsDesc, &m_SkyboxRS));

	D3D11_RASTERIZER_DESC defaultRsDesc = {};
	defaultRsDesc.FillMode = D3D11_FILL_SOLID;
	defaultRsDesc.CullMode = D3D11_CULL_BACK;
	defaultRsDesc.FrontCounterClockwise = FALSE;
	defaultRsDesc.DepthBias = 0;
	defaultRsDesc.DepthBiasClamp = 0.0f;
	defaultRsDesc.SlopeScaledDepthBias = 0.0f;
	defaultRsDesc.DepthClipEnable = TRUE;
	defaultRsDesc.ScissorEnable = FALSE;
	defaultRsDesc.MultisampleEnable = FALSE;
	defaultRsDesc.AntialiasedLineEnable = FALSE;
	HR_T(m_pDevice->CreateRasterizerState(&defaultRsDesc, &m_defaultRS));

	return true;
}

void LightApp::UninitScene()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pSkyVertexShader);
	SAFE_RELEASE(m_pPLightShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pSkyPixelShader);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pTextureRV);
	SAFE_RELEASE(m_pSamplerLinear);
	SAFE_RELEASE(m_defaultRS);

	SAFE_RELEASE(m_pSkyVertexShader);
	SAFE_RELEASE(m_pSkyPixelShader);
	SAFE_RELEASE(m_pSkyInputLayout);
	SAFE_RELEASE(m_SkyboxRS);
	SAFE_RELEASE(m_pSkyTextureRV);
}

bool LightApp::InitImGUI()
{
	/*
		ImGui 초기화.
	*/
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(this->m_pDevice, this->m_pDeviceContext);

	return true;
}

void LightApp::UninitImGUI()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void LightApp::RenderGUI()
{
	//아래부터는 ImGUI
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
		ImGui::SeparatorText("Object");
		ImGui::DragFloat3("Rotation", cbRotation, 0.01f, -360.0f, 360.0f);
		if (ImGui::Button("Reset")) {
			for (auto& val : cbRotation)
				val = 0.0f;
		}
		ImGui::PopID();
		ImGui::NewLine();

		ImGui::PushID(1);
		ImGui::SeparatorText("LightDir");
		ImGui::SliderFloat3("LightDir", lightDir, -1.0f, 1.0f);
		ImGui::ColorEdit3("Color", (float*)&m_LightColors);
		if (ImGui::Button("Reset")) {
			lightDir[0] = m_InitialLightDirs.x;
			lightDir[1] = m_InitialLightDirs.y;
			lightDir[2] = m_InitialLightDirs.z;

			m_LightColors = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		ImGui::PopID();
		ImGui::NewLine();

		ImGui::PushID(2);
		ImGui::SeparatorText("Camera");

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
		ImGui::DragFloat("##MinFarZ", &camFarZ[0], 0.1f, 0.01f, camFarZ[1] - 10.0f);
		ImGui::SameLine();
		ImGui::DragFloat("FarZ", &camFarZ[1], 0.1f, camFarZ[0] + 10.0f, 500.0f);
		ImGui::PopItemWidth();
		ImGui::SliderFloat("Fov", &camFov, 10.0f, 170.0f);
		if (ImGui::Button("Reset")) {
			camFarZ[0] = 0.01f;
			camFarZ[1] = 100.0f;

			camFov = 45.0f;
		}
		ImGui::PopID();
		ImGui::NewLine();

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK LightApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}
