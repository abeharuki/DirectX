#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include "StringUtility.h"
#include "Mesh.h"
#include <fstream>
#include <sstream>
#include <dxcapi.h>
#include "Engine.h"
#include <DirectXTex.h>
#include <d3dx12.h>
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "GraphicsPipeline.h"
#include <Line.h>

class LineBox {
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static LineBox* GetInstance();
	static LineBox* Create();
	static LineBox* Create(AABB aabb);
	static LineBox* Create(OBB obb);

	// 初期化
	void Initialize();
	void Initialize(AABB aabb);
	void Initialize(OBB obb);

	void Updata();

	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection, bool light);

	void SetCenter(Vector3 center) {
		obb_.center = center;
		//aabb_.min = aabb_.min + center;
		//aabb_.max = aabb_.max + center;
	}

	Vector3 GetCenter() { return obb_.center; }
	void SetWorldTransform(WorldTransform worldtransform) { 
		worldTransform_ = worldtransform;
		worldTransform_.translate = worldtransform.translate + obb_.center;
		worldTransform_.UpdateMatrix();
	}
	WorldTransform& GetWorldTramnsform() { return worldTransform_; }

	//光の色　向き　明るさ
	void LightDraw(Vector4 color, Vector3 direction, float intensity);

	//光の色　向き　明るさ
	void DirectionalLightDraw(DirectionLight directionLight);
	//ポイントライトの詳細　向き
	void PointLightDraw(PointLight pointLight, Vector3 direction);
	//スポットライト
	void SpotLightDraw(SpotLight spotLight);

	

	void SetLinePos(Vector3 start, Vector3 end);

	void SetColor(Vector4 color);
	// ブレンドモード
	void SetBlendMode(BlendMode blendMode);
	// 光沢度
	void SetShininess(float i);

private:
	WorldTransform worldTransform_;
	std::unique_ptr<Line> lines_[12];
	Vector3 vertex[8];
	AABB aabb_;
	OBB obb_;
};