#include "Model.h"
#include <iostream>

bool Model::LoadFile(std::string _filePath)
{
	scene = importer.ReadFile(_filePath, importFlags);
	
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::string error = importer.GetErrorString();
		std::cerr << "Assimp Error: " << importer.GetErrorString() << std::endl;
		return false;
	}

	return true;
}
