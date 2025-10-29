#pragma once

#include <d3d11_4.h>
#include <directxtk/SimpleMath.h>

struct Materials {
	ID3D11ShaderResourceView* diffuse = nullptr;		// µðÇ»Áî¸Ê
	ID3D11ShaderResourceView* specular = nullptr;		// ½ºÆåÅ§·¯¸Ê
	ID3D11ShaderResourceView* normal = nullptr;			// ³ë¸Ö¸Ê
	ID3D11ShaderResourceView* emissive = nullptr;		// ¹ß±¤¸Ê
};
struct Vertex
{
	DirectX::SimpleMath::Vector3 Pos;		// Á¤Á¡ À§Ä¡ Á¤º¸
	DirectX::SimpleMath::Vector3 Normal;	// ³ë¸Ö
	DirectX::SimpleMath::Vector3 Tangent;	// ÅºÁ¨Æ®
	DirectX::SimpleMath::Vector3 BiNormal;	// Bi³ë¸Ö
	DirectX::SimpleMath::Vector2 Tex;		// ÅØ½ºÃÄ UV
};

struct ConstantBuffer
{
	DirectX::SimpleMath::Matrix mWorld;
	DirectX::SimpleMath::Matrix mView;
	DirectX::SimpleMath::Matrix mProjection;
	DirectX::SimpleMath::Matrix mNormalMatrix;

	DirectX::SimpleMath::Vector4 vLightDir;
	DirectX::SimpleMath::Vector4 vLightColor;

	DirectX::SimpleMath::Vector4 camPos;

	DirectX::SimpleMath::Vector4 ambient;
	DirectX::SimpleMath::Vector4 diffuse;
	DirectX::SimpleMath::Vector4 specular;

	DirectX::SimpleMath::Vector4 Matambient;
	DirectX::SimpleMath::Vector4 Matdiffuse;
	DirectX::SimpleMath::Vector4 Matspecular;

	int shiness;
	DirectX::SimpleMath::Vector3 padding;

	DirectX::SimpleMath::Matrix skinMat;
	DirectX::SimpleMath::Matrix skinNorm;
};