#pragma once
#include "Math/math.h"
#include "Math/Quaternion.h"
#include "Manager/TextureManeger.h"
#include <GraphicsPipeline.h>
#include <Model.h>
#include <map>
#include <WorldTransform.h>
#include <ViewProjection.h>
#include <ModelManager.h>


template<typename tValue>
struct Keyframe {
	float time;
	tValue value;
};

using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;


template<typename tValue>
struct AnimationCurve {
	std::vector<Keyframe<tValue>> keyframes;
};

struct NodeAnimation {
	AnimationCurve<Vector3> translate;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> scale;
};

struct Animation{
	float duration;//アニメーション全体の尺
	//NodeAniamtionの集合,Node名でひけるようにしておく
	std::map<std::string, NodeAnimation> nodeAnimations;
};


Animation LoadAnimationFile(const std::string& directorPath, const std::string& filename);


class Animations {
public: // 静的メンバ変数



	// デスクリプタサイズ
	static UINT sDescriptorHandleIncrementSize_;

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

	BlendMode blendMode_ = BlendMode::kNormal;

public:
	

	//初期化
	void Initialize(const std::string& filename, const std::string& texturePath, const std::string& motionPath);

	
	void Update(WorldTransform& worldTransform);
	

	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection);

	//ファイルの場所//png//gltf
	static Animations* Create(const std::string& filename, const std::string& texturePath, const std::string& motionPath);
	

	

	/// <summary>
	/// グラフィックスパイプラインの初期化
	/// </summary>
	void sPipeline();

	/// <summary>
	/// 3Dモデル生成
	/// </summary>
	/// <returns></returns>
	void CreateVertexResource();

private:


	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
	// 頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	//マテリアル用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResorce_;
	// カメラ用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResorce_;


	VertexData* vertexData = nullptr;
	Material* materialData = nullptr;
	CameraForGPU* cameraData = nullptr;
	TransformationMatrix* transformData = nullptr;

	TextureManager* textureManager_;
	uint32_t texture_;

	float animationTime = 0.0f;

	ModelManager* modelManager_;
	ModelData modelData = ModelManager::LoadObjFile("./resources/AnimatedCube/AnimatedCube.gltf");
	Animation animation = LoadAnimationFile("./resources/AnimatedCube", "AnimatedCube.gltf");
private:
	void LoadAnimation(const std::string& filename, const std::string& texturePath);
	void LoadTexture(const std::string& filename);

	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

};