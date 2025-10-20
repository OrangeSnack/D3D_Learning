#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
public:
	Assimp::Importer importer;

	unsigned int importFlags =
		aiProcess_Triangulate |				// vertex 삼각형 으로 출력
		aiProcess_GenNormals |				// Normal 정보 생성  
		aiProcess_GenUVCoords |				// 텍스처 좌표 생성
		aiProcess_CalcTangentSpace |		// 탄젠트 벡터 생성
		aiProcess_MakeLeftHanded |		// DX용 왼손좌표계 변환
		aiProcess_FlipUVs |
		aiProcess_FlipWindingOrder |
		aiProcess_PreTransformVertices;		// 노드의 변환행렬을 적용한 버텍스 생성한다.  *StaticMesh로 처리할때만

	const aiScene* scene = nullptr;

public:
	bool LoadFile(std::string _filePath);
};

