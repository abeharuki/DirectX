#include "Math/math.h"
#include "Math/Quaternion.h"
#include <map>
#include <span>
#include <array>
#include <optional>
#include <GraphicsPipeline.h>
#include "Skeleton.h"
#include "3d/Mesh.h"
#include "Engine.h"

const uint32_t kNumMaxInfluence = 4;

/**
 * @brief 頂点の影響情報を格納する構造体
 * @details 各頂点に影響を与えるジョイントとその重みを管理
 * 最大で `kNumMaxInfluence` 個のジョイントの影響を保持
 */
struct VertexInfluence {
	std::array<float, kNumMaxInfluence> weights;
	std::array<int32_t, kNumMaxInfluence> jointIndices;
};

/**
*@brief GPU上で使用されるスケルトンに関連する変換行列を格納する構造体
* @details スケルトン空間での変換行列と、その逆転置行列を保持
*/
struct WellForGPU {
	Matrix4x4 skeletonSpaceMatrix;
	Matrix4x4 skeletonSpaceInverseTransposeMatrix;
};

/**
 * @brief スキンクラスターを表す構造体
 * @details スキンのジョイントの影響を管理するためのクラスター。頂点の影響、逆バインドポーズ行列などを管理
 */
struct SkinCluster {
	std::vector<Matrix4x4>inverseBindPoseMatrixs;
	std::unique_ptr<StructuredBuffer> influenceResource;
	std::span<VertexInfluence>mappedInfluence;
	std::unique_ptr<StructuredBuffer> paletteResource;
	std::span<WellForGPU>mappedPalette;

};

namespace SkinningPace {

	SkinCluster CreateSkinCuster( const Skeleton& skelteon, const ModelData& modelData);
}
