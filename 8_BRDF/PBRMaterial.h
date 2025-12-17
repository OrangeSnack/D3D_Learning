#pragma once
#include "Material.h"
class PBRMaterial : public Material
{
public:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> baseColor;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metallic;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ao;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> emissive;
};

