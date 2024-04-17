#include "ModelManager.h"



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



ModelData ModelManager::LoadObjFile(const std::string& filename) {
	/*/ 必要な変数の宣言
	ModelData modelData;            // 構築するModelData
	std::vector<Vector4> positions; // 位置
	std::vector<Vector3> normals;   // 法線
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::string line;               // ファイルから選んだ１行を格納する

	// ファイルを開く
	std::ifstream file(filename);
	assert(file.is_open()); // とりあえず開けなかったら止める

	std::filesystem::path ps = std::filesystem::path(filename);
	std::string directryPath = ps.parent_path().string();

	// ファイルを読み,ModelDataを構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		// identfierの応じた処理
		// 頂点情報を読み込む
		if (identifier == "v") {
			Vector4 position = {0.0f, 0.0f, 0.0f, 0.0f};
			s >> position.x >> position.y >> position.z;
			position.z *= -1.0f; // 位置の反転
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord = {0.0f, 0.0f};
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal = {0.0f, 0.0f, 0.0f};
			normal.z *= -1.0f; // 法線の反転
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		} else if (identifier == "f") {
			VertexData triangle[3];
			// 面は三角形減退。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは[位置/UV/法線]で格納しているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); // 区切りでインデックスを読んでく
					elementIndices[element] = std::stoi(index);
				}

				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				// VertexData vertex = { position,texcoord,normal };
				// modelData.vertices.push_back(vertex);
				triangle[faceVertex] = {position, texcoord, normal};
			}

			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);

		} else if (identifier == "mtllib") {
			// materialTemplateLibraryファイルの名前を取得
			std::string materialFilename;
			s >> materialFilename;
			materialFilename = directryPath + "/" + materialFilename;

			// 基本的にobjファイルと同一階層にmtlは存在させるから、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(materialFilename);
		}
	}
	return modelData;
	*/

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
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;//nodeのlocalMatrixを取得
	aiLocalMatrix.Transpose();//列ベクトル形式を行ベクトル形式に転置
	for (uint32_t i = 0; i < 4; i++)
	{
		for (uint32_t j = 0; j < 4; j++)
		{
			result.localMatrix.m[i][j] = aiLocalMatrix[i][j];
		}
	}
	result.name = node->mName.C_Str();//Node名を格納
	result.children.resize(node->mNumChildren);//子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
	{
		//再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}
	return result;
}
