#pragma once
#include "framework.h"
#include "Renderer.h"

class PhongRenderer : public Renderer
{
	ID3D11VertexShader* m_pVertexShader = nullptr;		// 정점 쉐이더
	ID3D11PixelShader* m_pAlphaClipShader = nullptr;	// 디퓨즈 전용 쉐이더\
	ID3D11InputLayout* m_pInputLayout = nullptr;	// 입력 레이아웃
};

