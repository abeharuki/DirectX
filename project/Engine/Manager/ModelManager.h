#pragma once
#include "Math/math.h"
#include "Engine.h"
#include <cassert>
#include <winerror.h>
#include <combaseapi.h>
#include <dxcapi.h>
#include <cstdlib>
#include <d3d12.h>
#include <wrl.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <DirectXTex.h>
#include <json.hpp>

/**
 * @struct ColliderData
 * @brief コライダーに関するデータを格納する構造体
 */
struct ColliderData {
	std::string typeName;
	Vector3 center;
	Vector3 size;
	float radius;
};

/**
 * @struct Scene
 * @brief シーン内のオブジェクトデータを管理する構造体
 */
struct Scene {
	struct ObjectData {
		std::string filename;
		Transform transform;
		ColliderData collider;
		std::vector<ObjectData> children;
	};

	std::vector<ObjectData> objects;
};

/**
 * @class ModelManager
 * @brief モデルの読み込みや管理を担当するクラス
 */
class ModelManager {
public:
	static ModelManager* GetInstance();

	//objファイルの読み込み
	static MaterialData LoadMaterialTemplateFile(const std::string& filename);
	static ModelData LoadGltfFile(const std::string& directorPath,const std::string& filename);
	static ModelData LoadObjFile(const std::string& filename);

	void LoadJsonObject(nlohmann::json& object, Scene::ObjectData& objectData);


	static Node ReadNode(aiNode* node);

	static const int maxtex = 256;


private:
	static ModelManager* instance_;
};
