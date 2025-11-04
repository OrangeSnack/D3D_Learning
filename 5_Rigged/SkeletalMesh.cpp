#include "SkeletalMesh.h"
#include <iostream>
#include <directxtk/WICTextureLoader.h>
#include "../BaseEngine/Helper.h"
#include <queue>
#include <stack>
#include "../BaseEngine/TimeSystem.h"

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

	// 버텍스, 인덱스 로딩
	std::vector<std::vector<Vertex>> modelVertices;
	std::vector<std::vector<UINT>> modelIndices;

	for (UINT i = 0; i < scene->mNumMeshes; i++) {
		std::vector<Vertex> tempV;
		std::vector<UINT> tempI;
		LoadVertex(&tempV, scene->mMeshes[i]);
		LoadIndex(&tempI, scene->mMeshes[i]);

		modelVertices.push_back(tempV);
		modelIndices.push_back(tempI);
	}

	// 메테리얼 로딩
	LoadMaterials(m_nMaterials, this);

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

		if (tempBuffer)
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

		if (tempBuffer)
			m_pIndexBuffer.push_back(tempBuffer);
	}

	// 버텍스 버퍼 바인딩.
	m_VertexBufferStride = sizeof(Vertex);
	m_VertexBufferOffset = 0;

	for (const auto& indices : modelIndices)
		m_nIndices.push_back(UINT(indices.size()));

	// 노드탐색하며 월드매트릭스 만들기
	UpdateBoneMat();

	// 애니메이션 로딩
	LoadAnimations(animations, scene);

	return true;
}

bool SkeletalMesh::PlayAnim(int _animIdx)
{
	if (_animIdx >= 0 && _animIdx < animations.size()) {
		isPlaying = true;
		currTime = 0.0f;
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

		v.Pos = Vector3(_mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z);

		if (_mesh->HasNormals()) {
			v.Normal = Vector3(_mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z);
		}

		if (_mesh->HasTangentsAndBitangents()) {
			v.Tangent = Vector3(_mesh->mTangents[i].x, _mesh->mTangents[i].y, _mesh->mTangents[i].z);
			v.BiNormal = Vector3(_mesh->mBitangents[i].x, _mesh->mBitangents[i].y, _mesh->mBitangents[i].z);
		}

		if (_mesh->HasTextureCoords(0)) {
			v.Tex = Vector2(_mesh->mTextureCoords[0][i].x, _mesh->mTextureCoords[0][i].y);
		}

		_vertices->push_back(v);
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

	while (!nodes.empty()) {
		aiNode* temp = nodes.top();
		nodes.pop();

		aiNode* currPar = temp->mParent;
		aiMatrix4x4 tempMat = temp->mTransformation;

		auto it = nodeAnimMap.find(temp->mName.C_Str());
		if (it != nodeAnimMap.end()) {

			// 적용될 키인덱스 구하기
			const auto animNode = it->second;
			currPosIdx = FindKeyIndex(animNode->mPositionKeys, animNode->mNumPositionKeys, animTime);
			currRotIdx = FindKeyIndex(animNode->mRotationKeys, animNode->mNumRotationKeys, animTime);
			currScaIdx = FindKeyIndex(animNode->mScalingKeys, animNode->mNumScalingKeys, animTime);

			// 보간값 구하기
			int nextIdx[3] = {
				currPosIdx == animNode->mNumPositionKeys ? currPosIdx : currPosIdx + 1,
				currRotIdx == animNode->mNumRotationKeys ? currRotIdx : currRotIdx + 1,
				currScaIdx == animNode->mNumScalingKeys ? currScaIdx : currScaIdx + 1
				};

			aiVectorKey pos = Evaluate(animNode->mPositionKeys[currPosIdx], animNode->mPositionKeys[nextIdx[0]], animTime);
			aiQuatKey rot = Evaluate(animNode->mRotationKeys[currRotIdx], animNode->mRotationKeys[nextIdx[1]], animTime);
			aiVectorKey sca = Evaluate(animNode->mScalingKeys[currScaIdx], animNode->mScalingKeys[nextIdx[2]], animTime);

			// 매트릭스 만들기
			aiMatrix4x4 matScale, matRot, matTrans;

			aiMatrix4x4::Translation(pos.mValue, matTrans);
			matRot = aiMatrix4x4(rot.mValue.GetMatrix());
			aiMatrix4x4::Scaling(sca.mValue, matScale);

			tempMat = matTrans * matRot * matScale;
		}

		// 부모 매트릭스 곱하기
		if (currPar != nullptr) {
			tempMat = nodeWorldMap[currPar->mName.C_Str()] * tempMat;
		}

		// 매트릭스 맵에 저장하기
		nodeWorldMap[temp->mName.C_Str()] = tempMat;

		// 자식노드 추가
		for (int i = 0; i < temp->mNumChildren; i++) {
			nodes.push(temp->mChildren[i]);
		}
	}

	return true;
}

int SkeletalMesh::FindKeyIndex(const aiVectorKey* keys, int size, float currTime)
{
	for (int i = 0; i < size - 1; ++i) {
		if (currTime < keys[i + 1].mTime)
			return i;
	}
	return size - 2; // 마지막 구간
}

int SkeletalMesh::FindKeyIndex(const aiQuatKey* keys, int size, float currTime)
{
	for (int i = 0; i < size - 1; ++i) {
		if (currTime < keys[i + 1].mTime)
			return i;
	}
	return size - 2; // 마지막 구간
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

bool SkeletalMesh::Draw(ID3D11DeviceContext* _deviceContext, ID3D11Buffer* _cbBuff, ConstantBuffer* _cb, ID3D11PixelShader* _shader /*= nullptr*/, bool _useMat /*= true*/)
{
	if (_shader)
		_deviceContext->PSSetShader(_shader, nullptr, 0);

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

				_deviceContext->PSSetShaderResources(0, 4, modelRV);
			}

			Matrix nodeMat = ConvertMat(nodeWorldMap[node->mName.C_Str()]);

			_cb->skinMat = nodeMat;
			_cb->skinNorm = XMMatrixInverse(nullptr, XMMatrixTranspose(nodeMat));
			_deviceContext->UpdateSubresource(_cbBuff, 0, nullptr, _cb, 0, 0);

			_deviceContext->DrawIndexed(m_nIndices[meshIdx], 0, 0);
		}

		// 차일드 큐에 넣기
		for (int i = 0; i < node->mNumChildren; i++) {
			nodes.push(node->mChildren[i]);
		}

		nodeIdx++;
	}
	
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
