#pragma once
#include <string>
#include <Model.h>
#include "Animation/Animation.h"

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

	//色とアルファ値
	void SetColor(Vector4 color);

	//光沢度
	void SetShininess(float i);

	//映り込み度
	void Environment(float environment, bool flag);


	Model* GetModel(std::string name) { return models[name]; }

private:
	void LoadJsonObjFile(const std::string& filename);

private:
	Scene* levelData = nullptr;
	std::map<std::string, Model*> models;
	std::map<std::string, Animation*> animationModels;
	std::vector<WorldTransform*> worldTransforms;

};
