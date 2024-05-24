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
#include <Object3d.h>

struct Scene {
	struct ObjectData {
		std::string filename;
		Transform transform;
		std::vector<Object3d*> objects;
		std::vector<ObjectData> children;
	};

	std::vector<ObjectData> objects;
};

class ModelManager {
public:
	static ModelManager* GetInstance();

	//objファイルの読み込み
	static MaterialData LoadMaterialTemplateFile(const std::string& filename);
	static ModelData LoadGltfFile(const std::string& filename);
	static ModelData LoadObjFile(const std::string& filename);

	void LoadJsonObjFile(const std::string& filename);
	

	static Node ReadNode(aiNode* node);

	static const int maxtex = 256;

private:
	void LoadJsonObject(nlohmann::json& object, Scene::ObjectData& parentObjectData);
private:
	static ModelManager* instance_;

	std::map<std::string,Model*> models;

};