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

/**
 * @file Line
 * @brief フレームだけのボックスの描画や管理を行うクラス
 */
class LineBox {
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static LineBox* GetInstance();

	//フレームboxの作成
	static LineBox* Create();
	static LineBox* Create(AABB aabb);
	static LineBox* Create(OBB obb);

	// 初期化
	void Initialize();
	void Initialize(AABB aabb);
	void Initialize(OBB obb);

	//毎フレームの更新
	void Update(AABB aabb);
	void Updata();

	//描画
	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection, bool light);

	//中心座標の設定
	void SetCenter(Vector3 center) {
		obb_.center = center;
		Updata();
	}

	//中心座標のゲッター
	Vector3 GetCenter() { return obb_.center; }

	//transformのセッター
	void SetWorldTransform(WorldTransform worldtransform) { 
		worldTransform_ = worldtransform;
		worldTransform_.translate = worldtransform.translate + obb_.center;
		worldTransform_.UpdateMatrix();
	}

	//transformのゲッター
	WorldTransform& GetWorldTramnsform() { return worldTransform_; }

	//光の色　向き　明るさ
	void LightDraw(Vector4 color, Vector3 direction, float intensity);

	//光の色　向き　明るさ
	void DirectionalLightDraw(DirectionLight directionLight);
	//ポイントライトの詳細　向き
	void PointLightDraw(PointLight pointLight, Vector3 direction);
	//スポットライト
	void SpotLightDraw(SpotLight spotLight);

	//線の開始地点と終了地点の設定
	void SetLinePos(Vector3 start, Vector3 end);
	//色の設定
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