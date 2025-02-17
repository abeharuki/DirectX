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
 * @brief シーン内のPlayerを管理する構造体
 */
struct PlayerSpawnData {
    Vector3 translation;
    Vector3 rotation;
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
    //std::vector<PlayerSpawnData> objects;
};

/**
 * @class ModelManager
 * @brief モデルの読み込みや管理を担当するクラス
 */
class ModelManager {
public:
    /// <summary>
    /// シングルトンインスタンスを取得
    /// </summary>
    /// <returns>ModelManagerのインスタンス</returns>
    static ModelManager* GetInstance();

    /// <summary>
    /// objファイルからマテリアルテンプレートを読み込む
    /// </summary>
    /// <param name="filename">マテリアルテンプレートファイル名</param>
    /// <returns>読み込まれたマテリアルデータ</returns>
    static MaterialData LoadMaterialTemplateFile(const std::string& filename);

    /// <summary>
    /// glTFファイルを読み込み、モデルデータを生成
    /// </summary>
    /// <param name="directorPath">ディレクトリのパス</param>
    /// <param name="filename">glTFファイル名</param>
    /// <returns>読み込まれたモデルデータ</returns>
    static ModelData LoadGltfFile(const std::string& directorPath, const std::string& filename);

    /// <summary>
    /// objファイルを読み込み、モデルデータを生成
    /// </summary>
    /// <param name="filename">objファイル名</param>
    /// <returns>読み込まれたモデルデータ</returns>
    static ModelData LoadObjFile(const std::string& filename);

    /// <summary>
    /// JSON形式のオブジェクトデータを読み込み、シーンに設定
    /// </summary>
    /// <param name="object">JSONオブジェクト</param>
    /// <param name="objectData">シーンのオブジェクトデータ</param>
    void LoadJsonObject(nlohmann::json& object, Scene::ObjectData& objectData);

    /// <summary>
    /// aiNodeからNodeデータを読み取る
    /// </summary>
    /// <param name="node">aiNode</param>
    /// <returns>生成されたNodeデータ</returns>
    static Node ReadNode(aiNode* node);

    /// <summary>
    /// 最大テクスチャ数
    /// </summary>
    static const int maxtex = 256;

private:
    /// <summary>
    /// シングルトンインスタンス
    /// </summary>
    static ModelManager* instance_;
};
