#pragma once
#include "IScene.h"
#include "AbstractSceneFactory.h"
#include <imgui.h>

/**
 * @class SceneManager
 * @brief シーンの管理と遷移を担当するシングルトンクラス
 */
class SceneManager {
public:
    /// <summary>
    /// シングルトンインスタンスを取得する
    /// </summary>
    /// <returns>SceneManagerのインスタンス</returns>
    static SceneManager* GetInstance();

    /// <summary>
    /// SceneManagerのインスタンスを破棄する
    /// </summary>
    static void Destroy();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~SceneManager();

    /// <summary>
    /// 現在のシーンを更新する
    /// </summary>
    void Update();

    /// <summary>
    /// 現在のシーンを描画する
    /// </summary>
    void Draw();

    /// <summary>
    /// ダイレクトに描画を行う
    /// </summary>
    void RenderDirect();

    /// <summary>
    /// ローディング画面を表示する
    /// </summary>
    void Loading();

    /// <summary>
    /// シーンをロードする
    /// </summary>
    void LoadScene();

    /// <summary>
    /// 最初のシーンを設定する
    /// </summary>
    /// <param name="sceneName">最初に読み込むシーンの名前</param>
    void FirstScene(const std::string& sceneName);

    /// <summary>
    /// シーンを変更する
    /// </summary>
    /// <param name="sceneName">変更するシーンの名前</param>
    void ChangeScene(const std::string& sceneName);

    /// <summary>
    /// シーンファクトリーを設定する
    /// </summary>
    /// <param name="sceneFactory">シーンファクトリーのポインタ</param>
    void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; };

    /// <summary>
    /// シーンが変更されているかどうかを確認する
    /// </summary>
    /// <returns>シーン変更フラグ</returns>
    bool ChangeScene() { return load_; }

    /// <summary>
    /// デバッグ用にシーンの変更を行う
    /// ImGuiを使用してシーンを選択し、変更を行う
    /// </summary>
    void DebugSceneChange() {
        const char* items[] = { "TitleScene", "GameScene", "ClearScene", "OverScene", "DebugScene" };
        if (!load_) {
            if (currentItem != preCurrentItem) {
                preCurrentItem = currentItem;
                ChangeScene(items[currentItem]);  // 配列からシーン名を取得
            }

            // ImGuiでシーン選択のコンボボックスを表示
            ImGui::Begin("Scene");
            ImGui::Combo("##combo", &currentItem, items, IM_ARRAYSIZE(items));
            ImGui::End();
        }
    }

private:
    static SceneManager* instance_; ///< SceneManagerのシングルトンインスタンス

    IScene* currentScene_ = nullptr; ///< 現在のシーン
    IScene* nextScene_ = nullptr;    ///< 次に切り替えるシーン

    IScene* loadScene_ = nullptr;    ///< ローディング画面
    bool load_ = false;              ///< シーンが変更中かどうかのフラグ

    AbstractSceneFactory* sceneFactory_ = nullptr; ///< シーンを生成するファクトリー

    int currentItem = 0;  ///< 初期選択アイテムのインデックス
    int preCurrentItem = 0; ///< 前回選択されていたアイテムのインデックス
};
