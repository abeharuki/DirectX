#pragma once
#include <string>
#include <Model.h>


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

private:
	void LoadJsonObjFile(const std::string& filename);

private:
	Scene* levelData = nullptr;
	std::map<std::string, Model*> models;
	std::vector<WorldTransform*> worldTransforms;

};