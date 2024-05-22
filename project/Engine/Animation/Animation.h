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
#include <span>
#include <array>
#include "Skeleton.h"
#include "Skinning.h"
#include "Line.h"
#include "ImGuiManager.h"
#include <Skybox.h>

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

struct Animation {
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

	// ライティング
	static Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
	static WritingStyle* lightData;

	BlendMode blendMode_ = BlendMode::kNormal;

public:


	//初期化
	void Initialize(const std::string& filename, const std::string& texturePath, const std::string& motionPath);

	//Animation
	void Update(WorldTransform& worldTransform);
	//skinningAnimation
	void Update();

	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection,bool flag);

	void AnimationDebug();

	static Animations* Create(const std::string& filename, const std::string& texturePath, const std::string& motionPath);

	//光の色　向き　明るさ
	static void DirectionalLightDraw(DirectionLight directionLight);
	//ポイントライトの詳細　向き
	static void PointLightDraw(PointLight pointLight, Vector3 direction);
	//スポットライト
	static void SpotLightDraw(SpotLight spotLight);
	//映り込み度
	void Environment(float environment,bool flag) { 
		if (Skybox::textureNum != 0) {
			lightData->environment_.isEnble_ = flag;
		}
		lightData->environment_.environment = environment; }

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
	ModelData modelData;
	Animation animation;
	Skeleton skeleton;
	SkinCluster skinCluster;

	//デバック表示
	std::vector<std::unique_ptr<Line>> line_;
	std::vector<Vector3> boonPos;
	std::vector<Vector3> boonPos2;
	int jointsNum_;
	bool debug_ = false;

private:
	void LoadAnimation(const std::string& filename, const std::string& texturePath);
	void LoadTexture(const std::string& filename);
	void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);

	void SkeletonUpdate(Skeleton& skeleton);
	void SkinningUpdate(SkinCluster& skinCluster, Skeleton& skeleton);

	void BoonRecursive(Skeleton& skeleton,int32_t child);

	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);
	
};