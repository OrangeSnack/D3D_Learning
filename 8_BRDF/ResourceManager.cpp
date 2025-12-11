#include "ResourceManager.h"
#include "../BaseEngine/TimeSystem.h"
using namespace std;

#define OUTDATETIME 10.0f

std::unique_ptr<ResourceManager> ResourceManager::instance = nullptr;

ResourceManager* ResourceManager::GetInstance()
{
	if (!instance) {
		instance = std::make_unique<ResourceManager>();
	}
	return instance.get();
}

void ResourceManager::Initialize()
{
	// 객체 초기화 함수
	instance->Start();
	resources.clear();
}

void ResourceManager::Start()
{
	
}

void ResourceManager::Update()
{
	for (const auto& resource : resources) {
		if(resource.second->elipsedTime >= OUTDATETIME)

		resource.second->elipsedTime += GameTimer::m_Instance->DeltaTime();
	}
}
