#pragma once
#include "framework.h"
#include <memory>
#include <unordered_map>
#include <string>
#include "GameResource.h"

class ResourceManager
{
private:
	std::unordered_map<std::wstring, std::shared_ptr<GameResource>> resources;
	static std::unique_ptr<ResourceManager> instance;

public:
	static ResourceManager* GetInstance();
	void Initialize();
	void Start();
	void Update();

	template <typename T, typename ... Args>
	std::weak_ptr<T> LoadFile(std::wstring _path, Args&&... args);
};

template <typename T, typename ... Args>
std::weak_ptr<T>
ResourceManager::LoadFile(std::wstring _path, Args&&... args)
{
	auto it = resources.find(_path);
	
	if (it != resources.end()) {
		// 기존 리소스 반환, 만료시간 초기화
		it->second->elipsedTime = 0.0f;
		return std::dynamic_pointer_cast<T>(it->second);
	}
	else {
		// 새로운 리소스 생성
		auto resource = std::make_shared<T>(std::forward<Args>(args));
		resources[_path] = resource;
		return resource;
	}
		
}

