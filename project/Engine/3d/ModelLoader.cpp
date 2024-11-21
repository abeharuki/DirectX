#include "ModelLoader.h"
#include <CollisionManager/CollisionConfig.h>
#include <ParticleManager.h>

void ModelLoader::Initialize(const std::string& filename) {
	particleNum_ = -1;
	LoadJsonObjFile(filename);
}

void ModelLoader::Update() {
	
	for (WorldTransform* object : worldTransforms) {
		object->UpdateMatrix();
	}
	for (auto& objectData : levelData->objects) {
		if (objectData.filename == "Camera") {
			camera_.UpdateMatrix();
		}
	}
	for (int i = 0; i < particle_.size(); ++i) {
		particle_[i]->Update();
	}
}

void ModelLoader::Draw(const ViewProjection& viewProjection, bool flag) {
	int i = 0;
	int j = 0;
	for (auto& objectData : levelData->objects) {
		
		if (objectData.filename != "Camera") {
			//ファイル名から登録済みモデルを検索
			Model* model = nullptr;
			decltype(models)::iterator it = models.find(objectData.filename);
			if (it != models.end()) { model = it->second; }
			//モデルの描画
			if (model && i < worldTransforms.size()) {
				model->Draw(*worldTransforms[i], viewProjection, flag);
				
			}

			if (objectData.filename == "doorwall"|| objectData.filename == "wall") {
				colliderManager_[j]->SetWorldTransform(*worldTransforms[i]);
				colliderManager_[j]->Draw(viewProjection);
				++j;
			}
			i++;
		}

	}

	for (int x = 0; x < particle_.size(); ++x) {
		particle_[x]->Draw(viewProjection);
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
	particle_.clear();
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
		//int z = 0;
		//ファイル名から登録済みモデルを検索
		decltype(models)::iterator it = models.find(objectData.filename);
		if (it == models.end() && objectData.filename != "Camera") {
			//blenderでlightを消さないとここでエラーが出る又はファイルネームを設定してないと
			Model* model = Model::CreateFromObj(objectData.filename);
			models[objectData.filename] = model;
			
			Transform uv = {
		            {30.0f,30.0f,30.0f},
		            {0.0f,0.0f,0.0f},
		            {0.0f,0.0f,0.0f},
			};
			if (objectData.filename == "ground") {
				models[objectData.filename]->SetUV(uv);
			}
			
		}

		//blenderで登録したコライダーの名
		if (objectData.collider.typeName != "") {
			if (objectData.collider.typeName == "BOX") {
				AABB aabb = { {-objectData.collider.size.x / 2.0f,-objectData.collider.size.y / 2.0f ,-objectData.collider.size.z / 2.0f },{objectData.collider.size.x / 2.0f,objectData.collider.size.y / 2.0f,objectData.collider.size.z / 2.0f} };
				OBB obb = Math::ConvertAABBToOBB(aabb);
				ColliderManager* colliderManager = new ColliderManager();

				if (objectData.filename == "wall"|| objectData.filename == "doorwall") {

					colliderManager->SetOBB(obb);
					colliderManager->SetCollisionMask(kCollisionMaskWall);
					colliderManager->SetCollisionAttribute(kCollisionAttributeLoderWall);
					colliderManager->SetCollisionPrimitive(kCollisionPrimitiveOBB);


				}

				//BOXの当たり判定があるオブジェクトがあるならここに書く
				

				colliderManager_.push_back(colliderManager);

			}

			if (objectData.collider.typeName == "SPHERE") {
				ColliderManager* colliderManager = new ColliderManager();
				//SPHEREの当たり判定があるオブジェクトがあるならここに書く
				if (objectData.filename == "") {
					colliderManager->SetRadius(objectData.collider.radius);
					colliderManager->SetCollisionAttribute(kCollisionAttributeLoderWall);
					colliderManager->SetCollisionMask(kCollisionMaskWall);

				}
				colliderManager->SetCollisionPrimitive(kCollisionPrimitiveSphere);
				colliderManager_.push_back(colliderManager);

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
			if (objectData.filename == "pillar") {
				++particleNum_;
				ParticleSystem* particle = new ParticleSystem;
				particle = ParticleManager::Create("resources/particle/circle.png",particleNum_);
				  emitter_ = {
						.translate = {objectData.transform.translate.x,objectData.transform.translate.y+4,objectData.transform.translate.z},
		                .count{50},
		                .frequency{0.075f},
		                .frequencyTime{0.5f},
		                .scaleRange{.min{2.2f,1.0f,11.0f},.max{2.2f,3.5f,1.0f}},
		                .translateRange{.min{-3.2f,-1.2f,0.0f},.max{3.2f,1.2f,0.0f}},
		                .colorRange{.min{0.33f,0,0.33f},.max{0.5f,0,1.0f}},
						.alphaRange{.min{1.0f},.max{1.0f}},
		                .lifeTimeRange{.min{0.1f},.max{1.0f}},
		                .velocityRange{.min{0.0f,0.1f,0.0f},.max{0.0f,0.4f,0.3f}},
						
				  };
				particle->SetEmitter(emitter_);

				particle_.push_back(particle);


			}

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
}
void ModelLoader::SetLight(DirectionLight directionLight){
	for (auto& objectData : levelData->objects) {
		//ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.filename);
		if (it != models.end()) { model = it->second; }

		model->DirectionalLightDraw(directionLight);

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

		model->Environment(environment, flag);

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
