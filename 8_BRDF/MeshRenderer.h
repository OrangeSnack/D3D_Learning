#pragma once
#include <unordered_map>
#include "Component.h"
#include "framework.h"

class StaticMesh;
class MeshRenderer : public Component
{
public:
	// GPU 버퍼
	std::shared_ptr<StaticMesh> mesh = nullptr;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> vertexBuffers;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> indexBuffer;
	std::vector<UINT> nIndices;

	std::unordered_map<UINT, std::vector<aiMesh*>> meshGroup;
	std::unordered_map<UINT, std::weak_ptr<Renderer>> rendererGroup;

	// 렌더링 옵션
	UINT vertexBufferStride = 0;
	UINT vertexBufferOffset = 0;
	bool castShadows = true;
	bool receiveShadows = true;

	void SetMesh(std::shared_ptr<StaticMesh>& _mesh);
	void Start() override;
	void Update() override;

	~MeshRenderer();
};

