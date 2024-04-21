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

struct Animation {
	float duration;//アニメーション全体の尺
	//NodeAniamtionの集合,Node名でひけるようにしておく
	std::map<std::string, NodeAnimation> nodeAnimations;
};

struct Joint {
	QuaternionTransform transform;//Transform情報
	Matrix4x4 locaalMatrix;//localMatrix
	Matrix4x4 skeletonSpaceMatrix;//skeletonSpaceでの変換行列
	std::string name;//名前
	std::vector<int32_t>children;//子JointnoIndexのリスト
	int32_t index;//自身のIndex
	std::optional<int32_t>parent;//親JointのIndex
};

struct Skeleton {
	int32_t root;
	std::map<std::string, int32_t>jointMap;//Joint名とIndexの辞書
	std::vector<Joint>joints;//所属しているジョイント
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
	void Update();

	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection);


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
private:
	void LoadAnimation(const std::string& filename, const std::string& texturePath);
	void LoadTexture(const std::string& filename);
	void SkeletonUpdate(Skeleton& skeleton);
	void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);

	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);
	Skeleton CreateSkeleton(const Node& rootNode);
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
};