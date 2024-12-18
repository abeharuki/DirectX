# アプリケーション
## タイトル
「DRAPONQUEST」<br>
<img src="https://github.com/user-attachments/assets/06c14d46-c4b2-478c-aeef-557a9d589e8c" alt="DRAPONQUEST" width="500" /><br>

[![DebugBuild](https://github.com/abeharuki/DirectX/actions/workflows/DebugBuild.yml/badge.svg)](https://github.com/abeharuki/DirectX/actions/workflows/DebugBuild.yml)
[![ReleaseBuild](https://github.com/abeharuki/DirectX/actions/workflows/ReleaseBuild.yml/badge.svg)](https://github.com/abeharuki/DirectX/actions/workflows/ReleaseBuild.yml)
## 概要
今の作品を作ろうと思った経緯は、人と一緒にゲームをするのも好きなのですが、
ドラゴンズドグマやドラクエ10のようなcpuと一緒に戦うゲームが、
仲間を自分が引き連れている感じがあって好きなので、就職作品としてドラクエ10の様な作品を作ろうと思い作っています。<br>
<br>
進捗は以下の通りです。
<br>

* 味方AIの実装(ある程度自動で動いてくれる)
* 敵AIの実装
* 各味方キャラごとのスキルの実装
* 味方AIをビヘイビアツリーを使用して管理
* 画面全体の絵作り(UI)
* エンカウント時の演出 など
* ImNodesをエディタとして使用し味方AIの状態遷移を管理
  - ImNodeエディタではノードの追加やリンクの削除などが行えます<br>

<img src="https://github.com/user-attachments/assets/d5f5ac05-6d9c-4e19-9da2-27de4c19e361" alt="アニメーション" width="500" />
<img src="https://github.com/user-attachments/assets/8c8e4516-a2bb-4bb4-b983-bed9f78eca04" alt="アニメーション" width="500" />
<br><br>
さらに細かい進捗は下記のURLから確認できます。<br>

  [作品動画はこちら](https://youtu.be/nyvwBBJCtAM)<br>[ポートフォリオはこちら](https://drive.google.com/file/d/1Y0W_oBFEl7xn3lC0QJBoP-e7Hye1SvJ5/view?usp=drive_link)<br>


# エンジン
DrectX12で自作しているエンジンです。2023年4月頃からエンジン部分を作り始め、今はこの自作エンジンを使用して就職作品を作っています。<br>
<br>
主な機能は次の通りです。
<br>
## 機能
<div style="display: flex; flex-wrap: wrap; justify-content: space-around;">
  <img src="https://github.com/user-attachments/assets/52fc32ec-2598-4a4d-92ae-66cc1165ea5f" alt="blender" width="200" style="margin: 10px;"/>
  <img src="https://github.com/user-attachments/assets/0e75b7c1-f854-4eca-be66-96275f3320b8" alt="DRAPONQUEST" width="200" style="margin: 10px;"/>
  <img src="https://github.com/user-attachments/assets/a5ace2ad-8a5a-4fb0-890c-a4c6f10bcc0f" alt="particle" width="200" style="margin: 10px;"/>
  <img src="https://github.com/user-attachments/assets/5750b5a1-33a2-426d-b0f0-650bfc8f1e22" alt="particle" width="80" style="margin: 10px;"/>
</div>
<br><br>

* スプライトの読み込み
* 3Dオブジェクトの描画
* 3Dアニメーションの再生
* コライダーの実装
* サウンドの再生
* GPUParticle<br>  
* blenderをステージエディタとして使うためのローダー
* PostEffect
  - アウトライン
  - ブルーム
  - ディゾルブ
  - トゥーンシェーダー など。<br>
   これらのシェーダーは、ドラクエ10の様なアニメ調の見た目に寄せるために実装しました。

## Windows SDKバージョン　10.0
## 外部ライブラリ
  - [DirextXTex](https://github.com/microsoft/DirectXTex/releases/tag/mar2023)
  - [assimp](https://github.com/assimp/assimp)
  - [imgui](https://github.com/ocornut/imgui/releases/tag/v1.89.4)
  - [imnode](https://github.com/Nelarius/imnodes/tree/master)
  - [nlohmann](https://github.com/nlohmann/json)
  
