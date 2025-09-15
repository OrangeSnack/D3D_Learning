#pragma once
#include <windows.h>
#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "../BaseEngine/GameApp.h"
#include <vector>

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

	Vector4 vLightDir[2];
	Vector4 vLightColor[2];
	Vector4 vOutputColor;
};

class LightApp : public GameApp
{
public:
	LightApp(HINSTANCE hInstance);
	~LightApp();

	Vector4 m_ClearColor = Vector4(0.45f, 0.55f, 0.60f, 1.00f);

	// ImGui ����
	float camPos[3] = { 0.0f, 0.0f, -10.0f };
	float camFov = 90.0f;
	float camFarZ[2] = { 0.01f, 100.0f };
	float camPrevpos[3] = { camPos[0],camPos[1],camPos[2] };

	float cbPos[3][3] = {
		{0.0f, 0.0f, 0.0f},
		{ -5.0f, 0.0f, 0.0f },
		{ -5.0f, 0.0f, 0.0f } };
	float cbScale[3][3] = {
		{ 1.0f, 1.0f, 1.0f },
		{ 0.5f, 0.5f, 0.5f },
		{ 0.5f, 0.5f, 0.5f } };

	// DXGI
	IDXGIFactory2* m_pDxgiFactory;
	IDXGIAdapter2* m_pDxgiAdapter;

	// ������ �������̽�
	ID3D11Device* m_pDevice = nullptr;						// ����̽�
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// ����̽� ���ؽ�Ʈ
	IDXGISwapChain* m_pSwapChain = nullptr;				// ����ü��
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// ������ Ÿ�ٺ�
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;  // ���̰� ó���� ���� �X�����ٽ� ��

	// ������ ����
	ID3D11VertexShader* m_pVertexShader = nullptr;	// ���� ���̴�
	ID3D11PixelShader* m_pPixelShader = nullptr;	// �ȼ� ���̴�
	ID3D11PixelShader* m_pPLightShader = nullptr;	// �ȼ� ����Ʈ ���̴�
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
	ConstantBuffer cb;

	// ������Ʈ ���� ��Ʈ����
	std::vector<Matrix> objWorlds;

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

public:
	void SetMatrix(Matrix mat, int index);
	void SetCBPos(Matrix mat, int index);
	void SetCamMat();

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

