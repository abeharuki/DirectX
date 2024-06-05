#include "ModelLoader.h"

void ModelLoader::Initialize(const std::string& filename) {
	LoadJsonObjFile(filename);
}

void ModelLoader::Update() {
	for (WorldTransform* object : worldTransforms) {
		object->UpdateMatrix();
	}
}

void ModelLoader::Draw(const ViewProjection& viewProjection, bool flag) {
	int i = 0;
	for (auto& objectData : levelData->objects) {
		//ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.filename);
		if (it != models.end()) { model = it->second; }

		model->Draw(*worldTransforms[i], viewProjection, flag);
		i++;
	}
}

ModelLoader* ModelLoader::Create(const std::string& filename) {
	ModelLoader* model = new ModelLoader;
	model->Initialize(filename);
	return model;
}

//Json型のモデル用
void ModelLoader::LoadJsonObjFile(const std::string& filename) {
	//連結してファイルパスを得る
	const std::string fullpath = filename;

	//fileストリーム
	std::ifstream file;

	//ファイルを開く
	file.open(fullpath);
	//fileオープンの失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	//JSON文字列から回答したデータ
	nlohmann::json deserialized;

	//解凍
	file >> deserialized;

	//正しいレベルデータfileかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	assert(name.compare("scene") == 0);

	//レベルデータ格納用インスタンスを作成
	levelData = new Scene();

	//"objects"のすべてのオブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"]) {
		//要素追加
		levelData->objects.emplace_back(Scene::ObjectData{});
		// 今回追加した要素の参照を得る
		Scene::ObjectData& objectData = levelData->objects.back();
		ModelManager::GetInstance()->LoadJsonObject(object, objectData);

	}

	//モデルの読み込み
	/*
	for (auto& objectData : levelData->objects) {
		//ファイル名から登録済みモデルを検索
		decltype(models)::iterator it = models.find(objectData.filename);
		if (it != models.end()) {
			Model* model = Model::CreateFromObj(objectData.filename);
			models[objectData.filename] = model;
		}
	}
	*/
	//レベルデータからオブジェクトの生成,配置
	for (auto& objectData : levelData->objects) {
		//ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.filename);
		if (it != models.end()) { model = it->second; }
		//モデルを指定して3Dオブジェクトを生成
		WorldTransform* newObject = new WorldTransform;
		newObject->Initialize();
		//座標
		newObject->translate = objectData.transform.translate;
		//回転角
		newObject->rotate = objectData.transform.rotate;
		//スケール
		newObject->scale = objectData.transform.scale;
		//配列に登録
		worldTransforms.push_back(newObject);
	}
}
