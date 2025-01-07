#pragma once
#include "Math/math.h"
#include <map>
#include <string>
#include <variant>


/**
 * @file GlobalVariables
 * @brief グローバル変数を管理するクラス
 */
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

	// 値のセット
	template <typename T>
	void SetValue(const std::string& grouName, const std::string& key, T& value);
	

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

	// 項目の追加
	template <typename T>
	void AddItem(const std::string& grouName, const std::string& key, T& value) {

		// グループの参照を取得
		Group& group = datas_[grouName];
		// 項目の参照を取得
		if (group.find(key) == group.end()) {

			SetValue(grouName, key, value);
		}
	}

	// 値の取得
	template <typename T>
	T GetValue(const std::string& groupName, const std::string& key) {

		// 未登録チェック
		assert(datas_.find(groupName) != datas_.end());

		// グループの参照を取得
		Group& group = datas_[groupName];

		// Vector3* ptr = std::get_if<Vector3>(&item.value);

		assert(group.find(key) != group.end());
		return std::get<T>(group[key]);
	}

private:
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables& obj) = delete;
	GlobalVariables& operator=(const GlobalVariables& obj) = delete;
};



