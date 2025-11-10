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

	// 디버깅용으로 public 설정함. 원래는 private으로 설정해야함
public:
	bool isPlaying = false;
	bool isLooping = true;
	float currTime = 0.0f;
	float animTime = 0.0f;
	int animIdx = -1;
	int currPosIdx = -1;
	int currRotIdx = -1;
	int currScaIdx = -1;

public:
	SkeletalMesh(ID3D11Device* _device) : m_pDevice(_device) {
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);
	};
	~SkeletalMesh();

public:
	std::vector<Materials> m_nMaterials;				// 모델 메테리얼
	std::vector<ID3D11Buffer*> m_pVertexBuffer;			// 모델 버텍스 버퍼
	std::vector<ID3D11Buffer*> m_pIndexBuffer;			// 모델 인덱스 버퍼
	std::vector<UINT> m_nIndices;						// 모델 인덱스 개수
	//std::vector<aiMatrix4x4> nodeWorld;					// 노드별 월드매트릭스
	ID3D11ShaderResourceView* modelRV[5] = { nullptr, };	// 메테리얼
	ID3D11PixelShader* m_pAlphaClipShader = nullptr;			// 디퓨즈 전용 쉐이더

	std::vector<aiAnimation*> animations;						// 애니메이션 모음집
	std::unordered_map<std::string, aiNodeAnim*> nodeAnimMap;	// 이름기반 노드맵
	DirectX::SimpleMath::Matrix boneMat[BONE_MAXSIZE];			// 본 매트릭스 모음집
	DirectX::SimpleMath::Matrix boneOffsetMat[BONE_MAXSIZE];	// 본 오프셋 매트릭스 모음집
	std::unordered_map<std::string, UINT> boneIndex;			// 본 인덱스
	std::unordered_map<std::string, aiMatrix4x4> nodeWorldMap;	// 노드별 월드맵

	ID3D11Buffer* m_pOffsetBuffer = nullptr;
	MaterialBuffer* matBuff = nullptr;
	BoneBuffer* boneBuff = nullptr;

	UINT m_VertexBufferStride = 0;		// 버텍스 하나의 크기
	UINT m_VertexBufferOffset = 0;		// 버텍스 버퍼의 오프셋
protected:
	bool LoadVertex(std::vector<Vertex>* _vertices, const aiMesh* _mesh);
	bool LoadSkinInfo(std::vector<Vertex>& _vertices, const aiMesh* _mesh);
	bool LoadIndex(std::vector<UINT>* _indices, const aiMesh* _mesh);
	bool LoadMaterials(std::vector<Materials>& _out, const SkeletalMesh* _model);
	bool LoadAnimations(std::vector<aiAnimation*>& _out, const aiScene* _scene);
	bool LoadNodeAnim(std::unordered_map<std::string, aiNodeAnim*>* _out, const aiAnimation* _anim);
	
	bool UpdateBoneMat();
	int FindKeyIndex(const aiVectorKey* _keys, int _size, float _animTime);
	int FindKeyIndex(const aiQuatKey* _keys, int _size, float _animTime);

	aiVectorKey Evaluate(const aiVectorKey& _k1, const aiVectorKey& _k2, float _currTime);
	aiQuatKey Evaluate(const aiQuatKey& _k1, const aiQuatKey& _k2, float _currTime);
public:
	void Update();

	bool LoadFile(std::wstring _filePath);
	bool PlayAnim(int _animIdx);
	bool SetLoop(bool _val);
	bool StopAnim();
	bool SetResources(MaterialBuffer* _matBuffer, BoneBuffer* _boneBuffer);
	bool Draw(ID3D11DeviceContext* _deviceContext, ID3D11Buffer** _bufferList, UINT _boneBuffIdx, UINT _matBuffIdx, ID3D11PixelShader* _shader = nullptr, bool _useMat = true);

	DirectX::SimpleMath::Matrix ConvertMat(const aiMatrix4x4& aiMat);
};

