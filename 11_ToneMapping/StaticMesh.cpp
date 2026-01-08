#include "StaticMesh.h"
#include <iostream>
#include <directxtk/WICTextureLoader.h>
#include "../BaseEngine/Helper.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>
using namespace DirectX::SimpleMath;
using namespace DirectX;

StaticMesh::~StaticMesh()
{
	for (auto& buffer : m_pVertexBuffer) SAFE_RELEASE(buffer); m_pVertexBuffer.clear();
	for (auto& buffer : m_pIndexBuffer) SAFE_RELEASE(buffer); m_pIndexBuffer.clear();
}

bool StaticMesh::LoadFile(std::wstring _filePath)
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
	LoadMaterials(m_pMaterials, this);

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

	// 버텍스 쉐이더 생성

	ID3D10Blob* vertexShader = nullptr;

	HR_T(CompileShaderFromFile(L"ShadowMapVS.hlsl", "main", "vs_5_0", &vertexShader));

	HR_T(m_pDevice->CreateVertexShader(
		vertexShader->GetBufferPointer(),
		vertexShader->GetBufferSize(),
		nullptr,
		&m_pShadowVS));

	return true;
}

bool StaticMesh::LoadVertex(std::vector<Vertex>* _vertices, const aiMesh* _mesh)
{
	for (UINT i = 0; i < _mesh->mNumVertices; i++) {
		Vertex v;

		v.Pos = Vector3(_mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z);

		if (_mesh->HasNormals()) {
			v.Normal = Vector3(_mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z);
		}

		if (_mesh->HasTangentsAndBitangents()) {
			v.Tangent = Vector3(_mesh->mTangents[i].x, _mesh->mTangents[i].y, _mesh->mTangents[i].z);
			v.BiTangent = Vector3(_mesh->mBitangents[i].x, _mesh->mBitangents[i].y, _mesh->mBitangents[i].z);
		}

		if (_mesh->HasTextureCoords(0)) {
			v.Tex = Vector2(_mesh->mTextureCoords[0][i].x, _mesh->mTextureCoords[0][i].y);
		}

		_vertices->push_back(v);
	}

	return true;
}

bool StaticMesh::LoadIndex(std::vector<UINT>* _indices, const aiMesh* _mesh)
{
	for (UINT i = 0; i < _mesh->mNumFaces; i++) {
		for (UINT j = 0; j < _mesh->mFaces[i].mNumIndices; j++)
			_indices->push_back(UINT(_mesh->mFaces[i].mIndices[j]));
	}

	return true;
}

bool StaticMesh::LoadMaterials(std::vector<PBR_Materials>& _out, const StaticMesh* _model)
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
		/*aiColor4D baseColor;
		if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor) == AI_SUCCESS) {
			_out[i].BaseColor = { baseColor.r, baseColor.g, baseColor.b, baseColor.a };
		}
		else if (aiMat->Get(AI_MATKEY_BASE_COLOR, baseColor) == AI_SUCCESS) {
			_out[i].BaseColor = { baseColor.r, baseColor.g, baseColor.b, baseColor.a };
		}*/

		// 디퓨즈
		if (aiMat->GetTextureCount(aiTextureType_DIFFUSE)) {
			if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _model->filePath / p.filename();

				CreateResourceView(relativePath, &_out[i].diffuse);
				//HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].diffuse));
			}
		}
		// 디퓨즈 없을시
		if (_out[i].diffuse == nullptr) {
			CreateResourceView(defaultWhite, &_out[i].diffuse);
		}

		// 메탈릭
		if (aiMat->GetTextureCount(aiTextureType_METALNESS)) {
			if (aiMat->GetTexture(aiTextureType_METALNESS, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _model->filePath / p.filename();

				CreateResourceView(relativePath, &_out[i].metalic);
				//HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].normal));
			}
		}
		// 메탈릭 없을시
		if (_out[i].metalic == nullptr) {
			CreateResourceView(defaultBlack, &_out[i].metalic);
		}

		// 러프니스
		if (aiMat->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS)) {
			if (aiMat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _model->filePath / p.filename();

				CreateResourceView(relativePath, &_out[i].roughness);
				//HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].normal));
			}
		}
		else if (aiMat->GetTextureCount(aiTextureType_SHININESS)) {
			if (aiMat->GetTexture(aiTextureType_SHININESS, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _model->filePath / p.filename();

				CreateResourceView(relativePath, &_out[i].roughness);
				//HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].normal));
			}
		}
		// 러프니스 없을시
		if (_out[i].roughness == nullptr) {
			CreateResourceView(defaultWhite, &_out[i].roughness);
		}

		// 노말
		if (aiMat->GetTextureCount(aiTextureType_NORMALS)) {
			if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _model->filePath / p.filename();

				CreateResourceView(relativePath, &_out[i].normal);
				//HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].normal));
			}
		}
		// 노말맵 없을시
		if (_out[i].normal == nullptr) {
			CreateResourceView(defaultNormal, &_out[i].normal);
		}

		// AO
		if (aiMat->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION)) {
			if (aiMat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _model->filePath / p.filename();

				CreateResourceView(relativePath, &_out[i].ao);
				//HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].normal));
			}
		}
		// AO 없을시
		if (_out[i].ao == nullptr) {
			CreateResourceView(defaultWhite, &_out[i].ao);
		}

		// 이미션
		if (aiMat->GetTextureCount(aiTextureType_EMISSIVE)) {
			if (aiMat->GetTexture(aiTextureType_EMISSIVE, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _model->filePath / p.filename();

				CreateResourceView(relativePath, &_out[i].emissive);
				//HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].emissive));
			}
		}
		// 이미션 없을시
		if (_out[i].emissive == nullptr) {
			CreateResourceView(defaultBlack, &_out[i].emissive);
		}
	}

	return true;
}

void StaticMesh::CreateResourceView(std::filesystem::path& _path, ID3D11ShaderResourceView** _out)
{
	// TGA파일 확인
	if (_path.extension() == L".tga") {
		DirectX::ScratchImage image;
		DirectX::TexMetadata meta;
		HR_T(DirectX::LoadFromTGAFile(_path.wstring().c_str(), &meta, image));
		CreateShaderResourceView(m_pDevice, image.GetImages(), image.GetImageCount(), meta, _out);
	}
	else {
		ID3D11DeviceContext* context = nullptr;
		m_pDevice->GetImmediateContext(&context);

		HR_T(CreateWICTextureFromFileEx(
			m_pDevice,
			context,
			_path.wstring().c_str(),
			0,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE,
			0, 0,
			WIC_LOADER_FORCE_RGBA32 | WIC_LOADER_IGNORE_SRGB,
			nullptr,
			_out));
	}
}

bool StaticMesh::ShadowDraw(ID3D11DeviceContext* _deviceContext, ID3D11ShaderResourceView* _rsv)
{
	// 쉐이더 업데이트
	ID3D11VertexShader* defaultShader = nullptr;
	_deviceContext->VSGetShader(&defaultShader, nullptr, nullptr);
	_deviceContext->VSSetShader(m_pShadowVS, nullptr, 0);

	// ResourceView 설정
	modelRV[6] = _rsv;

	for (int i = 0; i < m_pVertexBuffer.size(); i++) {
		_deviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer[i], &m_VertexBufferStride, &m_VertexBufferOffset);
		_deviceContext->IASetIndexBuffer(m_pIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0);

		_deviceContext->DrawIndexed(m_nIndices[i], 0, 0);
	}

	// 쉐이더 복원
	_deviceContext->VSSetShader(defaultShader, nullptr, 0);

	return true;
}

bool StaticMesh::Draw(ID3D11DeviceContext* _deviceContext, bool _useMat)
{

	for (int i = 0; i < m_pVertexBuffer.size(); i++) {
		_deviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer[i], &m_VertexBufferStride, &m_VertexBufferOffset);
		_deviceContext->IASetIndexBuffer(m_pIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0);

		if (_useMat) {
			UINT matIdx = scene->mMeshes[i]->mMaterialIndex;
			modelRV[0] = m_pMaterials[matIdx].diffuse;
			modelRV[1] = m_pMaterials[matIdx].normal;
			modelRV[2] = m_pMaterials[matIdx].metalic;
			modelRV[3] = m_pMaterials[matIdx].roughness;
			modelRV[4] = m_pMaterials[matIdx].ao;
			modelRV[5] = m_pMaterials[matIdx].emissive;

			_deviceContext->PSSetShaderResources(0, 7, modelRV);
		}
		else {
			_deviceContext->PSSetShaderResources(6, 1, &modelRV[6]);
		}

		_deviceContext->DrawIndexed(m_nIndices[i], 0, 0);
	}

	return true;
}
