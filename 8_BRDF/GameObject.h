#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <string>

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

	virtual void Start();
	virtual void Update();
};