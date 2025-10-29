#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
#include <d3d11_4.h>
#include <directxtk/SimpleMath.h>
#include "Shared.h"

class SkeletalMesh
{
private:
	ID3D11Device* m_pDevice = nullptr;

	Assimp::Importer importer;
	unsigned int importFlags =
		aiProcess_Triangulate |				// vertex 삼각형 으로 출력
		aiProcess_GenNormals |				// Normal 정보 생성  
		aiProcess_GenUVCoords |				// 텍스처 좌표 생성
		aiProcess_CalcTangentSpace |		// 탄젠트 벡터 생성
		aiProcess_ConvertToLeftHanded;		// DX용 왼손좌표계 변환

	std::wstring defaultDiffuse = L"../Resources/Texture/Default/Solid_White.png";

	const aiScene* scene = nullptr;
	std::filesystem::path filePath;

public:
	SkeletalMesh(ID3D11Device* _device) : m_pDevice(_device) {
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);
	};
	~SkeletalMesh();

public:
	std::vector<Materials> m_nMaterials;				// 모델 메테리얼
	std::vector<ID3D11Buffer*> m_pVertexBuffer;			// 모델 버텍스 버퍼
	std::vector<ID3D11Buffer*> m_pIndexBuffer;			// 모델 인덱스 버퍼
	std::vector<UINT> m_nIndices;						// 인덱스 개수
	std::vector<aiMatrix4x4> nodeWorld;						// 노드별 월드매트릭스

	ID3D11ShaderResourceView* modelRV[5] = { nullptr, };	// 메테리얼
	ID3D11PixelShader* m_pDiffuseShader = nullptr;			// 디퓨즈 전용 쉐이더

	UINT m_VertexBufferStride = 0;		// 버텍스 하나의 크기
	UINT m_VertexBufferOffset = 0;		// 버텍스 버퍼의 오프셋
public:
	bool LoadFile(std::wstring _filePath);
	bool LoadVertex(std::vector<Vertex>* _vertices, const aiMesh* _mesh);
	bool LoadIndex(std::vector<UINT>* _indices, const aiMesh* _mesh);
	bool LoadMaterials(std::vector<Materials>& _out, const SkeletalMesh* _model);
	bool Draw(ID3D11DeviceContext* _deviceContext, ID3D11Buffer* _cbBuff, ConstantBuffer* _cb, ID3D11PixelShader* _shader = nullptr, bool _useMat = true);
	DirectX::SimpleMath::Matrix ConvertMat(const aiMatrix4x4& aiMat);
};

