#pragma once
#include <Engine.h>
#include "SceneManager.h"
#include "PostEffects/PostEffect.h"

/**
 * @class Framework
 * @brief ゲームアプリケーションのフレームワークを提供するクラス
 */
class Framework {
public:
    /// <summary>
    /// ゲームの初期化処理
    /// サブクラスでオーバーライドして、ゲームのセットアップを行う
    /// </summary>
    virtual void Initialize();

    /// <summary>
    /// ゲームの更新処理
    /// サブクラスでオーバーライドして、ゲームの状態を更新する
    /// </summary>
    virtual void Update();

    /// <summary>
    /// ゲームの描画処理
    /// サブクラスでオーバーライドして、ゲームの描画を行う
    /// </summary>
    virtual void Draw();

    /// <summary>
    /// ゲームの終了処理
    /// ゲームの終了時にリソースの解放などを行う
    /// </summary>
    void Finalize();

    /// <summary>
    /// ゲームの実行ループ
    /// ゲームの初期化から、更新・描画・終了までのループを開始する
    /// </summary>
    void Run();

protected:
    SceneManager* sceneManager_ = nullptr; ///< シーンの管理を行うSceneManagerオブジェクトへのポインタ
    AbstractSceneFactory* sceneFactory_ = nullptr; ///< シーン生成を行うファクトリクラスへのポインタ
    PostEffect* postEffect_ = nullptr; ///< ポストエフェクトを管理するオブジェクトへのポインタ
    bool nowLoading = false; ///< ゲームが現在読み込み中かどうかを示すフラグ
};
