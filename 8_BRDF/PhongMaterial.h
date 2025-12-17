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
};

