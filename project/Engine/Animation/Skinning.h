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
struct VertexInfluence {
	std::array<float, kNumMaxInfluence> weights;
	std::array<int32_t, kNumMaxInfluence> jointIndices;
};

struct WellForGPU {
	Matrix4x4 skeletonSpaceMatrix;
	Matrix4x4 skeletonSpaceInverseTransposeMatrix;
};

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
