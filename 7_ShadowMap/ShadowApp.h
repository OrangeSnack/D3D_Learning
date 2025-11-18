#pragma once
#include <d3d11_4.h>
#include "../BaseEngine/GameApp.h"

#include <imgui.h>
#include <wrl/client.h>
#include <directxtk/SimpleMath.h>
#include <string>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <Psapi.h>
#include "StaticMesh.h"
#include "SkeletalMesh.h"
#include <memory>

#pragma comment (lib, "d3d11.lib")

struct Transform {
	Matrix m_World = Matrix::Identity;		// 월드좌표계 변환행렬
	Vector3 Position = Vector3::Zero;		// x, y, z Position
	Vector3 Rotation = Vector3::Zero;		// pitch, yaw, roll
	Vector3 Scale = { 1.0f, 1.0f, 1.0f };	// x, y, z Scale
	Matrix* Parent = nullptr;				// Parent Matrix
};

template <typename T>
struct Object
{
	Object(ID3D11Device* _device) : model(_device) {}

	std::string name;
	T model;
	Transform transform;
};

class ShadowApp : public GameApp
{
public:
	ShadowApp(HINSTANCE hInstance);
	~ShadowApp();

	Vector4 m_ClearColor = Vector4(0.45f, 0.55f, 0.60f, 1.00f);

	// Directional 라이트
	XMFLOAT4 m_LightColors = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);				// 라이트 색상
	XMFLOAT4 m_InitialLightDirs = XMFLOAT4(0.0f, -1.0f, 1.0f, 1.0f);	// 초기 라이트 방향
	XMFLOAT4 m_CurrLightDirs = m_InitialLightDirs;							// 현재 라이트 방향
	XMFLOAT4 m_LightDirsEvaluated = {};										// 계산된 라이트 방향

	// VS 전달용 매트릭스
	Matrix m_View;			// 카메라좌표계 변환행렬
	Matrix m_Projection;	// ndc좌표계 변환행렬

	// ImGui --------

	// 카메라
	float camFov = 45.0f;
	float camFarZ[2] = { 0.01f, 100.0f };

	// 오브젝트
	std::vector<Transform> modelFactor;
	//float cbRotation[3] = {};
	//float scaleFactor = 1.0f;

	// 라이트
	float lightDir[3] = {
		m_InitialLightDirs.x,
		m_InitialLightDirs.y,
		m_InitialLightDirs.z
	};
	Vector4 m_Ambients = Vector4(0.3f, 0.3f, 0.3f, 1.0f);
	Vector4 m_Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4 m_Specular = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	// 메테리얼
	Vector4 m_MatAmbients = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4 m_MatDiffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4 m_MatSpecular = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	int m_Shiness = 32;
	bool useAB = true;		// 알파블랜드 사용
	bool useAS = true;		// 알파소팅 사용

	// End ---------

	// 렌더링 인터페이스
	ID3D11Device* m_pDevice = nullptr;						// 디바이스
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// 디바이스 컨텍스트
	IDXGISwapChain* m_pSwapChain = nullptr;					// 스왑체인
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// 렌더링 타겟뷰
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;  // 깊이값 처리를 위한 뎊스스텐실 뷰

	// 렌더링 정보
	
	ID3D11Buffer* m_pConstantBuffer = nullptr;		// 상수 버퍼
	ID3D11Buffer* m_pMatBuffer = nullptr;			// 메테리얼 버퍼
	ID3D11Buffer* m_pBoneBuffer = nullptr;			// 본 버퍼

	ID3D11ShaderResourceView* m_pTextureRV = nullptr;	// 텍스처 리소스 뷰.
	ID3D11ShaderResourceView* m_pNormalRV = nullptr;	// 노멀맵 리소스 뷰.
	ID3D11ShaderResourceView* m_pSpecularRV = nullptr;	// 스페큘러맵 리소스 뷰.
	ID3D11SamplerState* m_pSamplerLinear = nullptr;		// 샘플러 상태.
	ID3D11RasterizerState* m_defaultRS = nullptr;		// 기본 RS
	ID3D11ShaderResourceView* m_pRenderImage = nullptr;	// 렌더링 이미지 복사본 (알파블랜딩용)
	
	ID3D11BlendState* m_pAlphaBS = nullptr;				// 알파블랜드 스테이트
	ID3D11BlendState* m_pDefaultBS = nullptr;			// 기본블랜드 스테이트

	// FBX 모델
	std::vector < std::unique_ptr<Object<StaticMesh>>> models;		// 모델 저장
	std::vector < std::unique_ptr<Object<SkeletalMesh>>> skeletal_models;
	//std::unique_ptr<Model> model;
	ID3D11PixelShader* m_pAlphaClipShader = nullptr;		// 디퓨즈 전용 쉐이더
	ID3D11VertexShader* m_pVertexShader = nullptr;	// 정점 쉐이더
	ID3D11PixelShader* m_pPixelShader = nullptr;	// 퐁 픽셀 쉐이더
	ID3D11PixelShader* m_pBlinnPixelShader = nullptr;	// 블린 퐁 픽셀 쉐이더
	ID3D11PixelShader* m_pPLightShader = nullptr;	// 라이트 쉐이더
	ID3D11InputLayout* m_pInputLayout = nullptr;	// 입력 레이아웃
	
	// 스카이박스
	std::unique_ptr<StaticMesh> cube;
	ID3D11VertexShader* m_pSkyVertexShader = nullptr;	// 스카이박스 정점 쉐이더
	ID3D11PixelShader* m_pSkyPixelShader = nullptr;	// 스카이박스 픽셀 쉐이더
	ID3D11InputLayout* m_pSkyInputLayout = nullptr;	// 스카이박스입력 레이아웃
	ID3D11RasterizerState* m_SkyboxRS = nullptr;	// 스카이박스 전용 RS
	ID3D11ShaderResourceView* m_pSkyTextureRV = nullptr;	// 스카이박스 텍스처 리소스 뷰.

	// 그림자
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pShadowMap;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pShadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShadowMapSRV;
	D3D11_VIEWPORT m_ShadowViewport;
	Matrix m_ShadowProjection;
	Matrix m_ShadowView;
	Vector3 m_ShadowLookAt;
	Vector3 m_ShadowPos;
	float m_ShadowForwardDistFromCamera = 10.0f;
	float m_ShadowUpDistFromLookAt = 100.0f;


	bool Initialize(UINT Width, UINT Height) override;
	void Update() override;
	void Render() override;

	bool InitD3D();
	void UninitD3D();

	bool InitScene();		// 쉐이더,버텍스,인덱스
	void UninitScene();

	//IMGUI
	bool InitImGUI();
	void UninitImGUI();
	void RenderGUI();

public:
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

