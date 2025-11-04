#pragma once
#include <iostream>
#include <vector>
#include <directxtk/SimpleMath.h>

struct AnimationKey {
	float time;
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Quaternion rotation;
	DirectX::SimpleMath::Vector3 scale;
};

struct BoneAnimtion {
	std::vector<AnimationKey> animKeys;

	AnimationKey Evaluate(AnimationKey& _k1, AnimationKey& _k2, float _currTime) {
		float lerpTime = (_currTime - _k1.time) / (_k2.time - _k1.time);

		return {
			_currTime,
			DirectX::SimpleMath::Vector3::Lerp(_k1.position, _k2.position, lerpTime),
			DirectX::SimpleMath::Quaternion::Slerp(_k1.rotation, _k2.rotation, lerpTime),
			DirectX::SimpleMath::Vector3::Lerp(_k1.scale, _k2.scale, lerpTime)
		};
	}
};

struct Animation {
	std::wstring name;
	float duration;
	std::vector<BoneAnimtion> boneAnim;
};

struct Bone {
	DirectX::SimpleMath::Matrix localMat;	// 로컬변환 매트릭스
	DirectX::SimpleMath::Matrix modelMat;	// 누적변환 매트릭스
	int m_ParentIndex;
	int m_Index;
	std::string Name;
	BoneAnimtion* m_pBoneAnim = nullptr;
};