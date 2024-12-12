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
#include <Line.h>
#include "ImGuiManager.h"
#include <PostEffects/Dissolve.h>

/**
 * @brief アニメーションのキーフレームを表す構造体
 * @tparam tValue キーフレームで保持するデータ型
 */
template<typename tValue>
struct Keyframe {
	float time;
	tValue value;
};

using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

/**
 * @brief アニメーションのカーブ（補間）のためのキーフレームを保持する構造体
 * @tparam tValue キーフレームで保持するデータ型（位置、回転、スケールなど）
 * @details アニメーションの進行に合わせて、時間ごとの値を補間するためのキーフレームを格納
 */
template<typename tValue>
struct AnimationCurve {
	std::vector<Keyframe<tValue>> keyframes;
};

/**
 * @brief ノードに関連するアニメーションデータを保持する構造体
 * @details ノードの位置、回転、スケールのアニメーションを表現するための構造体
 */
struct NodeAnimation {
	AnimationCurve<Vector3> translate;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> scale;
};

/**
 * @brief アニメーション全体のデータを管理する構造体
 * @details アニメーションの全体的な情報（尺）と、各ノードのアニメーション情報（位置、回転、スケール）を保持
 */
struct Animation {
	float duration;//アニメーション全体の尺
	//NodeAniamtionの集合,Node名でひけるようにしておく
	std::map<std::string, NodeAnimation> nodeAnimations;
};

/**
 * @brief スキニングに関する情報を保持する構造体
 * @details スキニング（頂点の変形）に必要な情報を保持するための構造体
 */
struct SkinningInformation {
	uint32_t numVertices;
};


/**
 * @file Animation
 * @brief アニメーション処理とレンダリングを管理するクラス
 * @details このクラスは3Dアニメーションの処理、スキンニング、ライティング、シェーダーの管理など、ゲームやシミュレーションで使用されるアニメーション全体の管理
 */
class Animations {
public: // 静的メンバ変数

	struct WritingStyle {
		DirectionLight directionLight_;
		PointLight pointLight_;
		SpotLight spotLight_;
		Environment  environment_;
		DissolveStyle dissolve_;
	};

	// デスクリプタサイズ
	static UINT sDescriptorHandleIncrementSize_;
	
	BlendMode blendMode_ = BlendMode::kNormal;

public:


	//初期化
	void Initialize(const std::string& filename, const std::string& texturePath, const std::string& motionPath);
	void Initialize(const std::string& directorPath,const std::string& motionPath);

	//毎フレームのskinningAnimation更新
	void Update(const uint32_t animationNumber);

	//GPU初期化
	void ComputeParameter();

	//描画
	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection, bool flag);

	//アニメーションデバックImGui描画
	void AnimationDebug();

	//アニメーションの作成
	static Animations* Create(const std::string& filename, const std::string& texturePath, const std::string& motionPath);
	static Animations* Create(const std::string& filename, const std::string& motionPath);

	/// <summary>
	/// グラフィックスパイプラインの初期化
	/// </summary>
	void sPipeline();

	/// <summary>
	/// 3Dモデル生成
	/// </summary>
	/// <returns></returns>
	void CreateResource();

	/*-----------------------------セッター---------------------------------*/

	//光の色　向き　明るさ
	void DirectionalLightDraw(DirectionLight directionLight);
	//ポイントライトの詳細　向き
	void PointLightDraw(PointLight pointLight, Vector3 direction);
	//スポットライト
	void SpotLightDraw(SpotLight spotLight);
	//映り込み度
	void SetEnvironment(float environment, bool flag) {
		if (Skybox::textureNum != 0) {
			lightData->environment_.isEnble_ = flag;
		}
		lightData->environment_.environment = environment;
	}	
	//止める
	void Stop(bool flag) {
		stop_ = flag;
	}
	//ループさせるかどうか
	void SetLoop(bool flag) { isLoop_ = flag; }
	

	//補間するアニメーションナンバーの設定
	void SetAnimatioNumber(uint32_t nextAnimation,uint32_t animationNum) { 
		preAnimationNumber_ = animationNum;
		animationNumber_ = nextAnimation;
	}

	//void isDissolve(bool flag) { lightData->dissolve_.isEnble = flag; }
	void SetThreshold(float num) { lightData->dissolve_.threshold = num; }
	void SetEdgeColor(Vector3 color) { lightData->dissolve_.edgeColor = color; }

	//アニメーションタイマーの設定
	void SetAnimationTimer(float startTime, float flameTime);
	void SetFlameTimer(float time) {flameTimer_ = time; }
	void SetpreAnimationTimer(float time) { animationTime = time; }
	float GetAnimationTimer() { return  animationTime; }

	//ブレンドモードの設定
	void SetBlend(const uint32_t animationNumber,float num);

	//テクスチャの設定
	void SetTexture(const std::string& texturePath);
	//マスクテクスチャの設定
	void SetMaskTexture(const std::string& texturePath);

	//指定したジョイントの座標受け取り
	const WorldTransform& GetJointWorldTransform(const std::string& name)const;
private:
	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CSRootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CSPipelineState_;

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

	//スキニングの情報
	Microsoft::WRL::ComPtr<ID3D12Resource> skinningInformation;
	SkinningInformation* skinningData;

	// ライティング
	Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
	WritingStyle* lightData;

	//メッシュデータ
	std::unique_ptr<Mesh> meshData_;
	//マテリアルデータ
	std::unique_ptr<Material> materialData_;

	// カメラ用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;

	CameraForGPU* cameraData = nullptr;
	TransformationMatrix* transformData = nullptr;

	// リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> dissolveResource_;
	DissolveStyle* dissolveData = nullptr;

	//テクスチャ
	TextureManager* textureManager_;
	uint32_t texture_;
	uint32_t maskTexture_;

	float animationTime = 0.0f;
	float nextAnimationTime = 0.0f;
	float flameTimer_ = 0.0f;
	bool stop_ = false;

	ModelManager* modelManager_;
	ModelData modelData;
	std::vector<Animation> animation{};
	Skeleton skeleton;
	SkinCluster skinCluster;

	uint32_t animationNumber_;
	uint32_t preAnimationNumber_;

	//補間のための情報
	bool isBlending_;
	float blendTime_;
	float blendDuration_= 0.2f;//何秒かけて補間するか

	//ジョイントのワールドトランスフォーム
	std::vector<WorldTransform> jointWorldTransforms_{};

	//デバック表示
	std::vector<std::unique_ptr<Line>> line_;
	std::vector<Vector3> boonPos;
	std::vector<Vector3> boonPos2;
	int jointsNum_;
	bool debug_ = false;
	bool isLoop_ = true;
	Matrix4x4 localMatrix;
private:
	std::vector<Animation> LoadAnimationFile(const std::string& directorPath, const std::string& filename);

	void LoadAnimation(const std::string& filename, const std::string& texturePath);
	void LoadTexture(const std::string& filename);
	void ApplyAnimation(const std::string& name,const uint32_t animationNumber);

	void SkeletonUpdate(const WorldTransform& worldTransform);
	void SkinningUpdate();

	void BoonRecursive(Skeleton& skeleton, int32_t child);

	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

	void CreateJointWorldTransforms();

};

	