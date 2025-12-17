#pragma once
#include "framework.h"
#include "GameResource.h"
#include "RenderStruct.h"
#include "Material.h"
#include <assimp/scene.h>

struct Mesh_Vertex
{
	DirectX::SimpleMath::Vector3 Pos;		// Á¤Á¡ À§Ä¡ Á¤º¸
	DirectX::SimpleMath::Vector3 Normal;	// ³ë¸Ö
	DirectX::SimpleMath::Vector3 Tangent;	// ÅºÁ¨Æ®
	DirectX::SimpleMath::Vector3 BiTangent;	// BiÅºÁ¨Æ®
	DirectX::SimpleMath::Vector2 Tex;		// ÅØ½ºÃÄ UV
};

class Material;
class StaticMesh : public GameResource
{
public:
	std::vector<std::vector<Mesh_Vertex>> modelVertices;
	std::vector<std::vector<UINT>> modelIndices;
	std::vector<std::shared_ptr<Material>> modelMaterials;

	const aiScene* scene = nullptr;

	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> vertexBuffers;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> indexBuffer;
};

