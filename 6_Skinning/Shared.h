#pragma once

#include <d3d11_4.h>
#include <directxtk/SimpleMath.h>

#define BONE_MAXSIZE 256

struct Materials {
	ID3D11ShaderResourceView* diffuse = nullptr;		// 디퓨즈맵
	ID3D11ShaderResourceView* specular = nullptr;		// 스펙큘러맵
	ID3D11ShaderResourceView* normal = nullptr;			// 노멀맵
	ID3D11ShaderResourceView* emissive = nullptr;		// 발광맵
	DirectX::SimpleMath::Color BaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };	// 베이스 컬러
};
struct Vertex
{
	DirectX::SimpleMath::Vector3 Pos;		// 정점 위치 정보
	DirectX::SimpleMath::Vector3 Normal;	// 노멀
	DirectX::SimpleMath::Vector3 Tangent;	// 탄젠트
	DirectX::SimpleMath::Vector3 BiTangent;	// Bi탄젠트
	DirectX::SimpleMath::Vector2 Tex;		// 텍스쳐 UV
	UINT boneIndices[4] = { UINT_MAX, };					// 버텍스와 연결된 본들의 인덱스
	float boneWeights[4] = { 0.0f, };		// 각 본들의 가중치
};

struct ConstantBuffer
{
	DirectX::SimpleMath::Matrix mWorld;
	DirectX::SimpleMath::Matrix mView;
	DirectX::SimpleMath::Matrix mProjection;
	DirectX::SimpleMath::Matrix mNormalMatrix;
};

struct MaterialBuffer
{
	DirectX::SimpleMath::Vector4 vLightDir;
	DirectX::SimpleMath::Vector4 vLightColor;

	DirectX::SimpleMath::Vector4 camPos;

	DirectX::SimpleMath::Vector4 ambient;		// 라이트 정보
	DirectX::SimpleMath::Vector4 diffuse;
	DirectX::SimpleMath::Vector4 specular;

	DirectX::SimpleMath::Vector4 Matambient;	// 메테리얼 정보
	DirectX::SimpleMath::Vector4 Matdiffuse;
	DirectX::SimpleMath::Vector4 Matspecular;

	int shiness;
	DirectX::SimpleMath::Vector3 padding;
};

struct BoneBuffer
{
	DirectX::SimpleMath::Matrix boneMat[BONE_MAXSIZE] = { DirectX::SimpleMath::Matrix::Identity, };
};