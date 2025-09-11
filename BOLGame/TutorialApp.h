#pragma once

#include <d3d11_4.h>
#include "../BaseEngine/GameApp.h"

#include <dxgi1_4.h>
#include <imgui.h>
#include <wrl/client.h>
#include <directxtk/SimpleMath.h>
#include <string>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <Psapi.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment(lib,"dxgi.lib")

struct ConstantBuffer
{
	Matrix mWorld;
	Matrix mView;
	Matrix mProjection;
};

class TutorialApp : public GameApp
{
public:
	TutorialApp(HINSTANCE hInstance);
	~TutorialApp();

	//IMGUI
	Microsoft::WRL::ComPtr<IDXGIFactory4> m_pDXGIFactory;		// DXGI���丮
	Microsoft::WRL::ComPtr<IDXGIAdapter3> m_pDXGIAdapter;		// ����ī�� ������ ���� ������ �������̽�

	Vector4 m_ClearColor = Vector4(0.45f, 0.55f, 0.60f, 1.00f);

	float camPos[3] = { 0.0f, 0.0f, -10.0f };
	float camFov = 90.0f;
	float camFarZ[2] = { 0.01f, 100.0f };
	float camPrevpos[3] = { camPos[0],camPos[1],camPos[2] };

	float cb1Pos[3] = { 0.0f, 0.0f, 0.0f };
	float cb1Scale[3] = { 1.0f, 1.0f, 1.0f };

	float cb2Pos[3] = { -5.0f, 0.0f, 0.0f };
	float cb2Scale[3] = { 0.5f, 0.5f, 0.5f };

	float cb3Pos[3] = { -5.0f, 0.0f, 0.0f };
	float cb3Scale[3] = { 0.5f, 0.5f, 0.5f };

	// ������ �������̽�
	ID3D11Device* m_pDevice = nullptr;						// ����̽�
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// ����̽� ���ؽ�Ʈ
	IDXGISwapChain* m_pSwapChain = nullptr;				// ����ü��
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// ������ Ÿ�ٺ�
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;  // ���̰� ó���� ���� �X�����ٽ� ��

	// ������ ����
	ID3D11VertexShader* m_pVertexShader = nullptr;	// ���� ���̴�
	ID3D11PixelShader* m_pPixelShader = nullptr;	// �ȼ� ���̴�
	ID3D11InputLayout* m_pInputLayout = nullptr;	// �Է� ���̾ƿ�
	ID3D11Buffer* m_pVertexBuffer = nullptr;		// ���ؽ� ����
	UINT m_VertexBufferStride = 0;					// ���ؽ� �ϳ��� ũ��
	UINT m_VertexBufferOffset = 0;					// ���ؽ� ������ ������

	ID3D11Buffer* m_pIndexBuffer = nullptr;			// �ε��� ����
	ID3D11Buffer* m_pConstantBuffer = nullptr;		// ��� ����
	int m_nIndices = 0;								// �ε��� ����

	// ���̴� ���޿� ��Ʈ����
	Matrix m_View;			// ī�޶���ǥ�� ���
	Matrix m_Projection;	// ndc��ǥ�� ���

	// ����
	ConstantBuffer cb1;
	ConstantBuffer cb2;
	ConstantBuffer cb3;

	virtual bool Initialize(UINT Width, UINT Height);
	virtual void Update();
	virtual void Render();

	bool InitD3D();
	void UninitD3D();

	bool InitScene();		// ���̴�,���ؽ�,�ε���
	void UninitScene();

	//IMGUI
	bool InitImGUI();
	void UninitImGUI();
	void RenderGUI();

	void GetDisplayMemoryInfo(std::string& out);
	void GetVirtualMemoryInfo(std::string& out);

public:
	void SetCBPos();
	void SetCamMat();

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

