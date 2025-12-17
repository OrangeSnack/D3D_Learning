#include "MeshRenderer.h"
#include "StaticMesh.h"
#include "ResourceManager.h"
#include "../BaseEngine/Helper.h"
#include "RenderPipe.h"
#include "PBRRenderer.h"
#include "PhongRenderer.h"

using namespace Microsoft::WRL;

void MeshRenderer::SetMesh(std::shared_ptr<StaticMesh>& _mesh)
{
	mesh = _mesh;
	Start();
}

void MeshRenderer::Start()
{
	// 유효성 확인
	if (!mesh || mesh->modelVertices.empty() || mesh->modelIndices.empty())
		return;

	// 디바이스 가져오기
	auto device = ResourceManager::GetInstance()->GetDevice();

	// 머터리얼별 메시 분류 (머터리얼별 개별쉐이딩 지원용)
	auto scene = mesh->scene;
	for (int i = 0; scene->mNumMeshes; i++) {
		aiMesh* currMesh = scene->mMeshes[i];
		auto& group = meshGroup[currMesh->mMaterialIndex];
		group.push_back(currMesh);
	}

	// 모델 버퍼 생성
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData = {};
	for (int i = 0; i < mesh->modelVertices.size(); i++) {
		bd.ByteWidth = UINT(sizeof(Mesh_Vertex) * mesh->modelVertices[i].size());
		vbData.pSysMem = mesh->modelVertices[i].data();

		ComPtr<ID3D11Buffer> tempBuffer = nullptr;
		HR_T(device->CreateBuffer(&bd, &vbData, &tempBuffer));

		if (tempBuffer)
			vertexBuffers.push_back(tempBuffer);
	}

	// 인덱스 버퍼 생성
	bd = {};
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData = {};


	for (int i = 0; i < mesh->modelIndices.size(); i++) {
		bd.ByteWidth = UINT(sizeof(UINT) * mesh->modelIndices[i].size());
		ibData.pSysMem = mesh->modelIndices[i].data();

		ComPtr<ID3D11Buffer> tempBuffer = nullptr;
		HR_T(device->CreateBuffer(&bd, &ibData, &tempBuffer));

		if (tempBuffer)
			indexBuffer.push_back(tempBuffer);
	}

	// 버텍스 버퍼 바인딩.
	vertexBufferStride = sizeof(Mesh_Vertex);
	vertexBufferOffset = 0;

	for (const auto& indices : mesh->modelIndices)
		nIndices.push_back(UINT(indices.size()));
	
	/*if (auto renderer = dynamic_pointer_cast<PBRRenderer>(mesh->modelMaterials[0]->renderer))
		RenderPipe::GetInstance()->AddRenderer<PBRRenderer>(1, renderer);
	else if (auto renderer = dynamic_pointer_cast<PhongRenderer>(mesh->modelMaterials[0]->renderer))
		RenderPipe::GetInstance()->AddRenderer<PhongRenderer>(2, renderer);
	else
		throw std::runtime_error("MeshRenderer::Unknown Renderer Type!!");*/
}

void MeshRenderer::Update()
{

}

MeshRenderer::~MeshRenderer()
{

	// 렌더러 제거 명령
	
}
