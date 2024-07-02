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
	static LineBox* Create(AABB aabb);
	static LineBox* Create(OBB obb);

	// 初期化
	void Initialize(AABB aabb);
	void Initialize(OBB obb);


	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection, bool light);

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
	std::unique_ptr<Line> lines_[12];
	AABB aabb_;
};