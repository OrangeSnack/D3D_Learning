#include "PhongRenderer.h"
#include "RenderPipe.h"
#include "ResourceManager.h"
#include "PSResource.h"
#include "VSResource.h"
#include "../BaseEngine/Helper.h"

PhongRenderer::PhongRenderer()
{
	// 리소스 불러오기
	m_pVSShader = ResourceManager::GetInstance()->LoadFile<VSResource>(L"../Resources/Shader/VS/SkeletalVertexShader.hlsl");
	m_pPSShader = ResourceManager::GetInstance()->LoadFile<PSResource>(L"../Resources/Shader/PS/AlphaClipPixelShader.hlsl");
	
	// TODO::filePath 문제 다른방법 없나 찾아보기
	m_pVSShader->Initialize();
	m_pPSShader->Initialize();

	auto device = ResourceManager::GetInstance()->GetDevice();

	// 인풋 레이아웃 생성
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	HR_T(device->CreateInputLayout(
		layout, ARRAYSIZE(layout), m_pVSShader->m_pBlob->GetBufferPointer(),
		m_pVSShader->m_pBlob->GetBufferSize(), &m_pInputLayout
	));
}

void PhongRenderer::Render()
{
	
}
