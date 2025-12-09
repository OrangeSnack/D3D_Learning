#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <string>

#include "Component.h"
#include "Transform.h"


class Component;
class Transform;
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
	auto comp = std::make_shared<T>(std::forward<Args>(args)...);
	components[typeid(T)] = comp;
	return comp;
}

// 컴포넌트 찾기
template<typename T>
std::weak_ptr<T>
GameObject::GetComponent()
{
	auto it = components.find(typeid(T));
	if (it == components.end())
		return std::weak_ptr<T>();
	return std::static_pointer_cast<T>(it->second);
}

