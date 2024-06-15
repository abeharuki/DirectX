#include "Material.h"
#include <Mesh.h>
#include <Engine.h>

void Material::Initialize() {

	//更新
	Update();
}

void Material::Update() {
	// マテリアル
	materialResorce_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(MaterialD));

	// マテリアルにデータを書き込む
	// 書き込むためのアドレスを取得
	materialResorce_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 今回は白を書き込む
	materialData->color.rgb = Vector3(1.0f, 1.0f, 1.0f);
	materialData->color.a = float(1.0f);
	// Lightingを有効にする
	materialData->enableLighting = false;
	//光沢
	materialData->shininess = 10.0f;
	// 初期化
	materialData->uvTransform = Math::MakeIdentity4x4();
}


