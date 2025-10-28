#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
#include <d3d11_4.h>
#include <directxtk/SimpleMath.h>

using namespace DirectX::SimpleMath;
using namespace DirectX;

struct Materials {
	ID3D11ShaderResourceView* diffuse = nullptr;		// 디퓨즈맵
	ID3D11ShaderResourceView* specular = nullptr;		// 스펙큘러맵
	ID3D11ShaderResourceView* normal = nullptr;			// 노멀맵
	ID3D11ShaderResourceView* emissive = nullptr;		// 발광맵
};
struct Vertex
{
	Vector3 Pos;		// 정점 위치 정보
	Vector3 Normal;		// 노멀
	Vector3 Tangent;	// 탄젠트
	Vector3 BiNormal;	// Bi노멀
	Vector2 Tex;		// 텍스쳐 UV
};

class StaticMesh
{
private:
	ID3D11Device* m_pDevice = nullptr;

	Assimp::Importer importer;
	unsigned int importFlags =
		aiProcess_Triangulate |				// vertex 삼각형 으로 출력
		aiProcess_GenNormals |				// Normal 정보 생성  
		aiProcess_GenUVCoords |				// 텍스처 좌표 생성
		aiProcess_CalcTangentSpace |		// 탄젠트 벡터 생성
		aiProcess_ConvertToLeftHanded |		// DX용 왼손좌표계 변환
		aiProcess_PreTransformVertices;		// 노드의 변환행렬을 적용한 버텍스 생성한다.  *StaticMesh로 처리할때만

	const aiScene* scene = nullptr;
	std::filesystem::path filePath;

public:
	StaticMesh(ID3D11Device* _device) : m_pDevice(_device) {};
	~StaticMesh();

public:
	std::vector<Materials> m_pMaterials;				// 모델 메테리얼
	std::vector<ID3D11Buffer*> m_pVertexBuffer;			// 모델 버텍스 버퍼
	std::vector<ID3D11Buffer*> m_pIndexBuffer;			// 모델 인덱스 버퍼
	std::vector<UINT> m_nIndices;						// 인덱스 개수

	ID3D11ShaderResourceView* modelRV[5] = { nullptr, };	// 메테리얼
	ID3D11PixelShader* m_pDiffuseShader = nullptr;			// 디퓨즈 전용 쉐이더

	UINT m_VertexBufferStride = 0;		// 버텍스 하나의 크기
	UINT m_VertexBufferOffset = 0;		// 버텍스 버퍼의 오프셋
public:
	bool LoadFile(std::wstring _filePath);
	bool LoadVertex(std::vector<Vertex>* _vertices, const aiMesh* _mesh);
	bool LoadIndex(std::vector<UINT>* _indices, const aiMesh* _mesh);
	bool LoadMaterials(std::vector<Materials>& _out, const StaticMesh* _model);

	bool Draw(ID3D11DeviceContext* _deviceContext , ID3D11PixelShader* _shader, bool _useMat = true);
};

