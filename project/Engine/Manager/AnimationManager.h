#pragma once
#include "Animation/Animation.h"

/**
 * @class AnimationManager
 * @brief アニメーションの管理を行うシングルトン
 * アニメーションの作成、管理、ライフサイクルを担当するクラス
 */
class AnimationManager {
public:
    /// <summary>
    /// シングルトンインスタンスを取得
    /// </summary>
    /// <returns>AnimationManagerのインスタンス</returns>
    static AnimationManager* GetInstance();

    /// <summary>
    /// シングルトンインスタンスを破棄
    /// </summary>
    static void Destroy();

    /// <summary>
    /// 新しいアニメーションを作成
    /// </summary>
    /// <param name="directorPath">アニメーションディレクターのパス</param>
    /// <param name="filename">アニメーションファイルの名前</param>
    /// <param name="motionPath">モーションファイルのパス</param>
    /// <returns>新しく作成したアニメーションインスタンス</returns>
    static Animations* Create(const std::string& directorPath, const std::string& filename, const std::string& motionPath);

private:
    // コンストラクタとデストラクタは非公開でシングルトンにするために使用
    AnimationManager() = default;
    ~AnimationManager() = default;

    // コピーコンストラクタと代入演算子を削除
    AnimationManager(const AnimationManager&) = delete;
    AnimationManager& operator=(const AnimationManager&) = delete;

    /// <summary>
    /// 内部でアニメーションを生成
    /// </summary>
    /// <param name="directorPath">アニメーションディレクターのパス</param>
    /// <param name="filename">アニメーションファイルの名前</param>
    /// <param name="motionPath">モーションファイルのパス</param>
    /// <returns>新しく作成したアニメーションインスタンス</returns>
    Animations* CreateInternal(const std::string& directorPath, const std::string& filename, const std::string& motionPath);

private:
    static AnimationManager* instance_; ///< シングルトンインスタンス

    /// <summary>
    /// アニメーションインスタンスを保持するためのマップ
    /// アニメーションの名前をキーにして対応するアニメーションを保持
    /// </summary>
    std::map<std::string, std::unique_ptr<Animations>> particleSystems_;
};
