#pragma once
#include <string>
#include <Model.h>
#include "Animation/Animation.h"
#include "LineBox.h"
#include <CollisionManager/Collider.h>
#include "CollisionManager/ColliderManager.h"
#include <ParticleSystem.h>

/**
 * @class ModelLoader
 * @brief Blenderをステージエディターして読み込み管理するクラス
 */
class ModelLoader {
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static ModelLoader* GetInstance();
	static ModelLoader* Create(const std::string& filename);

	//初期化
	void Initialize(const std::string& filename);

	//毎フレームの更新
	void Update();

	//描画
	void Draw(const ViewProjection& viewProjection, bool flag);

	//削除
	void Delete();

	//色とアルファ値
	void SetColor(Vector4 color);

	void SetLight(DirectionLight directionLight);

	//光沢度
	void SetShininess(float i);

	//映り込み度
	void Environment(float environment, bool flag);

	//モデルのゲッター
	Model* GetModel(std::string name) { return models[name]; }

	//テクスチャのセッター
	void SetTexture(const std::string& filename);

	//カメラ
	ViewProjection GetCamera() { return camera_; }

	//コライダー
	int GetColliderSize() { return int(colliderManager_.size()); }
	Collider* GetCollider(int i) { return colliderManager_[i]; }

private:
	//jsonファイルの読み込み
	void LoadJsonObjFile(const std::string& filename);

private:
	Scene* levelData = nullptr;
	std::map<std::string, Model*> models;
	std::map<std::string, Animation*> animationModels;
	std::vector<WorldTransform*> worldTransforms;
	ViewProjection camera_;
	

	std::vector<ColliderManager*> colliderManager_;
	std::vector<ParticleSystem*> particle_;
	EmitterSphere emitter_;
};


