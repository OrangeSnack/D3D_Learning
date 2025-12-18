#pragma once
#include "framework.h"
#include <assimp/scene.h>
#include "GameResource.h"
#include "ResourceShared.h"

class StaticMesh : public GameResource
{
public:
	std::shared_ptr<MeshData> meshData;		// 메시 데이터
	std::shared_ptr<MeshGPU> gpuBuffer;		// GPU 버퍼
	std::unordered_map<UINT, std::vector<UINT>> meshGroupData;	// 메시 그룹 <MatIdx, MeshIdx>
	bool castShadows = true;
	bool receiveShadows = true;
};

