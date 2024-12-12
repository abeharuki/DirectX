#pragma once
#include "Math/math.h"
#include "Math/Quaternion.h"
#include <map>
#include <span>
#include <array>
#include <optional>

/**
 * @brief 1つのジョイントを表す構造体
 * @details ジョイントは、スケルトンの1部であり、トランスフォーム情報、ローカル行列、スケルトン空間での変換行列を持っている
 * 子ジョイントのインデックスリストと、親ジョイントのインデックスも保持します。
 */
struct Joint {
	QuaternionTransform transform;//Transform情報
	Matrix4x4 locaalMatrix;//localMatrix
	Matrix4x4 skeletonSpaceMatrix;//skeletonSpaceでの変換行列
	std::string name;//名前
	std::vector<int32_t>children;//子JointnoIndexのリスト
	int32_t index;//自身のIndex
	std::optional<int32_t>parent;//親JointのIndex
};

/**
 * @brief スケルトン全体を表す構造体
 * @details スケルトンは、ジョイントの集合体であり、親子関係やジョイント名などを管理します。
 */
struct Skeleton {
	int32_t root;
	std::map<std::string, int32_t>jointMap;//Joint名とIndexの辞書
	std::vector<Joint>joints;//所属しているジョイント
};

namespace SkeletonPace {

	//スケルトンの作成
	Skeleton CreateSkeleton(const Node& rootNode);

	//ジョイントの作成
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
}
