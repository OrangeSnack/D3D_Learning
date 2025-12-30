#pragma once
#include "Material.h"
#include "Texture2D.h"

class PhongMaterial : public Material
{
public:
	std::shared_ptr<Texture2D> diffuse = nullptr;		// µ«ª¡Ó∏ 
	std::shared_ptr<Texture2D> specular = nullptr;		// Ω∫∆Â≈ß∑Ø∏ 
	std::shared_ptr<Texture2D> normal = nullptr;		// ≥Î∏÷∏ 
	std::shared_ptr<Texture2D> emissive = nullptr;		// πﬂ±§∏ 

	DirectX::SimpleMath::Vector4 matDiffuse = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::SimpleMath::Vector4 matSpecular = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	UINT shiness = 32;
};

