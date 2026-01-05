#pragma once

#include <d3d11_4.h>
#include <directxtk/SimpleMath.h>

#define BONE_MAXSIZE 256

//struct Materials {
//	ID3D11ShaderResourceView* diffuse = nullptr;		// µðÇ»Áî¸Ê
//	ID3D11ShaderResourceView* specular = nullptr;		// ½ºÆåÅ§·¯¸Ê
//	ID3D11ShaderResourceView* normal = nullptr;			// ³ë¸Ö¸Ê
//	ID3D11ShaderResourceView* emissive = nullptr;		// ¹ß±¤¸Ê
//	DirectX::SimpleMath::Color BaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };	// º£ÀÌ½º ÄÃ·¯
//};
struct PBR_Materials {
	ID3D11ShaderResourceView* diffuse = nullptr;		// µðÇ»Áî¸Ê
	ID3D11ShaderResourceView* normal = nullptr;			// ³ë¸Ö¸Ê
	ID3D11ShaderResourceView* metalic = nullptr;		// ¸ÞÅ»¸¯¸Ê
	ID3D11ShaderResourceView* roughness = nullptr;		// °ÅÄ¥±â¸Ê
	ID3D11ShaderResourceView* ao = nullptr;				// AO¸Ê
	ID3D11ShaderResourceView* emissive = nullptr;		// ¹ß±¤¸Ê
};

struct PBR_MatBuffer {
	DirectX::SimpleMath::Color baseColor = { 1.0f, 1.0f, 1.0f, 1.0f };	// º£ÀÌ½º ÄÃ·¯
	float metalic = 0.0f;
	float roughness = 0.0f;
	float aoStrength = 0.0f;
	float emissive = 0.0f;
};

struct Vertex
{
	DirectX::SimpleMath::Vector3 Pos;		// Á¤Á¡ À§Ä¡ Á¤º¸
	DirectX::SimpleMath::Vector3 Normal;	// ³ë¸Ö
	DirectX::SimpleMath::Vector3 Tangent;	// ÅºÁ¨Æ®
	DirectX::SimpleMath::Vector3 BiTangent;	// BiÅºÁ¨Æ®
	DirectX::SimpleMath::Vector2 Tex;		// ÅØ½ºÃÄ UV
	int boneIndices[4] = { -1, -1, -1, -1 };				// ¹öÅØ½º¿Í ¿¬°áµÈ º»µéÀÇ ÀÎµ¦½º
	float boneWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };		// °¢ º»µéÀÇ °¡ÁßÄ¡
};

struct ConstantBuffer
{
	DirectX::SimpleMath::Matrix mWorld;
	DirectX::SimpleMath::Matrix mView;
	DirectX::SimpleMath::Matrix mProjection;
	DirectX::SimpleMath::Matrix mNormalMatrix;
	DirectX::SimpleMath::Vector4 mCamPos;
};

struct ShadowBuffer
{
	DirectX::SimpleMath::Matrix ShadowView;
	DirectX::SimpleMath::Matrix ShadowProjection;
};

//struct MaterialBuffer
//{
//	DirectX::SimpleMath::Vector4 vLightDir;
//	DirectX::SimpleMath::Vector4 vLightColor;
//
//	DirectX::SimpleMath::Vector4 camPos;
//
//	DirectX::SimpleMath::Vector4 ambient;		// ¶óÀÌÆ® Á¤º¸
//	DirectX::SimpleMath::Vector4 diffuse;
//	DirectX::SimpleMath::Vector4 specular;
//
//	DirectX::SimpleMath::Vector4 Matambient;	// ¸ÞÅ×¸®¾ó Á¤º¸
//	DirectX::SimpleMath::Vector4 Matdiffuse;
//	DirectX::SimpleMath::Vector4 Matspecular;
//
//	int shiness;
//	DirectX::SimpleMath::Vector3 padding;
//};

struct BoneBuffer
{
	DirectX::SimpleMath::Matrix boneMat[BONE_MAXSIZE] = { DirectX::SimpleMath::Matrix::Identity, };
};