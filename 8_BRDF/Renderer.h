#pragma once
#include "framework.h"


class Renderer
{
private:
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

public:
	Renderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> _deviceContext) : context(_deviceContext) {};

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;		// ·»´õ¸µ Å¸°Ùºä
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;		// ±íÀÌ°ª Ã³¸®¸¦ À§ÇÑ ‰X½º½ºÅÙ½Ç ºä

};

