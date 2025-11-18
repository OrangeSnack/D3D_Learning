#include "SkeletalMesh.h"
#include <iostream>
#include <directxtk/WICTextureLoader.h>
#include "../BaseEngine/Helper.h"
#include <queue>
#include <stack>
#include "../BaseEngine/TimeSystem.h"
#include <d3dcompiler.h>

using namespace DirectX::SimpleMath;
using namespace DirectX;

SkeletalMesh::~SkeletalMesh()
{
	for (auto& buffer : m_pVertexBuffer) SAFE_RELEASE(buffer); m_pVertexBuffer.clear();
	for (auto& buffer : m_pIndexBuffer) SAFE_RELEASE(buffer); m_pIndexBuffer.clear();
}

void SkeletalMesh::Update()
{
	if (animIdx >= 0 && scene->HasAnimations()) {
		if (isPlaying) {
			currTime += GameTimer::m_Instance->DeltaTime();
			animTime = currTime * animations[animIdx]->mTicksPerSecond;

			if (animTime > (float)animations[animIdx]->mDuration) {
				if (isLooping) {
					currTime -= (float)animations[animIdx]->mDuration / animations[animIdx]->mTicksPerSecond;
				}
				else
					isPlaying = false;
			}

			UpdateBoneMat();
		}
	}
}

bool SkeletalMesh::LoadFile(std::wstring _filePath)
{
	// 파일경로 저장
	std::filesystem::path p = _filePath.c_str();

	if (!p.has_filename())
		return false;	// 파일이 없다는 에러문구도 출력?
	filePath = p.parent_path();

	scene = importer.ReadFile(p.string(), importFlags);

	

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::string error = importer.GetErrorString();
		std::cerr << "Assimp Error: " << importer.GetErrorString() << std::endl;
		return false;
	}

	// 노드탐색하며 월드매트릭스 만들기
	UpdateBoneMat();

	// 애니메이션 로딩
	LoadAnimations(animations, scene);

	// 메테리얼 로딩
	LoadMaterials(m_nMaterials, this);

	// 버텍스, 인덱스 로딩
	std::vector<std::vector<Vertex>> modelVertices;
	std::vector<std::vector<UINT>> modelIndices;

	for (UINT i = 0; i < scene->mNumMeshes; i++) {
		std::vector<Vertex> tempV;
		std::vector<UINT> tempI;
		LoadVertex(&tempV, scene->mMeshes[i]);
		LoadSkinInfo(tempV, scene->mMeshes[i]);
		LoadIndex(&tempI, scene->mMeshes[i]);
		NormalizeWeights(tempV);

		modelVertices.push_back(tempV);
		modelIndices.push_back(tempI);
	}

	// 모델 버퍼 생성.
	// Vertex
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData = {};

	for (int i = 0; i < modelVertices.size(); i++) {
		bd.ByteWidth = UINT(sizeof(Vertex) * modelVertices[i].size());
		vbData.pSysMem = modelVertices[i].data();

		ID3D11Buffer* tempBuffer = nullptr;
		HR_T(m_pDevice->CreateBuffer(&bd, &vbData, &tempBuffer));

		if (tempBuffer != nullptr)
			m_pVertexBuffer.push_back(tempBuffer);
	}

	// Index
	bd = {};
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData = {};


	for (int i = 0; i < modelIndices.size(); i++) {
		bd.ByteWidth = UINT(sizeof(UINT) * modelIndices[i].size());
		ibData.pSysMem = modelIndices[i].data();

		ID3D11Buffer* tempBuffer = nullptr; 
		HR_T(m_pDevice->CreateBuffer(&bd, &ibData, &tempBuffer));

		if (tempBuffer != nullptr)
			m_pIndexBuffer.push_back(tempBuffer);
	}

	// 버텍스 버퍼 바인딩.
	m_VertexBufferStride = sizeof(Vertex);
	m_VertexBufferOffset = 0;

	for (const auto& indices : modelIndices)
		m_nIndices.push_back(UINT(indices.size()));

	// 오프셋 버퍼 생성 & 바인딩
	ID3D11DeviceContext* tempContext = nullptr;
	m_pDevice->GetImmediateContext(&tempContext);
	BoneBuffer offsetBuffer;

	for (int i = 0; i < BONE_MAXSIZE; i++) {
		offsetBuffer.boneMat[i] = boneOffsetMat[i];
	}

	boneIndex;
	boneMat;

	bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(BoneBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pOffsetBuffer));

	tempContext->UpdateSubresource(m_pOffsetBuffer, 0, nullptr, &offsetBuffer, 0, 0);
	tempContext->VSSetConstantBuffers(3, 1, &m_pOffsetBuffer);

	// 버텍스 쉐이더 생성
	D3D_SHADER_MACRO macros[] = {
		{"VERTEX_SKINNING", "1"},
		{nullptr, nullptr}
	};

	ID3D10Blob* vertexShader = nullptr;
	ID3D10Blob* errormsg = nullptr;

	HR_T(D3DCompileFromFile(
		L"SkeletalVertexShader.hlsl",
		macros,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0, 0,
		&vertexShader,
		&errormsg));

	if (errormsg) {
		OutputDebugStringA((char*)errormsg->GetBufferPointer());
		errormsg->Release();
	}
	else {
		OutputDebugStringA("Shader compile failed with no error message.");
	}


	HR_T(m_pDevice->CreateVertexShader(
		vertexShader->GetBufferPointer(), 
		vertexShader->GetBufferSize(), 
		nullptr, 
		&m_pVertexShader));

	return true;
}

bool SkeletalMesh::PlayAnim(int _animIdx)
{
	if (_animIdx >= 0 && _animIdx < animations.size()) {
		isPlaying = true;
		animTime = 0.0f;
		if (!(_animIdx == animIdx)) {
			animIdx = _animIdx;
			LoadNodeAnim(&nodeAnimMap, animations[_animIdx]);
		}
		return true;
	}
	else {
		return false;
	}
}

bool SkeletalMesh::SetLoop(bool _val)
{
	isLooping = _val;
	return _val;
}

bool SkeletalMesh::StopAnim()
{
	isPlaying = false;
	return true;
}

bool SkeletalMesh::LoadVertex(std::vector<Vertex>* _vertices, const aiMesh* _mesh)
{
	for (UINT i = 0; i < _mesh->mNumVertices; i++) {
		Vertex v;

		// 버텍스 포지션
		v.Pos = Vector3(_mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z);

		// 버텍스 노멀
		if (_mesh->HasNormals()) {
			v.Normal = Vector3(_mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z);
		}

		// 버텍스 탄젠트
		if (_mesh->HasTangentsAndBitangents()) {
			v.Tangent = Vector3(_mesh->mTangents[i].x, _mesh->mTangents[i].y, _mesh->mTangents[i].z);
			v.BiTangent = Vector3(_mesh->mBitangents[i].x, _mesh->mBitangents[i].y, _mesh->mBitangents[i].z);
		}

		// 버텍스 UV좌표
		if (_mesh->HasTextureCoords(0)) {
			v.Tex = Vector2(_mesh->mTextureCoords[0][i].x, _mesh->mTextureCoords[0][i].y);
		}

		_vertices->push_back(v);
	}

	return true;
}

bool SkeletalMesh::LoadSkinInfo(std::vector<Vertex>& _vertices, const aiMesh* _mesh)
{
	// 버텍스 본
	if (_mesh->HasBones()) {
		for (int i = 0; i < _mesh->mNumBones; i++) {
			auto bone = _mesh->mBones[i];

			// 본 오프셋 저장 (기생코드)
			if (boneIndex.find(bone->mName.C_Str()) != boneIndex.end())
				boneOffsetMat[boneIndex[bone->mName.C_Str()]] = ConvertMat(bone->mOffsetMatrix);

			// 버텍스별 본인덱스, 웨이트 집어넣기
			for (int j = 0; j < bone->mNumWeights; j++) {
				UINT id = bone->mWeights[j].mVertexId;
				float weight = bone->mWeights[j].mWeight;

				float totalWeight = 0.0f;

				// 뽑은 index 정보를 통해 버텍스배열 접근
				for (int k = 0; k < 4; k++) {
					// 가중치 더하기
					totalWeight += _vertices[id].boneWeights[k];

					// 비어있을시 집어넣기
					if (_vertices[id].boneIndices[k] == -1) {
						_vertices[id].boneIndices[k] = boneIndex[bone->mName.C_Str()];
						_vertices[id].boneWeights[k] = weight;
						totalWeight += weight;
						break;
					}
				}
			}
		}
	}
	return true;
}

bool SkeletalMesh::NormalizeWeights(std::vector<Vertex>& _vertices)
{
	for (auto& vertex : _vertices) {
		float totalWeight = 0.0f;

		for (float boneWeight : vertex.boneWeights) {
			totalWeight += boneWeight;
		}

		for (float& boneWeight : vertex.boneWeights) {
			boneWeight = boneWeight / totalWeight;
		}
	}

	return true;
}

bool SkeletalMesh::LoadIndex(std::vector<UINT>* _indices, const aiMesh* _mesh)
{
	for (UINT i = 0; i < _mesh->mNumFaces; i++) {
		for (UINT j = 0; j < _mesh->mFaces[i].mNumIndices; j++)
			_indices->push_back(UINT(_mesh->mFaces[i].mIndices[j]));
	}

	return true;
}

bool SkeletalMesh::LoadMaterials(std::vector<Materials>& _out, const SkeletalMesh* _model)
{
	const aiScene* scene = _model->scene;

	if (!_model->scene->HasMaterials())
		return false;

	_out.resize(_model->scene->mNumMaterials);

	for (UINT i = 0; i < _model->scene->mNumMaterials; i++) {
		// TODO :: 메테리얼으로 ResourceView 만들기
		aiString aiStr;
		aiMaterial* aiMat = scene->mMaterials[i];
		
		// 베이스 컬러
		aiColor4D baseColor;
		if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor) == AI_SUCCESS) {
			_out[i].BaseColor = { baseColor.r, baseColor.g, baseColor.b, baseColor.a };
		}
		else if (aiMat->Get(AI_MATKEY_BASE_COLOR, baseColor) == AI_SUCCESS) {
			_out[i].BaseColor = { baseColor.r, baseColor.g, baseColor.b, baseColor.a };
		}

		// 디퓨즈
		if (aiMat->GetTextureCount(aiTextureType_DIFFUSE)) {
			if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _model->filePath / p.filename();

				HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].diffuse));
			}
		}
		// 디퓨즈가 없으면?
		if (_out[i].diffuse == nullptr) {
			HR_T(CreateWICTextureFromFile(m_pDevice, defaultDiffuse.c_str(), nullptr, &_out[i].diffuse));
		}


		// 스페큘러
		if (aiMat->GetTextureCount(aiTextureType_SPECULAR)) {
			if (aiMat->GetTexture(aiTextureType_SPECULAR, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _model->filePath / p.filename();

				HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].specular));
			}
		}

		// 노말
		if (aiMat->GetTextureCount(aiTextureType_NORMALS)) {
			if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _model->filePath / p.filename();

				HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].normal));
			}
		}

		// 이미션
		if (aiMat->GetTextureCount(aiTextureType_EMISSIVE)) {
			if (aiMat->GetTexture(aiTextureType_EMISSIVE, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _model->filePath / p.filename();

				HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].emissive));
			}
		}
	}

	return true;
}

bool SkeletalMesh::LoadAnimations(std::vector<aiAnimation*>& _out, const aiScene* _scene)
{
	if (!_scene->HasAnimations())
		return false;

	for (int i = 0; i < _scene->mNumAnimations; i++) {
		_out.push_back(_scene->mAnimations[i]);
	}

	return true;
}

bool SkeletalMesh::LoadNodeAnim(std::unordered_map<std::string, aiNodeAnim*>* _out, const aiAnimation* _anim)
{
	_out->clear();

	for (int i = 0; i < _anim->mNumChannels; i++) {
		std::string nodeName = _anim->mChannels[i]->mNodeName.C_Str();
		nodeAnimMap[nodeName] = _anim->mChannels[i];
	}

	return true;
}

bool SkeletalMesh::UpdateBoneMat()
{
	// 노드탐색하며 월드매트릭스 만들기
	std::stack<aiNode*> nodes;
	nodes.push(scene->mRootNode);

	int nodeIdx = 0;

	while (!nodes.empty()) {
		aiNode* temp = nodes.top();
		nodes.pop();

		aiNode* currPar = temp->mParent;
		aiMatrix4x4 tempMat = temp->mTransformation;

		auto it = nodeAnimMap.find(temp->mName.C_Str());
		if (it != nodeAnimMap.end()) {

			// 적용될 키인덱스 구하기
			const auto animNode = it->second;



			int currPosIdx = FindKeyIndex(animNode->mPositionKeys, animNode->mNumPositionKeys, animTime);
			int currRotIdx = FindKeyIndex(animNode->mRotationKeys, animNode->mNumRotationKeys, animTime);
			int currScaIdx = FindKeyIndex(animNode->mScalingKeys, animNode->mNumScalingKeys, animTime);

			// 보간값 구하기
			int nextIdx[3] = {
				currPosIdx == animNode->mNumPositionKeys - 1 ? currPosIdx : currPosIdx + 1,
				currRotIdx == animNode->mNumRotationKeys - 1 ? currRotIdx : currRotIdx + 1,
				currScaIdx == animNode->mNumScalingKeys - 1 ? currScaIdx : currScaIdx + 1
				};

			aiVectorKey pos = Evaluate(animNode->mPositionKeys[currPosIdx], animNode->mPositionKeys[nextIdx[0]], animTime);
			aiQuatKey rot = Evaluate(animNode->mRotationKeys[currRotIdx], animNode->mRotationKeys[nextIdx[1]], animTime);
			aiVectorKey sca = Evaluate(animNode->mScalingKeys[currScaIdx], animNode->mScalingKeys[nextIdx[2]], animTime);

			// 매트릭스 만들기
			aiMatrix4x4 matScale, matRot, matTrans;

			aiMatrix4x4::Translation(pos.mValue, matTrans);
			matRot = aiMatrix4x4(rot.mValue.GetMatrix());
			aiMatrix4x4::Scaling(sca.mValue, matScale);

			tempMat = matTrans * matRot * matScale;		// 열우선 매트릭스라 TRS 순서로 곱해야함
			INT a = 0;
			// TODO :: 본의 aiVertexWeight::mVertexId와 mWeight를 버텍스 셰이더에 보내기. - 했음
		}

		// 부모 매트릭스 곱하기
		if (currPar != nullptr) {
			tempMat = nodeWorldMap[currPar->mName.C_Str()] * tempMat;
		}

		// 매트릭스 맵에 저장하기
		nodeWorldMap[temp->mName.C_Str()] = tempMat;
		boneIndex[temp->mName.C_Str()] = nodeIdx;
		boneMat[nodeIdx] = ConvertMat(tempMat);
		nodeIdx++;

		// TODO :: 본 월드매트릭스 업데이트 해주기 ( 노드수 128개 상정하고 상수버퍼에 집어넣을것!) - 했음 보내줘야함 이제
		//	그 다음은 버텍스 쉐이더에서 작업이다!! (아직 버텍스 구조체 수정한거 CB에 반영안함)

		// 자식노드 추가
		for (int i = 0; i < temp->mNumChildren; i++) {
			nodes.push(temp->mChildren[i]);
		}
	}

	return true;
}

int SkeletalMesh::FindKeyIndex(const aiVectorKey* _keys, int _size, float _animTime)
{
	for (int i = 0; i < _size - 1; ++i) {
		if (_animTime < _keys[i + 1].mTime)
			return i;
	}
	return _size - 1; // 마지막 구간
}

int SkeletalMesh::FindKeyIndex(const aiQuatKey* _keys, int _size, float _animTime)
{
	for (int i = 0; i < _size - 1; ++i) {
		if (_animTime < _keys[i + 1].mTime)
			return i;
	}
	return _size - 1; // 마지막 구간
}

aiVectorKey SkeletalMesh::Evaluate(const aiVectorKey& _k1, const aiVectorKey& _k2, float _currTime) {
	if (_k1 == _k2)
		return _k1;

	float lerpTime = (_currTime - _k1.mTime) / (_k2.mTime - _k1.mTime);

	return {
		_currTime,
		_k1.mValue + (_k2.mValue - _k1.mValue) * lerpTime
	};
}

aiQuatKey SkeletalMesh::Evaluate(const aiQuatKey& _k1, const aiQuatKey& _k2, float _currTime)
{
	if (_k1 == _k2)
		return _k1;

	float lerpTime = (_currTime - _k1.mTime) / (_k2.mTime - _k1.mTime);

	aiQuaternion temp;
	aiQuaternion::Interpolate(temp, _k1.mValue, _k2.mValue, lerpTime);
	return {
		_currTime,
		temp };
}


bool SkeletalMesh::SetResources(MaterialBuffer* _matBuffer, BoneBuffer* _boneBuffer)
{
	if (_matBuffer == nullptr || _boneBuffer == nullptr)
		return false;

	matBuff = _matBuffer;
	boneBuff = _boneBuffer;

	return true;
}

bool SkeletalMesh::Draw(ID3D11DeviceContext* _deviceContext, ID3D11Buffer** _bufferList, UINT _boneBuffIdx, UINT _matBuffIdx, ID3D11PixelShader* _shader /*= nullptr*/, bool _useMat /*= true*/)
{
	// 쉐이더 업데이트
	if (_shader)
		_deviceContext->PSSetShader(_shader, nullptr, 0);

	ID3D11VertexShader* defaultShader = nullptr;
	_deviceContext->VSGetShader(&defaultShader, nullptr, nullptr);
	_deviceContext->VSSetShader(m_pVertexShader, nullptr, 0);

	// 매트릭스 업데이트
	for (int i = 0; i < BONE_MAXSIZE; i++) {
		boneBuff->boneMat[i] = boneMat[i];
	}

	// 본버퍼 업데이트
	_deviceContext->UpdateSubresource(_bufferList[_boneBuffIdx], 0, nullptr, boneBuff, 0, 0);

	// 노드에 따라 인덱스 불러와서 업데이트
	std::stack<aiNode*> nodes;
	nodes.push(scene->mRootNode);
	int nodeIdx = 0;

	while (!nodes.empty()) {
		aiNode* node = nodes.top();
		nodes.pop();

		// 메시 그리기
		for (int i = 0; i < node->mNumMeshes; i++) {
			int meshIdx = node->mMeshes[i];
			_deviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer[meshIdx], &m_VertexBufferStride, &m_VertexBufferOffset);
			_deviceContext->IASetIndexBuffer(m_pIndexBuffer[meshIdx], DXGI_FORMAT_R32_UINT, 0);

			if (_useMat) {
				UINT matIdx = scene->mMeshes[meshIdx]->mMaterialIndex;
				modelRV[0] = m_nMaterials[matIdx].diffuse;
				modelRV[1] = m_nMaterials[matIdx].normal;
				modelRV[2] = m_nMaterials[matIdx].specular;
				modelRV[3] = m_nMaterials[matIdx].emissive;
				matBuff->Matdiffuse = m_nMaterials[matIdx].BaseColor;

				_deviceContext->PSSetShaderResources(0, 4, modelRV);
				_deviceContext->UpdateSubresource(_bufferList[_matBuffIdx], 0, nullptr, matBuff, 0, 0);
			}

			//Matrix nodeMat = ConvertMat(nodeWorldMap[node->mName.C_Str()]);

			//_cb->skinMat = nodeMat;
			//_cb->skinNorm = XMMatrixInverse(nullptr, XMMatrixTranspose(nodeMat));
			//_deviceContext->UpdateSubresource(_cbBuff, 0, nullptr, _cb, 0, 0);

			_deviceContext->DrawIndexed(m_nIndices[meshIdx], 0, 0);
		}

		// 차일드 큐에 넣기
		for (int i = 0; i < node->mNumChildren; i++) {
			nodes.push(node->mChildren[i]);
		}

		nodeIdx++;
	}

	// 쉐이더 돌려놓기
	_deviceContext->VSSetShader(defaultShader, nullptr, 0);

	return true;
}

DirectX::SimpleMath::Matrix SkeletalMesh::ConvertMat(const aiMatrix4x4& aiMat)
{
	return {
		aiMat.a1, aiMat.a2, aiMat.a3, aiMat.a4,
		aiMat.b1, aiMat.b2, aiMat.b3, aiMat.b4,
		aiMat.c1, aiMat.c2, aiMat.c3, aiMat.c4,
		aiMat.d1, aiMat.d2, aiMat.d3, aiMat.d4
	};
}
