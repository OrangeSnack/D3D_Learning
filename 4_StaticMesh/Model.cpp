#include "Model.h"
#include <iostream>

bool Model::LoadFile(std::string _filePath)
{
	// 파일경로 저장
	std::filesystem::path p = _filePath.c_str();

	if (!p.has_filename())
		return false;	// 파일이 없다는 에러문구도 출력?
	filePath = p.parent_path();

	scene = importer.ReadFile(_filePath, importFlags);
	
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::string error = importer.GetErrorString();
		std::cerr << "Assimp Error: " << importer.GetErrorString() << std::endl;
		return false;
	}

	return true;
}
