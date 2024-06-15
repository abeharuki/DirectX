#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <unordered_map>
#include <vector>
#include <wrl.h>
#include "Math/math.h"
#include "WinApp.h"
#include "TextureManeger.h"

class Material
{
public:

	void Initialize();

	void Update();

	const Microsoft::WRL::ComPtr<ID3D12Resource> GetResource() const { return materialResorce_; }

	//色
	const Color& GetColor() const { return materialData->color; };
	void SetColor(const Vector4& color) {
		materialData->color.rgb = { color.x, color.y, color.z };
		materialData->color.a = color.w;
	};

	void SetLighting(bool flag) { materialData->enableLighting = flag; }

	//UVTransform
	void SetUV(Transform& uvTransform) {
		// UVTransform用の行列
		Matrix4x4 uvTransformMatrix = Math::MakeAffineMatrix(
			{
				uvTransform.scale.x + 1,
				uvTransform.scale.y + 1,
				uvTransform.scale.z + 1,
			},
			uvTransform.rotate, uvTransform.translate);

		materialData->uvTransform = uvTransformMatrix;
	}

	//光沢度
	void SetShininess(float i) { materialData->shininess = i; };
	
private:

	//マテリアル用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResorce_;

	MaterialD* materialData = nullptr;
};
