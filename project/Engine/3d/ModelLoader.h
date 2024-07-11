#pragma once
#include <string>
#include <Model.h>
#include "Animation/Animation.h"
#include "LineBox.h"
#include <CollisionManager/Collider.h>
#include "CollisionManager/ColliderManager.h"

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

	void Update();
	void Draw(const ViewProjection& viewProjection, bool flag);

	void Delete();

	//色とアルファ値
	void SetColor(Vector4 color);

	//光沢度
	void SetShininess(float i);

	//映り込み度
	void Environment(float environment, bool flag);


	Model* GetModel(std::string name) { return models[name]; }

	void SetTexture(const std::string& filename);

	int GetColliderSize() { return int(colliderManager_.size()); }
	Collider* GetCollider(int i) { return colliderManager_[i]; }

private:
	void LoadJsonObjFile(const std::string& filename);

private:
	Scene* levelData = nullptr;
	std::map<std::string, Model*> models;
	std::map<std::string, Animation*> animationModels;
	std::vector<WorldTransform*> worldTransforms;
	//std::map<std::string, LineBox*>lineboxs_;

	std::vector<ColliderManager*> colliderManager_;
};


