#include "AssimpLoader.h"
#include "ResourceManager.h"
#include <filesystem>
#include "StaticMesh.h"
#include "Material.h"
#include "directxtk/SimpleMath.h"
#include "../BaseEngine/Helper.h"
#include "directxtk/WICTextureLoader.h"
#include "Texture2D.h"
#include "PhongMaterial.h"
#include "PhongRenderer.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

AssimpLoader* AssimpLoader::instance = nullptr;

bool AssimpLoader::LoadVertex(std::vector<Mesh_Vertex>* _vertices, const aiMesh* _mesh)
{
	for (UINT i = 0; i < _mesh->mNumVertices; i++) {
		Mesh_Vertex v;

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

bool AssimpLoader::LoadIndex(std::vector<UINT>* _indices, const aiMesh* _mesh)
{
	for (UINT i = 0; i < _mesh->mNumFaces; i++) {
		for (UINT j = 0; j < _mesh->mFaces[i].mNumIndices; j++)
			_indices->push_back(UINT(_mesh->mFaces[i].mIndices[j]));
	}

	return true;
}

bool AssimpLoader::LoadMaterials(std::vector<std::shared_ptr<Material>>& _out, const aiScene* _scene, const std::wstring& _path)
{
	if (!_scene->HasMaterials())
		return false;

	_out.resize(_scene->mNumMaterials);

	for (UINT i = 0; i < _scene->mNumMaterials; i++) {
		// TODO :: 메테리얼으로 ResourceView 만들기
		aiString aiStr;
		aiMaterial* aiMat = _scene->mMaterials[i];
		auto m_pDevice = ResourceManager::GetInstance()->GetDevice();
		auto currMat = std::make_shared<PhongMaterial>();
		currMat->renderer = std::make_shared<PhongRenderer>();

		_out[i] = currMat;

		// 디퓨즈
		if (aiMat->GetTextureCount(aiTextureType_DIFFUSE)) {
			if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _path / p.filename();

				// 리소스 매니저에서 불러오기
				auto diffMat = ResourceManager::GetInstance()->LoadFile<Texture2D>(relativePath.wstring());
				if(!diffMat->srv)
					HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, diffMat->srv.GetAddressOf()));

				currMat->diffuse = diffMat;
			}
		}

		// 디퓨즈가 없으면?
		if (!currMat->diffuse) {
			auto diffMat = ResourceManager::GetInstance()->LoadFile<Texture2D>(defaultDiffuse);
			if(!diffMat->srv)
				HR_T(CreateWICTextureFromFile(m_pDevice, defaultDiffuse.c_str(), nullptr, diffMat->srv.GetAddressOf()));

			currMat->diffuse = diffMat;
		}

		// 스페큘러
		if (aiMat->GetTextureCount(aiTextureType_SPECULAR)) {
			if (aiMat->GetTexture(aiTextureType_SPECULAR, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _path / p.filename();

				auto specMat = ResourceManager::GetInstance()->LoadFile<Texture2D>(relativePath.wstring());
				if (!specMat->srv)
					HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, specMat->srv.GetAddressOf()));

				currMat->specular = specMat;
			}
		}

		// 노말
		if (aiMat->GetTextureCount(aiTextureType_NORMALS)) {
			if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _path / p.filename();

				auto normalMat = ResourceManager::GetInstance()->LoadFile<Texture2D>(relativePath.wstring());
				if (!normalMat->srv)
					HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, normalMat->srv.GetAddressOf()));

				currMat->normal = normalMat;

				//HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].normal));
			}
		}

		// 이미션
		if (aiMat->GetTextureCount(aiTextureType_EMISSIVE)) {
			if (aiMat->GetTexture(aiTextureType_EMISSIVE, 0, &aiStr) == aiReturn_SUCCESS) {
				std::filesystem::path p = std::filesystem::path(aiStr.C_Str());
				std::filesystem::path relativePath = _path / p.filename();

				auto emissMat = ResourceManager::GetInstance()->LoadFile<Texture2D>(relativePath.wstring());
				if (!emissMat->srv)
					HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, emissMat->srv.GetAddressOf()));

				currMat->normal = emissMat;

				//HR_T(CreateWICTextureFromFile(m_pDevice, relativePath.wstring().c_str(), nullptr, &_out[i].emissive));
			}
		}
	}

	return true;
}

AssimpLoader* AssimpLoader::GetInstance()
{
	if (instance == nullptr) {
		instance = new AssimpLoader();
	}
	return instance;
}

void AssimpLoader::Initialize()
{
	
}

AssimpLoader::~AssimpLoader()
{
	delete(instance);
}

void AssimpLoader::LoadStaticMesh(std::wstring _filePath)
{
	UINT importFlags =
		aiProcess_CalcTangentSpace |
		aiProcess_GenUVCoords |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_GenNormals |
		aiProcess_OptimizeMeshes |
		aiProcess_PreTransformVertices |
		aiProcess_ImproveCacheLocality |
		aiProcess_ConvertToLeftHanded;

	// 경로 가공
	std::filesystem::path p = _filePath.c_str();

	// 유효성 확인
	if (!p.has_filename())
		throw std::exception("AssimpLoader::LoadStaticMesh : path is not file!!");
	if (!std::filesystem::exists(p))
		throw std::exception("AssimpLoader::LoadStaticMesh : file is not found!!");

	// 리소스 로드
	auto scene = m_importer.ReadFile(p.string().c_str(), importFlags);
	auto meshResource = ResourceManager::GetInstance()->LoadFile<StaticMesh>(p.wstring());
	meshResource->scene = scene;

	if (meshResource->modelVertices.empty()) {
		// 메시 로딩
		for (UINT i = 0; i < scene->mNumMeshes; i++) {
			std::vector<Mesh_Vertex> tempV;
			std::vector<UINT> tempI;
			LoadVertex(&tempV, scene->mMeshes[i]);
			LoadIndex(&tempI, scene->mMeshes[i]);

			meshResource->modelVertices.push_back(tempV);
			meshResource->modelIndices.push_back(tempI);
		}

		// 메테리얼 로딩
		LoadMaterials(meshResource->modelMaterials, scene, p.parent_path().wstring());
	}
};

void AssimpLoader::LoadSkeletalMesh(std::wstring _filePath)
{

}

void AssimpLoader::LoadMesh(std::wstring _filePath)
{

}
