#include "Skinning.h"
#include "base/DescriptorHandle.h"
#include "base/DirectXCommon.h"

namespace SkinningPace {

	SkinCluster CreateSkinCuster(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Skeleton& skeleton, const ModelData& modelData, const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize) {
		SkinCluster skinCluster;
		//palette用のResourceを確保
		skinCluster.paletteResource = std::make_unique<StructuredBuffer>();
		skinCluster.paletteResource->Create(uint32_t(skeleton.joints.size()), sizeof(WellForGPU));
		WellForGPU* mappedPalette = static_cast<WellForGPU*>(skinCluster.paletteResource->Map());
		skinCluster.mappedPalette = { mappedPalette,skeleton.joints.size() };//spanを使ってアクセスするようにする



		//influence用のResource確保
		skinCluster.influenceResource = std::make_unique<StructuredBuffer>();
		skinCluster.influenceResource->Create((uint32_t)modelData.meshData.vertices.size(), sizeof(VertexInfluence));
		VertexInfluence* mappedInfluence = static_cast<VertexInfluence*>(skinCluster.influenceResource->Map());
		std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.meshData.vertices.size());//0埋め。weightを0にしておく。
		skinCluster.mappedInfluence = { mappedInfluence,modelData.meshData.vertices.size() };

		//InverceBindPoseMatrixの保存領域を作成
		skinCluster.inverseBindPoseMatrixs.resize(skeleton.joints.size());
		//std::generate(skinCluster.inverseBindPoseMatrixs.begin(), skinCluster.inverseBindPoseMatrixs.end(), Math::MakeIdentity4x4());
		for (int i = 0; i < skinCluster.inverseBindPoseMatrixs.size(); ++i) {
			skinCluster.inverseBindPoseMatrixs[i] = Math::MakeIdentity4x4();
		}

		//ModelDataのSkinCluster情報を解析してInfluence中身を埋める
		for (const auto& jointWeight : modelData.skinClusterData) {
			auto it = skeleton.jointMap.find(jointWeight.first);
			if (it == skeleton.jointMap.end()) {
				continue;
			}

			//該当のindexのinverseBindposeMatrixを代入
			skinCluster.inverseBindPoseMatrixs[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
			for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
				auto& currentInflunec = skinCluster.mappedInfluence[vertexWeight.vertexIndex];
				for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {
					if (currentInflunec.weights[index] == 0.0f) {
						currentInflunec.weights[index] = vertexWeight.weight;
						currentInflunec.jointIndices[index] = (*it).second;
						break;
					}

				}
			}

		}


		return skinCluster;
	}
}