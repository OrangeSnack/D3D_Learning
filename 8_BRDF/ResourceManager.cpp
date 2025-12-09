#include "ResourceManager.h"
#include "../BaseEngine/TimeSystem.h"
using namespace std;

#define OUTDATETIME 10.0f

void ResourceManager::Initialize()
{
	if (!instance) {
		instance = make_unique<ResourceManager>();
	}

	// 객체 초기화 함수
	instance->Start();
}

void ResourceManager::Start()
{
	resources.clear();
}

void ResourceManager::Update()
{
	for (const auto& resource : resources) {
		if(resource.second->elipsedTime >= OUTDATETIME)

		resource.second->elipsedTime += GameTimer::m_Instance->DeltaTime();
	}
}
