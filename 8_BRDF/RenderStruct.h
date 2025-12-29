#pragma once

#include "framework.h"

#define BONE_MAXSIZE 256

enum RenderType {
	SKYBOX = 0,
	PREDEPTH = 1,
	PBR = 2,
	PHONG = 3,
	TRANSCULANT = 4,
	POSTPROCESS = 5,
	UI = 6
};

struct Mesh_SkinVertex
{
	DirectX::SimpleMath::Vector3 Pos;		// 정점 위치 정보
	DirectX::SimpleMath::Vector3 Normal;	// 노멀
	DirectX::SimpleMath::Vector3 Tangent;	// 탄젠트
	DirectX::SimpleMath::Vector3 BiTangent;	// Bi탄젠트
	DirectX::SimpleMath::Vector2 Tex;		// 텍스쳐 UV
	int boneIndices[4] = { -1, -1, -1, -1 };				// 버텍스와 연결된 본들의 인덱스
	float boneWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };		// 각 본들의 가중치
};

struct Mesh_BoneBuffer
{
	DirectX::SimpleMath::Matrix boneMat[BONE_MAXSIZE] = { DirectX::SimpleMath::Matrix::Identity, };
};

struct Render_CamBuffer {
	DirectX::SimpleMath::Matrix mView = DirectX::SimpleMath::Matrix::Identity;			// 카메라좌표계 변환행렬
	DirectX::SimpleMath::Matrix mProjection = DirectX::SimpleMath::Matrix::Identity;	// ndc좌표계 변환행렬
	DirectX::SimpleMath::Vector4 camPos;
};

struct Render_TransformBuffer
{
	DirectX::SimpleMath::Matrix mWorld;
	DirectX::SimpleMath::Matrix mNormalMatrix;
};

struct Render_LightBuffer {
	DirectX::SimpleMath::Vector4 vLightDir;
	DirectX::SimpleMath::Vector4 vLightColor;
};

struct Render_MaterialBuffer
{
	DirectX::SimpleMath::Vector4 Matambient;	// 메테리얼 정보
	DirectX::SimpleMath::Vector4 Matdiffuse;
	DirectX::SimpleMath::Vector4 Matspecular;

	int shiness;
	DirectX::SimpleMath::Vector3 padding;
};

struct Render_ShadowBuffer
{
	DirectX::SimpleMath::Matrix ShadowView;
	DirectX::SimpleMath::Matrix ShadowProjection;
};