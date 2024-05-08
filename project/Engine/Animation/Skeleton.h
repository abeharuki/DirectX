#pragma once
#include "Math/math.h"
#include "Math/Quaternion.h"
#include <map>
#include <span>
#include <array>
#include <optional>


struct Joint {
	QuaternionTransform transform;//Transform情報
	Matrix4x4 locaalMatrix;//localMatrix
	Matrix4x4 skeletonSpaceMatrix;//skeletonSpaceでの変換行列
	std::string name;//名前
	std::vector<int32_t>children;//子JointnoIndexのリスト
	int32_t index;//自身のIndex
	std::optional<int32_t>parent;//親JointのIndex
};

struct Skeleton {
	int32_t root;
	std::map<std::string, int32_t>jointMap;//Joint名とIndexの辞書
	std::vector<Joint>joints;//所属しているジョイント
};

namespace SkeletonPace {
	
	
	Skeleton CreateSkeleton(const Node& rootNode);

	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
}