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

#pragma comment (lib, "d3d11.lib")

using namespace DirectX::SimpleMath;
using namespace DirectX;

struct Vertex
{
	Vector3 Pos;		// ���� ��ġ ����.
	Vector3 Normal;
};

struct ConstantBuffer
{
	Matrix mWorld;
	Matrix mView;
	Matrix mProjection;
	Matrix mNormalMatrix;

	Vector4 vLightDir;
	Vector4 vLightColor;
	Vector4 vOutputColor;
};

class LightApp : public GameApp
{
public:
	LightApp(HINSTANCE hInstance);
	~LightApp();

	Vector4 m_ClearColor = Vector4(0.45f, 0.55f, 0.60f, 1.00f);

	// ����Ʈ
	XMFLOAT4 m_LightColors = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);				// ����Ʈ ����
	XMFLOAT4 m_InitialLightDirs = XMFLOAT4(-0.577f, 0.577f, -0.577f, 1.0f);	// �ʱ� ����Ʈ ����
	XMFLOAT4 m_CurrLightDirs = m_InitialLightDirs;							// ���� ����Ʈ ����
	XMFLOAT4 m_LightDirsEvaluated = {};										// ���� ����Ʈ ����

	// VS ���޿� ��Ʈ����
	Matrix m_World;			// ������ǥ�� ��ȯ���
	Matrix m_View;			// ī�޶���ǥ�� ��ȯ���
	Matrix m_Projection;	// ndc��ǥ�� ��ȯ���

	// ImGui --------

	// ī�޶�
	float camFov = 45.0f;
	float camFarZ[2] = { 0.01f, 100.0f };

	// ������Ʈ
	float cbRotation[3] = {};		// pitch, yaw, roll

	// ����Ʈ
	float lightDir[3] = {
		m_InitialLightDirs.x,
		m_InitialLightDirs.y,
		m_InitialLightDirs.z
	};

	// End ---------

	// ������ �������̽�
	ID3D11Device* m_pDevice = nullptr;						// ����̽�
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// ����̽� ���ؽ�Ʈ
	IDXGISwapChain* m_pSwapChain = nullptr;				// ����ü��
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// ������ Ÿ�ٺ�
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;  // ���̰� ó���� ���� �X�����ٽ� ��

	// ������ ����
	ID3D11VertexShader* m_pVertexShader = nullptr;	// ���� ���̴�
	ID3D11PixelShader* m_pPixelShader = nullptr;	// �ȼ� ���̴�
	ID3D11PixelShader* m_pPLightShader = nullptr;	// ����Ʈ ���̴�
	ID3D11InputLayout* m_pInputLayout = nullptr;	// �Է� ���̾ƿ�
	ID3D11Buffer* m_pVertexBuffer = nullptr;		// ���ؽ� ����
	UINT m_VertexBufferStride = 0;					// ���ؽ� �ϳ��� ũ��
	UINT m_VertexBufferOffset = 0;					// ���ؽ� ������ ������

	ID3D11Buffer* m_pIndexBuffer = nullptr;			// �ε��� ����
	ID3D11Buffer* m_pConstantBuffer = nullptr;		// ��� ����
	int m_nIndices = 0;								// �ε��� ����

	bool Initialize(UINT Width, UINT Height) override;
	void Update() override;
	void Render() override;

	bool InitD3D();
	void UninitD3D();

	bool InitScene();		// ���̴�,���ؽ�,�ε���
	void UninitScene();

	//IMGUI
	bool InitImGUI();
	void UninitImGUI();
	void RenderGUI();

public:
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

