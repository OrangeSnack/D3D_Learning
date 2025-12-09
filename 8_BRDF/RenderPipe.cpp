#include "RenderPipe.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

void RenderPipe::Initialize()
{
	if (!instance) {
		instance = make_unique<RenderPipe>();
	}

	// 인스턴스 초기화 뭉탱이
	instance->Start();
}

void RenderPipe::Start()
{

	// 버퍼 기본색상
	m_ClearColor = Vector4(0.45f, 0.55f, 0.60f, 1.00f);
}

void RenderPipe::InitD3D()
{

}

void RenderPipe::UnInitD3D()
{

}

void RenderPipe::Render()
{
	for (const auto& pass : passes) {
		for (const auto& renderer : pass.second) {
			renderer->Render();
		}
	}
}
