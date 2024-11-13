#pragma once
#include "Model.h"
#include "Renju.h"
#include <ParticleSystem.h>


class RenjuManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);

	/*-----ゲッター-----*/
	Renju* GetRenju() { return renju_.get(); };

private:
	std::unique_ptr<Animations>animation_;
	std::unique_ptr<Model> bowModel_;
	std::unique_ptr<Renju> renju_;
};
