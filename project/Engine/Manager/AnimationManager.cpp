#include "AnimationManager.h"

AnimationManager* AnimationManager::instance_ = nullptr;

AnimationManager* AnimationManager::GetInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = new AnimationManager();
    }
    return instance_;
}

void AnimationManager::Destroy()
{
    if (instance_ != nullptr)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

Animations* AnimationManager::Create(const std::string& directorPath, const std::string& filename, const std::string& motionPath)
{
    Animations* animation = AnimationManager::GetInstance()->CreateInternal(directorPath, filename, motionPath);
    return animation;
}


Animations* AnimationManager::CreateInternal(const std::string& directorPath, const std::string& filename, const std::string& motionPath)
{
   
    // リソースが既に存在するかを検索
    auto it = particleSystems_.find(motionPath);
    if (it != particleSystems_.end()) {
        // 既存のリソースを返す
        return it->second.get();
    }

    // 新規リソースを作成
    auto animation = std::make_unique<Animations>();
    animation->Initialize(directorPath, filename, motionPath);
    Animations* rawPtr = animation.get(); // 生ポインタを取得

    // 新しいリソースをマップに追加
    particleSystems_[motionPath] = std::move(animation);

    // 新規リソースを返す
    return rawPtr;
}

