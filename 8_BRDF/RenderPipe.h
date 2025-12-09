#pragma once
#include "framework.h"
#include "Renderer.h"
#include <vector>
#include <memory>
#include <map>
#include "../BaseEngine/Camera.h"
#include "RenderStruct.h"

class RenderPipe
{
private:
	RenderPipe() = default;
	//std::vector<std::shared_ptr<Renderer>> renderers;
	std::map<int, std::vector<std::shared_ptr<Renderer>>> passes;

protected:
	// 기본 렌더 인터페이스
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext;		// 디바이스 컨텍스트
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;				// 스왑체인

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;		// 렌더링 타겟뷰
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;		// 깊이값 처리를 위한 뎊스스텐실 뷰

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pDafaultSamplerLinear;		// 샘플러 상태.
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pDefaultRS;				// 기본 RS

	Microsoft::WRL::ComPtr<ID3D11BlendState> m_pDefaultBS;		// 기본블랜드 스테이트
	D3D11_VIEWPORT m_defaultViewport;							// 기본 뷰포트

	// 버퍼 기본색상
	DirectX::SimpleMath::Vector4 m_ClearColor;

	// VS 전달용 버퍼 (메인카메라)
	CamBuffer m_camMat;
public:
	static std::unique_ptr<RenderPipe> instance;
	static void Initialize();
	void Start();

public:
	void InitD3D();
	void UnInitD3D();
	void Render();

public:
	template <typename T, typename... Args>
	std::weak_ptr<Renderer> AddRenderer(int _passNum, Args&&... args);

	template <typename T>
	bool RemoveRenderer(int _passNum, std::shared_ptr<T>& _renderer);
	
};

template <typename T, typename... Args>
std::weak_ptr<Renderer>
RenderPipe::AddRenderer(int _passNum, Args&&... args)
{
	std::shared_ptr<T> temp = std::make_shared<T>(std::forward<Args>(args));
	passes[_passNum].push_back(temp);

	return temp;
}

template <typename T>
bool RenderPipe::RemoveRenderer(int _passNum, std::shared_ptr<T>& _renderer)
{
	if (_renderer && (passes.find(_passNum) != passes.end())) {
		auto it = std::find(passes[_passNum].begin(), passes[_passNum].end(), _renderer);

		if (it != passes.end()) {
			passes[_passNum].erase(it);
			return true;
		}
	}
	
	return false;
}

