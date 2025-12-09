#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class AssimpLoader
{
	Assimp::Importer importer;
	unsigned int importFlags = 0;
		
	const aiScene* scene = nullptr;
};

