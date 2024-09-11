#include "GlobalVariables.h"
#include "ImGuiManager.h"
#include <fstream>
#include <iostream>
#include <json.hpp>


GlobalVariables *GlobalVariables::GetInstance() {
	static GlobalVariables globalVariables_;
	return &globalVariables_;
}

void GlobalVariables::CreateGroup(const std::string &groupName) {
	// 指定名のオブジェクトがなければ追加する
	datas_[groupName];
}

// int
void GlobalVariables::SetValue(const std::string &grouName, const std::string &key, int32_t value) {
	// グループの参照を取得
	Group &group = datas_[grouName];
	// 新しい項目のデータ
	Item newItem{};
	newItem = value;
	// 設定した項目をstd::mapに追加
	group[key] = newItem;
}

// float
void GlobalVariables::SetValue(const std::string &grouName, const std::string &key, float value) {
	// グループの参照を取得
	Group &group = datas_[grouName];
	// 新しい項目のデータ
	Item newItem{};
	newItem = value;
	// 設定した項目をstd::mapに追加
	group[key] = newItem;
}

// Vector3
void GlobalVariables::SetValue(
	const std::string &grouName, const std::string &key, const Vector3 &value) {
	// グループの参照を取得
	Group &group = datas_[grouName];
	// 新しい項目のデータ
	Item newItem{};
	newItem = value;
	// 設定した項目をstd::mapに追加
	group[key] = newItem;
}

// int
void GlobalVariables::AddItem(const std::string &grouName, const std::string &key, int32_t value) {

	// グループの参照を取得
	Group &group = datas_[grouName];
	// 項目の参照を取得
	if (group.find(key) == group.end()) {

		SetValue(grouName, key, value);
	}
}

// float
void GlobalVariables::AddItem(const std::string &grouName, const std::string &key, float value) {

	// グループの参照を取得
	Group &group = datas_[grouName];
	// 項目の参照を取得

	if (group.find(key) == group.end()) {

		SetValue(grouName, key, value);
	}
}

// Vector3
void GlobalVariables::AddItem(const std::string &grouName, const std::string &key, Vector3 &value) {
	// グループの参照を取得
	Group &group = datas_[grouName];
	// 項目の参照を取得

	if (group.find(key) == group.end()) {

		SetValue(grouName, key, value);
	}
}

void GlobalVariables::Update() {

	if (!ImGui::Begin("Global Variadles", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar())
		return;

	// 各グループについて
	for (std::map<std::string, Group>::iterator itGroup = datas_.begin(); itGroup != datas_.end();
		++itGroup) {
		// グループ名を取得
		const std::string &groupName = itGroup->first;
		// グループの参照を取得
		Group &group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str()))
			continue;

		// 各項目について
		for (std::map<std::string, Item>::iterator itItem = group.begin(); itItem != group.end();
			++itItem) {

			// 項目名を取得
			const std::string &itemName = itItem->first;
			// 項目の参照を取得
			Item &item = itItem->second;

			// int32_t型の値を保持していれば
			if (std::holds_alternative<int32_t>(item)) {
				int32_t *ptr = std::get_if<int32_t>(&item);
				ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);
			}
			// float型の値を保持していれば
			else if (std::holds_alternative<float>(item)) {
				float *ptr = std::get_if<float>(&item);
				ImGui::SliderFloat(itemName.c_str(), ptr, 0, 100);
			}
			// Vector3型の値を保持していれば
			else if (std::holds_alternative<Vector3>(item)) {
				Vector3 *ptr = std::get_if<Vector3>(&item);
				ImGui::SliderFloat3(itemName.c_str(), reinterpret_cast<float *>(ptr), -3.0f, 3.0f);
			}
		}

		ImGui::Text("\n");

		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json save.", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}

		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
	ImGui::End();
}

void GlobalVariables::SaveFile(const std::string &groupName) {

	// グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	// 未登録チェック
	assert(itGroup != datas_.end());

	nlohmann::json root;

	root = nlohmann::json::object();

	// jsonオブジェクト登録
	root[groupName] = nlohmann::json::object();

	// 各項目について
	for (std::map<std::string, Item>::iterator itItem = itGroup->second.begin();
		itItem != itGroup->second.end(); ++itItem) {

		// 項目名を取得
		const std::string &itemName = itItem->first;
		// 項目の参照を取得
		Item &item = itItem->second;

		// int32_t型の値を保持していれば
		if (std::holds_alternative<int32_t>(item)) {
			// int32_t型の値を登録
			root[groupName][itemName] = std::get<int32_t>(item);
		}
		// float型の値を保持していれば
		else if (std::holds_alternative<float>(item)) {
			// float型の値を登録
			root[groupName][itemName] = std::get<float>(item);
		}
		// Vector3型の値を保持していれば
		else if (std::holds_alternative<Vector3>(item)) {
			// float型のjson配列登録
			Vector3 value = std::get<Vector3>(item);
			// Vector3型の値を登録
			root[groupName][itemName] = nlohmann::json::array({ value.x, value.y, value.z });
		}

		// ディレクトがなければ作成する
		std::filesystem::path dir(kDirectoryPath);
		if (!std::filesystem::exists(kDirectoryPath)) {
			std::filesystem::create_directory(kDirectoryPath);
		}

		// 書き込むJSONファイルパスのフルパス合成する
		std::string filePath = kDirectoryPath + groupName + ".json";
		// 書き込み用ファイルストリーム
		std::ofstream ofs;
		// ファイル書き込み用に開く
		ofs.open(filePath);

		// ファイルオープン失敗？
		if (ofs.fail()) {
			std::string message = "Failed open data file for write.";
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
			assert(0);
			return;
		}

		// ファイルにjson文字列を書き込む
		ofs << std::setw(4) << root << std::endl;
		// ファイルを閉じる
		ofs.close();
	}
}

void GlobalVariables::LoadFile(const std::string &groupName) {

	// 読み込みJSONファイルのフルパスを合成
	std::string filePath = kDirectoryPath + groupName + ".json";
	// 書き込み用ファイルストリーム
	std::ifstream ifs;
	// ファイル書き込み用に開く
	ifs.open(filePath);
	// ファイルオープン失敗？
	if (ifs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	nlohmann::json root;

	// json文字列からjsonデータ構造に展開
	ifs >> root;

	// ファイルを閉じる
	ifs.close();

	// グループを検索
	nlohmann::json::iterator itGroup = root.find(groupName);

	// 未登録チェック
	assert(itGroup != root.end());

	// 各アイテムについて
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		// アイテム名を取得
		const std::string &itemName = itItem.key();

		// int32_t型の値を保持していれば
		if (itItem->is_number_integer()) {
			// int32_t型の値を登録
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		}
		// float型の値を保持していれば
		else if (itItem->is_number_float()) {
			// float型の値を登録
			double value = itItem->get<double>();
			SetValue(groupName, itemName, static_cast<float>(value));
		}
		// 要素数3の配列いれば
		else if (itItem->is_array() && itItem->size() == 3) {
			// float型のjson配列
			Vector3 value = {
				itItem->at(0),
				itItem->at(1),
				itItem->at(2),
			};
			SetValue(groupName, itemName, value);
		}
	}
}

void GlobalVariables::LoadFiles() {
	const std::string kDirectoryPathLoad = "Resources/GlobalVariables/";
	// ディレクトがなけれスキップ
	std::filesystem::path dir(kDirectoryPathLoad);
	if (!std::filesystem::exists(kDirectoryPathLoad)) {
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPathLoad);
	for (const std::filesystem::directory_entry &entry : dir_it) {
		// ファイルパスを取得
		const std::filesystem::path &filePath = entry.path();

		// ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}

		// ファイル読み込み
		LoadFile(filePath.stem().string());
	}
}

int32_t GlobalVariables::GetIntValue(const std::string &groupName, const std::string &key) const {
	int32_t result{};
	assert(GetValue(groupName, key, &result) and "データの読み込みに失敗しました");
	return result;
}

float GlobalVariables::GetFloatValue(const std::string &groupName, const std::string &key) const {
	float result{};
	assert(GetValue(groupName, key, &result) and "データの読み込みに失敗しました");
	return result;
}

Vector3 GlobalVariables::GetVecter3Value(const std::string &groupName, const std::string &key) const {
	Vector3 result{};
	assert(GetValue(groupName, key, &result) and "データの読み込みに失敗しました");
	return result;
}
