#include "ModelLoader.h"
#include <CollisionManager/CollisionConfig.h>

void ModelLoader::Initialize(const std::string& filename) {
	LoadJsonObjFile(filename);
}

void ModelLoader::Update() {
	int i = 0;
	for (WorldTransform* object : worldTransforms) {
		object->UpdateMatrix();
		if (i < colliderManager_.size()) {
			colliderManager_[i]->SetWorldTransform(*worldTransforms[i]);
			colliderManager_[i]->Chack(false);
			++i;
		}

	}
	for (auto& objectData : levelData->objects) {
		if (objectData.filename == "Camera") {
			camera_.UpdateMatrix();
		}
	}
	
}

void ModelLoader::Draw(const ViewProjection& viewProjection, bool flag) {
	int i = 0;
	for (auto& objectData : levelData->objects) {
		if (objectData.filename != "Camera") {
			//ファイル名から登録済みモデルを検索
			Model* model = nullptr;
			decltype(models)::iterator it = models.find(objectData.filename);
			if (it != models.end()) { model = it->second; }

			if (model && i < worldTransforms.size()) {
				model->Draw(*worldTransforms[i], viewProjection, flag);
				if (i < colliderManager_.size()) {
					colliderManager_[i]->Draw(viewProjection);
				}

			}


			i++;
		}
		
	}

}


void ModelLoader::Delete() {
	// worldTransformsのクリアとメモリ解放
	for (auto& transform : worldTransforms) {
		delete transform;
	}
	worldTransforms.clear();

	// modelsのクリアとメモリ解放
	for (auto& pair : models) {
		delete pair.second;
	}
	models.clear();

	// animationModelsのクリアとメモリ解放
	for (auto& pair : animationModels) {
		delete pair.second;
	}
	animationModels.clear();
	colliderManager_.clear();
}

ModelLoader* ModelLoader::Create(const std::string& filename) {
	ModelLoader* model = new ModelLoader;
	model->Initialize(filename);
	return model;
}

//Json型のモデル用
void ModelLoader::LoadJsonObjFile(const std::string& filename) {
	//連結してファイルパスを得る
	const std::string fullpath = /*./resources/JsonFile*/filename;

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

	//古いデータの削除と更新
	delete levelData;
	levelData = nullptr;
	Delete();


	//レベルデータ格納用インスタンスを作成
	levelData = new Scene();
	
	//"objects"のすべてのオブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"]) {
		//要素追加
		levelData->objects.emplace_back(Scene::ObjectData{});
		// 今回追加した要素の参照を得る
		Scene::ObjectData& objectData = levelData->objects.back();


		if (object.contains("file_name")) {
			objectData.filename = object["file_name"];
		}
		ModelManager::GetInstance()->LoadJsonObject(object, objectData);

	}

	//モデルの読み込み
	for (auto& objectData : levelData->objects) {
		//ファイル名から登録済みモデルを検索
		decltype(models)::iterator it = models.find(objectData.filename);
		if (it == models.end() && objectData.filename !="Camera") {
			//blenderでlightを消さないとここでエラーが出る又はファイルネームを設定してないと
			Model* model = Model::CreateFromObj(objectData.filename);
			models[objectData.filename] = model;

			if (objectData.collider.typeName != "") {
				if (objectData.collider.typeName == "BOX") {
					AABB aabb = { {-objectData.collider.size.x / 2.0f,-objectData.collider.size.y / 2.0f,-objectData.collider.size.z / 2.0f},{objectData.collider.size.x / 2.0f,objectData.collider.size.y / 2.0f,objectData.collider.size.z / 2.0f} };
					ColliderManager* colliderManager = new ColliderManager();
					if (objectData.filename == "ICO") {
						colliderManager->SetAABB(aabb);
						colliderManager->SetCollisionAttribute(kCollisionAttributeLoderICO);
						colliderManager->SetCollisionMask(kCollisionMaskICO);

					}

					if (objectData.filename == "BOX") {
						colliderManager->SetAABB(aabb);
						colliderManager->SetCollisionAttribute(kCollisionAttributeEnemy);
						colliderManager->SetCollisionMask(kCollisionMaskEnemy);

					}
					colliderManager->SetCollisionPrimitive(kCollisionPrimitiveAABB);

					colliderManager_.push_back(colliderManager);
				}

				if (objectData.collider.typeName == "SPHERE") {
					ColliderManager* colliderManager = new ColliderManager();
					if (objectData.filename == "BOX") {
						colliderManager->SetRadius(objectData.collider.radius);
						colliderManager->SetCollisionAttribute(kCollisionAttributeLoderICO);
						colliderManager->SetCollisionMask(kCollisionMaskICO);

					}
					colliderManager->SetCollisionPrimitive(kCollisionPrimitiveSphere);
					colliderManager_.push_back(colliderManager);

				}

			}

		}
	}

	//レベルデータからオブジェクトの生成,配置
	for (auto& objectData : levelData->objects) {
		if (objectData.filename != "Camera") {
			//モデルを指定して3Dオブジェクトを生成
			WorldTransform* newObject = new WorldTransform;
			//座標
			newObject->translate = objectData.transform.translate;
			//回転角
			newObject->rotate = objectData.transform.rotate;
			//スケール
			newObject->scale = objectData.transform.scale;
			newObject->Initialize();

			//配列に登録
			worldTransforms.push_back(newObject);
		}
		else {
			camera_.Initialize();
			camera_.translation_ = objectData.transform.translate;
			camera_.rotation_ = objectData.transform.rotate;
			
		}
		
		
	}

}


//色とアルファ値
void ModelLoader::SetColor(Vector4 color) {
	for (auto& objectData : levelData->objects) {
		//ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.filename);
		if (it != models.end()) { model = it->second; }

		model->SetColor(color);

	}
};



//光沢度
void ModelLoader::SetShininess(float i) {
	for (auto& objectData : levelData->objects) {
		//ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.filename);
		if (it != models.end()) { model = it->second; }

		model->SetShininess(i);

	}
};

//映り込み度
void ModelLoader::Environment(float environment, bool flag) {

	for (auto& objectData : levelData->objects) {
		//ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.filename);
		if (it != models.end()) { model = it->second; }

		model->Environment(environment,flag);
		
	}
}

void ModelLoader::SetTexture(const std::string& filename) {
	for (auto& objectData : levelData->objects) {
		//ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.filename);
		if (it != models.end()) { model = it->second; }
		model->SetTexture(filename);
		
	}
}