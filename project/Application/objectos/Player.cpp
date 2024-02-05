#include "Player.h"

void Player::Initialize(const std::vector<Model*>& models) {
	models_ = models;

	// 初期化
	worldTransformBase_.Initialize();
}

void Player::Update(){

	
}

void Player::Draw(const ViewProjection& viewprojection, bool light) {
	models_[0]->Draw(worldTransformBase_, viewprojection,light);
}

const Vector3 Player::GetWorldPosition() const{
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];
	return worldPos;
}

void Player::OnCollision(Collider* collider) {

}
