#pragma once
#include <memory>
#include "GameResource.h"
#include "Texture2D.h"
#include "Renderer.h"

class Material : public GameResource
{
public:
	std::shared_ptr<Renderer> renderer = nullptr;

	//std::shared_ptr<Texture2D> diffuse = nullptr;		// µ«ª¡Ó∏ 
	//std::shared_ptr<Texture2D> specular = nullptr;		// Ω∫∆Â≈ß∑Ø∏ 
	//std::shared_ptr<Texture2D> normal = nullptr;		// ≥Î∏÷∏ 
	//std::shared_ptr<Texture2D> emissive = nullptr;		// πﬂ±§∏ 
	//DirectX::SimpleMath::Color BaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };		// ∫£¿ÃΩ∫ ƒ√∑Ø
};

