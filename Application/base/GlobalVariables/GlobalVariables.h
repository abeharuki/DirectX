#pragma once
#include "Math/math.h"
#include <map>
#include <string>
#include <variant>

class GlobalVariables {
public:
	static GlobalVariables* GetInstance();

	// 項目
	using Item = std::variant<int32_t, float, Vector3>;

	// グループ
	using Group = std::map<std::string, Item>;

	// 全データ
	std::map<std::string, Group> datas_;

	// グループの作成
	void CreateGroup(const std::string& groupName);

	// 値のセット(int)
	void SetValue(const std::string& grouName, const std::string& key, int32_t value);
	// 値のセット(float)
	void SetValue(const std::string& grouName, const std::string& key, float value);
	// 値のセット(Vector3)
	void SetValue(const std::string& grouName, const std::string& key, Vector3& value);

	// 毎フレーム処理
	void Updeat();

	// ファイルに書き出し
	void SaveFile(const std::string& groupName);

	// グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/GlobalVariables/";

	// ディレクトの全ファイル読み込み
	void LoadFiles();

	// ファイルから読み込み
	void LoadFile(const std::string& groupName);

	// 項目の追加(int)
	void AddItem(const std::string& grouName, const std::string& key, int32_t value);
	// 項目の追加(float)
	void AddItem(const std::string& grouName, const std::string& key, float value);
	// 項目の追加(Vector3)
	void AddItem(const std::string& grouName, const std::string& key, Vector3& value);

	// 値の取得
	int32_t GetIntValue(const std::string& groupName, const std::string& key);
	float GetFloatIntValue(const std::string& groupName, const std::string& key);
	Vector3 GetVecter3Value(const std::string& groupName, const std::string& key);

private:
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables& obj) = delete;
	GlobalVariables& operator=(const GlobalVariables& obj) = delete;
};
