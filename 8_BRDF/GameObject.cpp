#include "GameObject.h"
#include "Transform.h"
#include "Component.h"

GameObject::GameObject()
{
	transform = AddComponent<Transform>();
}

// 컴포넌트 추가
template<typename T, typename... Args>
std::weak_ptr<T> GameObject::AddComponent(Args&&... args)
{
	static_assert(std::is_base_of<Component, T>::value, "AddComponent<T> : T must derive from Component!!");

	auto comp = std::make_shared<T>(std::forward<Args>(args)...);
	comp->gameObject = this;
	components[typeid(T)] = comp;
	return comp;
}

// 컴포넌트 찾기
template<typename T>
std::weak_ptr<T> GameObject::GetComponent()
{
	static_assert(std::is_base_of<Component, T>::value, "GetComponent<T> : T must derive from Component!!");

	auto it = components.find(typeid(T));
	if (it == components.end())
		return nullptr;
	return std::static_pointer_cast<T>(it->second);
}