#pragma once

class SceneManager;

/**
 * @class IScene
 * @brief ゲームのシーンを表現するインターフェース
 */
class IScene {
public:
    /// <summary>
    /// シーンの初期化処理を行う
    /// サブクラスでシーンごとの初期化を実装する
    /// </summary>
    virtual void Initialize() = 0;

    /// <summary>
    /// シーンの更新処理を行う
    /// サブクラスでシーンごとの更新を実装する
    /// </summary>
    virtual void Update() = 0;

    /// <summary>
    /// シーンの描画処理を行う
    /// サブクラスでシーンごとの描画を実装する
    /// </summary>
    virtual void Draw() = 0;

    /// <summary>
    /// ダイレクトに描画処理を行う
    /// </summary>
    virtual void RenderDirect() = 0;

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~IScene() = default;

    /// <summary>
    /// シーンマネージャーを設定する
    /// シーンマネージャーとの連携を行う
    /// </summary>
    /// <param name="sceneManager">シーンマネージャーのポインタ</param>
    virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; };

protected:
    SceneManager* sceneManager_ = nullptr; ///< シーンマネージャーへのポインタ
};
