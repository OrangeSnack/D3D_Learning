#pragma once
#include "framework.h"
#include "Material.h"

struct Mesh_Vertex
{
	DirectX::SimpleMath::Vector3 Pos;		// Á¤Á¡ À§Ä¡ Á¤º¸
	DirectX::SimpleMath::Vector3 Normal;	// ³ë¸Ö
	DirectX::SimpleMath::Vector3 Tangent;	// ÅºÁ¨Æ®
	DirectX::SimpleMath::Vector3 BiTangent;	// BiÅºÁ¨Æ®
	DirectX::SimpleMath::Vector2 Tex;		// ÅØ½ºÃÄ UV
};

struct MeshData {
	std::vector<std::vector<Mesh_Vertex>> vertices;
	std::vector<std::vector<UINT>> indices;
	std::vector<std::shared_ptr<Material>> materials;
};

struct MeshGPU {
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> vertexBuffers;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> indexBuffers;
};
