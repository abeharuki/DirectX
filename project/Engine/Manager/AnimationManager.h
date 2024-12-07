#pragma once
#include "Animation/Animation.h"

/**
 * @class AnimationManager
 * @brief アニメーションの管理を行うシングルトン
 * アニメーションの作成、管理、ライフサイクルを担当するクラス
 */
class AnimationManager {
public:
	static AnimationManager* GetInstance();

	static void Destroy();

	static Animations* Create(const std::string& directorPath, const std::string& filename, const std::string& motionPath);

private:
	AnimationManager() = default;
	~AnimationManager() = default;
	AnimationManager(const AnimationManager&) = delete;
	AnimationManager& operator=(const AnimationManager&) = delete;

	Animations* CreateInternal(const std::string& directorPath, const std::string& filename, const std::string& motionPath);
private:
	static AnimationManager* instance_;

	std::map<std::string, std::unique_ptr<Animations>> particleSystems_;
};