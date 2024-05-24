#include "ModelManager.h"
#include <Model.h>

ModelManager* ModelManager::instance_ = nullptr;

ModelManager* ModelManager::GetInstance() {
	if (instance_ == nullptr)
	{
		instance_ = new ModelManager();
	}
	return instance_;
}

MaterialData ModelManager::LoadMaterialTemplateFile(const std::string& filename) {
	// 宣言
	MaterialData materialData; // 構築するMaterialData
	ModelData modelData;
	std::string line;                              // ファイルから読んだ1行を格納する
	std::ifstream file(filename); // ファイルを開く
	assert(file.is_open());                        // 開けなかったら止める

	std::filesystem::path ps = std::filesystem::path(filename);
	std::string directryPath = ps.parent_path().string();


	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結しているファイルパス
			materialData.textureFilePath = directryPath + "/" + textureFilename;
		}
	}

	return materialData;
}

//アニメーション用
ModelData ModelManager::LoadGltfFile(const std::string& filename) {


	ModelData modelData;
	Assimp::Importer importer;
	std::string filePath = filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());



	//メッシュの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));
		modelData.vertices.resize(mesh->mNumVertices);

		//頂点の解析
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
			//座標系の変換
			modelData.vertices[vertexIndex].position = { -position.x,position.y,position.z,1.0f };
			modelData.vertices[vertexIndex].normal = { -normal.x,normal.y,normal.z };
			modelData.vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };

		}

		//Indexの解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				modelData.indices.push_back(vertexIndex);
			}

		}

		//SkinCluster構築用データの取得
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			//格納領域確保
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

			//InverseBindposeMatrixの抽出
			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
			Matrix4x4 bindPoseMatrix = Math::MakeAffineMatrix({ scale.x,scale.y,scale.z }, { rotate.x,-rotate.y,-rotate.z,rotate.w }, { -translate.x,translate.y,translate.z });
			jointWeightData.inverseBindPoseMatrix = Math::Inverse(bindPoseMatrix);

			//Weight情報を取り出す
			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
			}

		}
		
	}



	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = textureFilePath.C_Str();
		}
	}

	modelData.rootNode = ReadNode(scene->mRootNode);

	return modelData;

}

//普通のモデル
ModelData ModelManager::LoadObjFile(const std::string& filename) {


	ModelData modelData;
	Assimp::Importer importer;
	std::string filePath = filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());




	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));
		modelData.vertices.resize(mesh->mNumVertices);

		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				VertexData vertex;
				vertex.position = { position.x,position.y,position.z,1.0f };
				vertex.normal = { normal.x,normal.y,normal.z };
				vertex.texcoord = { texcoord.x,texcoord.y };
				vertex.position.z *= -1.0f;
				vertex.normal.x *= -1.0f;
				modelData.vertices.push_back(vertex);
			}

		}

		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				modelData.indices.push_back(vertexIndex);
			}

		}


	}



	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = textureFilePath.C_Str();
		}
	}

	modelData.rootNode = ReadNode(scene->mRootNode);

	return modelData;

}

Node ModelManager::ReadNode(aiNode* node)
{
	Node result{};

	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate);//assimpの行列からSRTの抽出する
	result.transform.scale = { scale.x,scale.y,scale.z };
	result.transform.rotate = { rotate.x,-rotate.y,-rotate.z,rotate.w };
	result.transform.translate = { -translate.x,translate.y,translate.z };
	result.localMatrix = Math::MakeAffineMatrix(result.transform.scale, result.transform.rotate, result.transform.translate);

	result.name = node->mName.C_Str();//Node名を格納
	result.children.resize(node->mNumChildren);//子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
	{
		//再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}
	return result;
}

//Json型のモデル用
void ModelManager::LoadJsonObjFile(const std::string& filename) {
	//連結してファイルパスを得る
	const std::string fullpath = "./resources/JsonFile" + filename;

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
	Scene* levelData = new Scene();

	//"objects"のすべてのオブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"]) {
		Scene::ObjectData rootObject;
		LoadJsonObject(object, rootObject);
		levelData->objects.push_back(rootObject);

	}

	//レベルデータからオブジェクトの生成,配置
	for (auto& objectData : levelData->objects) {
		//ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.filename);
		if (it != models.end()) { model = it->second; }
		//モデルを指定して3Dオブジェクトを生成
		Object3d* newObject = Object3d::Create(model);
		//座標
		newObject->SetPosition(objectData.transform.translate);
		//回転角
		newObject->SetRatation(objectData.transform.rotate);
		//スケール
		newObject->SetScale(objectData.transform.scale);
		//配列に登録
		objectData.objects.clear();
		objectData.objects.push_back(newObject);
		levelData->objects.push_back(objectData);
	}
}

void ModelManager::LoadJsonObject(nlohmann::json& object, Scene::ObjectData& parentObjectData) {
	assert(object.contains("type"));

	// 種別を取得
	std::string type = object["type"].get<std::string>();

	// MESH
	if (type.compare("MESH") == 0) {
		// 今回追加した要素の参照を得る
		Scene::ObjectData objectData;

		if (object.contains("file_name")) {
			// ファイル名
			objectData.filename = object["file_name"].get<std::string>();
		}

		nlohmann::json transform = object["transform"];
		// 平行移動
		objectData.transform.translate.x = transform["translation"][0].get<float>();
		objectData.transform.translate.y = transform["translation"][1].get<float>();
		objectData.transform.translate.z = transform["translation"][2].get<float>();
		// 回転角
		objectData.transform.rotate.x = transform["rotation"][0].get<float>();
		objectData.transform.rotate.y = transform["rotation"][1].get<float>();
		objectData.transform.rotate.z = transform["rotation"][2].get<float>();
		// スケーリング
		objectData.transform.scale.x = transform["scaling"][0].get<float>();
		objectData.transform.scale.y = transform["scaling"][1].get<float>();
		objectData.transform.scale.z = transform["scaling"][2].get<float>();

		// 子オブジェクトの処理
		if (object.contains("children")) {
			for (nlohmann::json& child : object["children"]) {
				LoadJsonObject(child, objectData);
			}
		}

		// 親オブジェクトの子リストに追加
		parentObjectData.children.push_back(objectData);
	}


}
