#include "ModelManager.h"

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
ModelData ModelManager::LoadGltfFile(const std::string& directorPath,const std::string& filename) {

	ModelData modelData;
	Assimp::Importer importer;
	std::string filePath = directorPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());

	//メッシュの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));//
		modelData.meshData.vertices.resize(mesh->mNumVertices);

		//頂点の解析
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
			//座標系の変換
			modelData.meshData.vertices[vertexIndex].position = { -position.x,position.y,position.z,1.0f };
			modelData.meshData.vertices[vertexIndex].normal = { -normal.x,normal.y,normal.z };
			modelData.meshData.vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };

		}

		//Indexの解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				modelData.meshData.indices.push_back(vertexIndex);
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

	
	//Materialの解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = directorPath + "/" + textureFilePath.C_Str();
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
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			//aiProcess_Triangulate(face);
			// 四角形も描画させてる
			//assert(face.mNumIndices == 3);
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
				modelData.meshData.vertices.push_back(vertex);
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


void ModelManager::LoadJsonObject(nlohmann::json& object, Scene::ObjectData& objectData) {

	assert(object.contains("type"));

	// 種別を取得
	std::string type = object["type"].get<std::string>();

	// MESH
	if (type.compare("MESH") == 0) {

		if (object.contains("file_name")) {
			// ファイル名
			objectData.filename = object["file_name"].get<std::string>();
		}

		nlohmann::json transform = object["transform"];
		// 平行移動
		objectData.transform.translate.x = transform["translation"][0].get<float>();
		objectData.transform.translate.y = transform["translation"][2].get<float>();
		objectData.transform.translate.z = transform["translation"][1].get<float>();
		// 回転角
		objectData.transform.rotate.x = -transform["rotation"][0].get<float>();
		objectData.transform.rotate.y = -transform["rotation"][2].get<float>();
		objectData.transform.rotate.z = -transform["rotation"][1].get<float>();
		// スケーリング
		objectData.transform.scale.x = transform["scaling"][0].get<float>();
		objectData.transform.scale.y = transform["scaling"][2].get<float>();
		objectData.transform.scale.z = transform["scaling"][1].get<float>();

		//TODO: コライダーのパラメータ読み込み
		nlohmann::json collider = object["collider"];
		//タイプ
		objectData.collider.typeName = collider["type"];
		//中心
		objectData.collider.center.x = collider["center"][0].get<float>();
		objectData.collider.center.y = collider["center"][2].get<float>();
		objectData.collider.center.z = collider["center"][1].get<float>();
		//サイズ
		objectData.collider.size.x = collider["size"][0].get<float>();
		objectData.collider.size.y = collider["size"][2].get<float>();
		objectData.collider.size.z = collider["size"][1].get<float>();

	}

	// 子オブジェクトの処理
	if (object.contains("children")) {
		for (nlohmann::json& child : object["children"]) {
			objectData.children.emplace_back(Scene::ObjectData{});
			LoadJsonObject(child, objectData.children.back());
		}
	}

}