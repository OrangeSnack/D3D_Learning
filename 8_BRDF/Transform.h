#pragma once
#include "Component.h"
#include "framework.h"

class Transform : public Component
{
	DirectX::SimpleMath::Matrix m_World = DirectX::SimpleMath::Matrix::Identity;	// 월드좌표계 변환행렬
	DirectX::SimpleMath::Vector3 Position = DirectX::SimpleMath::Vector3::Zero;		// x, y, z Position
	DirectX::SimpleMath::Vector3 Rotation = DirectX::SimpleMath::Vector3::Zero;		// pitch, yaw, roll
	DirectX::SimpleMath::Vector3 Scale = { 1.0f, 1.0f, 1.0f };						// x, y, z Scale
	DirectX::SimpleMath::Matrix* Parent = nullptr;									// Parent Matrix
};

