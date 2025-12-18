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
	if (!mesh || mesh->meshData || mesh->gpuBuffer)
		return;
	
	// TODO:: 렌더파이프에 렌더러 등록하기, 메테리얼 읽어서 렌더러에 메시 정보 보내기.
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

	// TODO::렌더러 제거 명령
	
}
