#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <string>

#include "Component.h"
#include "Transform.h"



class GameObject
{
public:
	GameObject();

	template<typename T, typename... Args>
	std::weak_ptr<T> AddComponent(Args&&... args);
	template<typename T>
	std::weak_ptr<T> GetComponent();

	std::wstring name = L"GameObject";
	std::wstring tag = L"Default";
	std::unordered_map<std::type_index, std::shared_ptr<Component>> components;
	std::weak_ptr<Transform> transform;
};

// 컴포넌트 추가
template<typename T, typename... Args>
std::weak_ptr<T>
GameObject::AddComponent(Args&&... args)
{
	components[typeid(T)] = std::make_shared<T>(std::forward<Args>(args));
	return components[typeid(T)];
}

// 컴포넌트 찾기
template<typename T>
std::weak_ptr<T>
GameObject::GetComponent()
{
	auto it = components.find(typeid(T));
	if (it == components.end())
		return nullptr;
	return std::static_pointer_cast<T>(it->second);
}

