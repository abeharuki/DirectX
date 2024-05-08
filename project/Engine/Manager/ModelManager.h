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


class ModelManager {
public:

	//objファイルの読み込み
	static MaterialData LoadMaterialTemplateFile(const std::string& filename);
	static ModelData LoadGltfFile(const std::string& filename);
	static ModelData LoadObjFile(const std::string& filename);

	static Node ReadNode(aiNode* node);

	static const int maxtex = 256;


private:

};
