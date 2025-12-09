#pragma once
#include "GameObject.h"

class GameObject;
class Component
{
public:
	Component() = default;

	// 부모객체 표시
	GameObject* gameObject = nullptr;

	virtual void Start() = 0;
	virtual void Update() = 0;
};

