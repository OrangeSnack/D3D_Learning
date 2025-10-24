#include "MeshApp.h"
#include "../BaseEngine/Helper.h"
#include <d3dcompiler.h>
#include <Directxtk/DDSTextureLoader.h>
#include <directxtk/WICTextureLoader.h>
#include <filesystem>
#include <algorithm>

#pragma comment(lib,"d3dcompiler.lib")

MeshApp::MeshApp(HINSTANCE hInstance) : GameApp(hInstance)
{
}

MeshApp::~MeshApp()
{
	UninitImGUI();
	UninitScene();
	UninitD3D();
}

bool MeshApp::Initialize(UINT Width, UINT Height)
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

void MeshApp::Update()
{
	__super::Update();

	//float t = GameTimer::m_Instance->TotalTime();
	m_World = 
		XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor) *
		XMMatrixRotationRollPitchYaw(cbRotation[0], cbRotation[1], cbRotation[2]);

	// 라이트 방향
	m_CurrLightDirs.x = lightDir[0];
	m_CurrLightDirs.y = lightDir[1];
	m_CurrLightDirs.z = lightDir[2];

	m_LightDirsEvaluated = m_CurrLightDirs;

	// 프로젝션 매트릭스 설정
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4 * (camFov / 45.0f), m_ClientWidth / (FLOAT)m_ClientHeight, camFarZ[0], camFarZ[1]);
	m_Camera.GetViewMatrix(m_View);

	// 메시 순서 결정
	//std::sort(model);
}

void MeshApp::Render()
{
	float color[4] = { 0.0f, 0.5f, 0.5f, 1.0f };

	// Update matrix variables and lighting variables
	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(m_World);
	cb1.mView = XMMatrixTranspose(m_View);
	cb1.mProjection = XMMatrixTranspose(m_Projection);
	cb1.mNormalMatrix = XMMatrixInverse(nullptr, m_World);
	cb1.vLightDir = m_LightDirsEvaluated;
	cb1.vLightColor = m_LightColors;
	cb1.camPos = (Vector4) m_Camera.m_Position;
	cb1.ambient = m_Ambients;
	cb1.diffuse = m_Diffuse;
	cb1.specular = m_Specular;
	cb1.Matambient = m_MatAmbients;
	cb1.Matdiffuse = m_MatDiffuse;
	cb1.Matspecular = m_MatSpecular;
	cb1.shiness = m_Shiness;
	m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb1, 0, 0);

	// Clear 
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Render Setting
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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

	cube->Draw(m_pDeviceContext, nullptr, false);
	//m_pDeviceContext->DrawIndexed(m_nCubeIndices, 0, 0);

	// ----- 모델 렌더링 -----

	// 렌더타겟 설정
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	m_pDeviceContext->RSSetState(m_defaultRS);
	m_pDeviceContext->OMSetBlendState(m_pAlphaBS, nullptr, 0xFFFFFFFF);

	if (useClip)
		m_pDeviceContext->PSSetShader(m_pBlinnPixelShader, nullptr, 0);
	else
		m_pDeviceContext->PSSetShader(m_pDiffuseShader, nullptr, 0);

	/*for (int i = 0; i < m_pVertexBuffer.size(); i++) {
		m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer[i], &m_VertexBufferStride, &m_VertexBufferOffset);
		m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0);

		UINT matIdx = model.scene->mMeshes[i]->mMaterialIndex;
		modelRV[0] = m_pMaterials[matIdx].diffuse;
		modelRV[1] = m_pMaterials[matIdx].normal;
		modelRV[2] = m_pMaterials[matIdx].specular;
		modelRV[3] = m_pMaterials[matIdx].emissive;
		modelRV[4] = m_pSkyTextureRV;

		m_pDeviceContext->PSSetShaderResources(0, 5, modelRV);

		m_pDeviceContext->DrawIndexed(m_nIndices[i], 0, 0);
	}*/

	// 이걸로 대체
	model->Draw(m_pDeviceContext, nullptr);

	// GUI Render
	RenderGUI();

	// Present our back buffer to our front buffer
	m_pSwapChain->Present(0, 0);
}

bool MeshApp::InitD3D()
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

	// 알파 블랜딩 생성
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR_T(m_pDevice->CreateBlendState(&blendDesc, &m_pAlphaBS));
	
	// 기본 블랜딩 가져오기
	m_pDeviceContext->OMGetBlendState(&m_pDefaultBS, nullptr, nullptr);

	return true;
}

void MeshApp::UninitD3D()
{
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pSwapChain); 
	SAFE_RELEASE(m_pRenderTargetView);
}

bool MeshApp::InitScene()
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
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

	HR_T(CompileShaderFromFile(L"BlinnPhongPixelShader.hlsl", "main", "ps_4_0", &pixelShader));
	HR_T(m_pDevice->CreatePixelShader(pixelShader->GetBufferPointer(),
		pixelShader->GetBufferSize(), NULL, &m_pBlinnPixelShader));
	SAFE_RELEASE(pixelShader);

	HR_T(CompileShaderFromFile(L"SolidPixelShader.hlsl", "main", "ps_4_0", &pixelShader));
	HR_T(m_pDevice->CreatePixelShader(pixelShader->GetBufferPointer(),
		pixelShader->GetBufferSize(), NULL, &m_pPLightShader));
	SAFE_RELEASE(pixelShader);

	HR_T(CompileShaderFromFile(L"SkyBoxPixelShader.hlsl", "main", "ps_4_0", &pixelShader));
	HR_T(m_pDevice->CreatePixelShader(pixelShader->GetBufferPointer(),
		pixelShader->GetBufferSize(), NULL, &m_pSkyPixelShader));
	SAFE_RELEASE(pixelShader);

	HR_T(CompileShaderFromFile(L"AlphaClipPixelShader.hlsl", "main", "ps_4_0", &pixelShader));
	HR_T(m_pDevice->CreatePixelShader(pixelShader->GetBufferPointer(),
		pixelShader->GetBufferSize(), NULL, &m_pDiffuseShader));
	SAFE_RELEASE(pixelShader);
	
	// 모델 로딩
	model = std::make_unique<Model>(m_pDevice);
	cube = std::make_unique<Model>(m_pDevice);

	//model.LoadFile("../Resources/Models/Mass/Character.fbx");
	//model.LoadFile("../Resources/Models/Zelda/Zelda.fbx");
	model->LoadFile(L"../Resources/Models/Tree/Tree.fbx");
	cube->LoadFile(L"../Resources/Models/Cube/Cube.fbx");

	// Render() 에서 파이프라인에 바인딩할 상수 버퍼 생성
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer));

	// 스카이박스 텍스쳐 로딩
	//HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resources/Church.dds", nullptr, &m_pSkyTextureRV));
	HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resources/CubeMap.dds", nullptr, &m_pSkyTextureRV));
	//HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resources/Hanako.dds", nullptr, &m_pSkyTextureRV));
	

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

	return true;
}

void MeshApp::UninitScene()
{
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pSkyVertexShader);
	SAFE_RELEASE(m_pPLightShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pSkyPixelShader);
	SAFE_RELEASE(m_pInputLayout);
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

bool MeshApp::InitImGUI()
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

void MeshApp::UninitImGUI()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void MeshApp::RenderGUI()
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
		ImGui::DragFloat("Scale", &scaleFactor, 0.001f, 0.001f, 10.0f);

		if (ImGui::Button("Reset")) {
			for (auto& val : cbRotation)
				val = 0.0f;
			scaleFactor = 1.0f;
		}
		ImGui::PopID();
		ImGui::NewLine();

		ImGui::PushID(1);
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

		ImGui::PushID(2);
		ImGui::SeparatorText("Light");
		ImGui::Checkbox("BlinnPhong", &useClip);
		ImGui::SliderFloat3("LightDir", lightDir, -1.0f, 1.0f);
		ImGui::ColorEdit4("Color(l_i)", (float*)&m_LightColors);
		ImGui::ColorEdit4("Ambients(l_a)", (float*)&m_Ambients);
		ImGui::ColorEdit4("Diffuse", (float*)&m_Diffuse);
		ImGui::ColorEdit4("Specular", (float*)&m_Specular);
		if (ImGui::Button("Reset")) {
			m_Ambients = Vector4(0.3f, 0.3f, 0.3f, 1.0f);
			m_Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			m_Specular = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

			lightDir[0] = m_InitialLightDirs.x;
			lightDir[1] = m_InitialLightDirs.y;
			lightDir[2] = m_InitialLightDirs.z;

			m_LightColors = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		ImGui::PopID();
		ImGui::NewLine();


		ImGui::PushID(3);
		ImGui::SeparatorText("Material");

		ImGui::ColorEdit4("MatAmbients(k_a)", (float*)&m_MatAmbients);
		ImGui::ColorEdit4("MatDiffuse(k_d)", (float*)&m_MatDiffuse);
		ImGui::ColorEdit4("MatSpecular(k_s)", (float*)&m_MatSpecular);
		ImGui::SliderInt("Shiness(a)", &m_Shiness, 32, 256);
		if (ImGui::Button("Reset")) {
			m_MatAmbients = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			m_MatDiffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			m_MatSpecular = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			m_Shiness = 32;
		}

		

		ImGui::PopID();
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK MeshApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}

//bool MeshApp::LoadVertex(std::vector<Vertex>* _vertices, const aiMesh* _mesh)
//{
//	for (int i = 0; i < _mesh->mNumVertices; i++) {
//		Vertex v;
//
//		v.Pos = Vector3(_mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z);
//
//		if (_mesh->HasNormals()) {
//			v.Normal = Vector3(_mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z);
//		}
//
//		if (_mesh->HasTangentsAndBitangents()) {
//			v.Tangent = Vector3(_mesh->mTangents[i].x, _mesh->mTangents[i].y, _mesh->mTangents[i].z);
//			v.BiNormal = Vector3(_mesh->mBitangents[i].x, _mesh->mBitangents[i].y, _mesh->mBitangents[i].z);
//		}
//
//		if (_mesh->HasTextureCoords(0)) {
//			v.Tex = Vector2(_mesh->mTextureCoords[0][i].x, _mesh->mTextureCoords[0][i].y);
//		}
//
//		_vertices->push_back(v);
//	}
//
//	return true;
//}

//bool MeshApp::LoadIndex(std::vector<UINT>* _indices, const aiMesh* _mesh)
//{
//	for (int i = 0; i < _mesh->mNumFaces; i++) {
//		for(int j = 0; j < _mesh->mFaces[i].mNumIndices; j++)
//			_indices->push_back(UINT(_mesh->mFaces[i].mIndices[j]));
//	}
//
//	return true;
//}
//
//bool MeshApp::LoadMaterials(std::vector<Materials>& _out, const Model* _model)
//{
//	const aiScene* scene = _model->scene;
//
//	if (!_model->scene->HasMaterials())
//		return false;
//
//	_out.resize(_model->scene->mNumMaterials);
//
//	for (int i = 0; i < _model->scene->mNumMaterials; i++) {
//		// TODO :: 메테리얼으로 ResourceView 만들기
//		aiString aiStr;
//		aiMaterial* aiMat = scene->mMaterials[i];
//
//		// 디퓨즈
//		if (aiMat->GetTextureCount(aiTextureType_DIFFUSE)) {
//			if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &aiStr) == aiReturn_SUCCESS) {
//				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
//				std::filesystem::path relativePath = _model->filePath / p.filename();
//
//				HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].diffuse));
//			}
//		}
//
//		// 스페큘러
//		if (aiMat->GetTextureCount(aiTextureType_SPECULAR)) {
//			if (aiMat->GetTexture(aiTextureType_SPECULAR, 0, &aiStr) == aiReturn_SUCCESS) {
//				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
//				std::filesystem::path relativePath = _model->filePath / p.filename();
//
//				HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].specular));
//			}
//		}
//
//		// 노말
//		if (aiMat->GetTextureCount(aiTextureType_NORMALS)) {
//			if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &aiStr) == aiReturn_SUCCESS) {
//				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
//				std::filesystem::path relativePath = _model->filePath / p.filename();
//
//				HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].normal));
//			}
//		}
//
//		// 이미션
//		if (aiMat->GetTextureCount(aiTextureType_EMISSIVE)) {
//			if (aiMat->GetTexture(aiTextureType_EMISSIVE, 0, &aiStr) == aiReturn_SUCCESS) {
//				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
//				std::filesystem::path relativePath = _model->filePath / p.filename();
//
//				HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].emissive));
//			}
//		}
//	}
//
//	return true;
//}
