#include "Transform.h"
#include <memory>


void Transform::Start()
{

}

void Transform::Update()
{

}

void Transform::SetParent(std::weak_ptr<Transform>& _parent)
{
	parent = _parent;
}

std::weak_ptr<Transform> Transform::GetParent() const
{
	return parent;
}

const DirectX::SimpleMath::Matrix& Transform::GetMatrix() const
{
	return m_World;
}
