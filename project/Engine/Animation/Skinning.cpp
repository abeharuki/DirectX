#include "Skinning.h"

namespace SkinningPace {

	SkinCluster CreateSkinCuster(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Skeleton& skeleton, const ModelData& modelData, const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize) {
		SkinCluster skinCluster;
		//palette用のResourceを確保
		skinCluster.paletteResource = Mesh::CreateBufferResoure(device.Get(), sizeof(WellForGPU) * skeleton.joints.size());
		WellForGPU* mappedPalette = nullptr;
		skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
		skinCluster.mappedPalette = { mappedPalette,skeleton.joints.size() };
		//srvhandle = Allo～()
		skinCluster.paletteSrvHandle.first = Engine::GetCPUDescriptorHandle(descriptorHeap.Get(), descriptorSize, 50);//空いてるインデックスに入れるマネージャーがないので手動で入れてる
		skinCluster.paletteSrvHandle.second = Engine::GetGPUDescriptorHandle(descriptorHeap.Get(), descriptorSize, 50);//posteffectのアロケータを使えばできる気がする

		//palette用のsrvを作成
		D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
		paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
		paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		paletteSrvDesc.Buffer.FirstElement = 0;
		paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		paletteSrvDesc.Buffer.NumElements = UINT(skeleton.joints.size());
		paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
		device->CreateShaderResourceView(skinCluster.paletteResource.Get(), &paletteSrvDesc, skinCluster.paletteSrvHandle.first);

		//influence用のResource確保
		skinCluster.influenceResource = Mesh::CreateBufferResoure(device.Get(), sizeof(VertexInfluence) * modelData.vertices.size());
		VertexInfluence* mappedInfluence = nullptr;
		skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
		std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size());
		skinCluster.mappedInfluence = { mappedInfluence,modelData.vertices.size() };

		//Influence用のVBV作成
		skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
		skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
		skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

		//InverceBindPoseMatrixの保存領域を作成
		skinCluster.inverseBindPoseMatrixs.resize(skeleton.joints.size());
		//std::generate(skinCluster.inverseBindPoseMatrixs.begin(), skinCluster.inverseBindPoseMatrixs.end(), Math::MakeIdentity4x4());

		for (auto& matrix : skinCluster.inverseBindPoseMatrixs) {
			matrix = Math::MakeIdentity4x4();
		}

		//ModelDataのSkinCluster情報を解析してInfluence中身を埋める
		for (const auto& jointWeight : modelData.skinClusterData) {
			auto it = skeleton.jointMap.find(jointWeight.first);
			if (it == skeleton.jointMap.end()) {
				continue;
			}

			//該当のindexのinverseBindposeMatrixを代入
			skinCluster.inverseBindPoseMatrixs[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
			for (const auto& vertexWeight : jointWeight.second.vertexWeightts) {
				auto& currentInflunec = skinCluster.mappedInfluence[vertexWeight.vertexIndex];
				for (uint32_t index = 0; index < kNumMaxInfluence;++index){
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