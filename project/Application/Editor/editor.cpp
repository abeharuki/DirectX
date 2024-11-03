#define _CRT_SECURE_NO_WARNINGS
#include <imnodes.h>
#include <imgui.h>

#include <algorithm>
#include <cassert>
#include <fstream>
#include <ios> // for std::streamsize
#include <stddef.h>
#include <vector>
#include <string>
#include <iostream>


namespace NodeEditor
{
	namespace
	{
		
		enum class NodeType
		{
			BasicNode,
			DoubleOutputNode, // 2出力ポート
			TripleOutputNode, // 3出力ポート
			NoOutputNode      // 0出力ポート
		};


		struct Node
		{
			int   id;
			std::string name; // ノード名を追加
			NodeType type; // ノードの種類を追加
			std::vector<std::string> outputNames; // 出力ポートの名前を保存するベクター


			Node() = default;
			Node(int i, const std::string& n, NodeType t, const std::vector<std::string>& outputNames): id(i), name(n), type(t), outputNames(outputNames) {}
		};

		struct Link
		{
			int id;
			int start_attr, end_attr;
		};

		class SaveLoadEditor
		{
		public:
			SaveLoadEditor() : nodes_(), links_(), current_id_(0), new_node_name("Node"), selected_type(NodeType::BasicNode) {}

			void show()
			{

				bool node_deleted = false;
				static std::vector<std::string> outputNames;
				ImGui::Begin("Save & load");
				ImGui::TextUnformatted("Press N to add a node.");
				
				ImNodes::BeginNodeEditor();

				

				// ノード追加用のポップアップを表示
				if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
					ImNodes::IsEditorHovered() && ImGui::IsKeyReleased(ImGuiKey_N))
				{
					// ノード名入力のポップアップを開く
					ImGui::OpenPopup("Add Node");
				}

				// ノード追加のポップアップ
				if (ImGui::BeginPopup("Add Node"))
				{
					
					ImGui::InputText("Node Name", new_node_name, sizeof(new_node_name));

					// ノードの種類を選択するUI要素
					if (ImGui::RadioButton("Basic Node", selected_type == NodeType::BasicNode))
						selected_type = NodeType::BasicNode;
					ImGui::SameLine();
					if (ImGui::RadioButton("DoubleOutput Node", selected_type == NodeType::DoubleOutputNode))
						selected_type = NodeType::DoubleOutputNode;
					ImGui::SameLine();
					if (ImGui::RadioButton("TripleOutput Node", selected_type == NodeType::TripleOutputNode))
						selected_type = NodeType::TripleOutputNode;
					ImGui::SameLine();
					if (ImGui::RadioButton("NoOutput Node", selected_type == NodeType::NoOutputNode))
						selected_type = NodeType::NoOutputNode;

					
					if (selected_type != NodeType::NoOutputNode) {
						int outputCount = (selected_type == NodeType::BasicNode) ? 1 : (selected_type == NodeType::DoubleOutputNode ? 2 : 3);
						outputNames.resize(outputCount); // 出力名のベクターをサイズ変更

						for (int i = 0; i < outputCount; ++i) {
							char outputName[128] = ""; // ローカルに文字列を宣言
							if (i < outputNames.size()) {
								strncpy_s(outputName, outputNames[i].c_str(), sizeof(outputName)); // 既存の名前をコピー
							}
							ImGui::InputText(("Output Name " + std::to_string(i + 1)).c_str(), outputName, sizeof(outputName));
							outputNames[i] = std::string(outputName); // 入力された名前をベクターに保存
						}
					}


					if (ImGui::Button("Create Node"))
					{
						const int node_id = ++current_id_;
						ImNodes::SetNodeScreenSpacePos(node_id, ImGui::GetMousePos());
						nodes_.push_back(Node(node_id, std::string(new_node_name), selected_type, outputNames));
						new_node_name[0] = '\0'; // 名前をリセット
						outputNames.clear(); // 出力名をクリア
						ImGui::CloseCurrentPopup(); // ポップアップを閉じる
					}
					ImGui::EndPopup();
				}


				for (auto iter = nodes_.begin(); iter != nodes_.end();)
				{
					Node& node = *iter;
					ImNodes::BeginNode(node.id);

					// ノードタイトルバーに名前とタイプを表示
					ImNodes::BeginNodeTitleBar();
					ImGui::Text("%s (%s)", node.name.c_str(),
						node.type == NodeType::BasicNode ? "Basic" :
						node.type == NodeType::DoubleOutputNode ? "Double Output" :
						node.type == NodeType::TripleOutputNode ? "Triple Output" :
						"No Output");
					ImNodes::EndNodeTitleBar();

					// 共通の入力属性
					ImNodes::BeginInputAttribute(node.id << 8);
					ImGui::TextUnformatted("input");
					ImNodes::EndInputAttribute();

					// NodeTypeによる出力属性の条件分岐
					if (node.type == NodeType::BasicNode) {
						ImNodes::BeginOutputAttribute(node.id << 24);
						const float text_width = ImGui::CalcTextSize("output 1").x; // 基本的な出力名の幅を計算
						ImGui::Indent(120.f - text_width); // ここでインデントを調整
						ImGui::TextUnformatted(node.outputNames.empty() ? "output 1" : node.outputNames[0].c_str());
						ImNodes::EndOutputAttribute();
					}
					else if (node.type == NodeType::DoubleOutputNode) {
						for (int i = 0; i < 2; ++i) {
							ImNodes::BeginOutputAttribute((node.id << 24) + i);
							const float text_width = ImGui::CalcTextSize(node.outputNames.size() > i ? node.outputNames[i].c_str() : ("output " + std::to_string(i + 1)).c_str()).x;
							ImGui::Indent(120.f - text_width); // インデントを調整
							ImGui::TextUnformatted(node.outputNames.size() > i ? node.outputNames[i].c_str() : ("output " + std::to_string(i + 1)).c_str());
							ImNodes::EndOutputAttribute();
						}
					}
					else if (node.type == NodeType::TripleOutputNode) {
						for (int i = 0; i < 3; ++i) {
							ImNodes::BeginOutputAttribute((node.id << 24) + i);
							const float text_width = ImGui::CalcTextSize(node.outputNames.size() > i ? node.outputNames[i].c_str() : ("output " + std::to_string(i + 1)).c_str()).x;
							ImGui::Indent(120.f - text_width); // インデントを調整
							ImGui::TextUnformatted(node.outputNames.size() > i ? node.outputNames[i].c_str() : ("output " + std::to_string(i + 1)).c_str());
							ImNodes::EndOutputAttribute();
						}
					}




					ImNodes::EndNode();

					//if (ImNodes::IsNodeSelected(node.id)) {
					//	ImGui::OpenPopup("Edit Node"); // ノードが選択されたらポップアップを開く
					//}

					//if (ImGui::BeginPopup("Edit Node")) {
					//	for (size_t i = 0; i < node.outputNames.size(); ++i) {
					//		char buffer[128];
					//		strncpy_s(buffer, sizeof(buffer), node.outputNames[i].c_str(), _TRUNCATE);
					//		if (ImGui::InputText(("Output Name " + std::to_string(i + 1)).c_str(), buffer, sizeof(buffer))) {
					//			node.outputNames[i] = buffer; // 名前を更新
					//		}
					//	}
					//	ImGui::EndPopup();
					//}

					// //NodeTypeによる出力属性の条件分岐
					//for (size_t i = 0; i < node.outputNames.size(); ++i) {
					//	ImNodes::BeginOutputAttribute((node.id << 24) + int(i));
					//	ImGui::TextUnformatted(node.outputNames[i].c_str()); // ユーザーが指定した名前を表示
					//	ImNodes::EndOutputAttribute();
					//}


					// 右クリックで削除ポップアップを開く
					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
					{
						ImGui::OpenPopup(("Node Context Menu " + std::to_string(node.id)).c_str());
					}

					// ノード削除用のコンテキストメニュー
					if (ImGui::BeginPopup(("Node Context Menu " + std::to_string(node.id)).c_str()))
					{
						if (ImGui::MenuItem("Delete Node"))
						{

							// ノードの選択をクリア
							if (ImNodes::IsNodeSelected(node.id)) {
								ImNodes::ClearNodeSelection(node.id);
							}

							// ノードを削除
							iter = nodes_.erase(iter); // ノード削除
							node_deleted = true;
							ImGui::EndPopup(); // ポップアップを閉じる
							continue;
						}
						ImGui::EndPopup();
					}


					++iter; // 次のノードに進む
			
				}

				//リンクの描画
				for (const Link& link : links_)
				{
					// リンクの始点ノードが存在するか確認
					bool startExists = std::any_of(nodes_.begin(), nodes_.end(), [link](const Node& node) {
						return node.id == (link.start_attr >> 24); // 出力属性用
						});

					// リンクの終点ノードが存在するか確認
					bool endExists = std::any_of(nodes_.begin(), nodes_.end(), [link](const Node& node) {
						return node.id == (link.end_attr >> 8); // 入力属性用
						});

					if (startExists && endExists) {
						ImNodes::Link(link.id, link.start_attr, link.end_attr);

						// 始点のノード情報の受け取り
						int startNodeId = (link.start_attr >> 24);
						auto startNode = std::find_if(nodes_.begin(), nodes_.end(), [startNodeId](const Node& node) {
							return node.id == startNodeId;
							});

						if (startNode != nodes_.end()) {
							std::string startNodeName = startNode->name;
							
						}

						// 終点のノード情報の受け取り
						int endNodeId = (link.end_attr >> 8);
						auto endNode = std::find_if(nodes_.begin(), nodes_.end(), [endNodeId](const Node& node) {
							return node.id == endNodeId;
							});

						if (endNode != nodes_.end()) {
							std::string endNodeName = endNode->name;
							
						}


					}
					
				}

				

				ImNodes::EndNodeEditor();

				// リンク作成処理
				if (!node_deleted)
				{
					Link link;
					if (ImNodes::IsLinkCreated(&link.start_attr, &link.end_attr))
					{
						link.id = ++current_id_;
						links_.push_back(link);
					}
				}

				{
					int link_id;
					if (ImNodes::IsLinkDestroyed(&link_id))
					{
						auto iter =
							std::find_if(links_.begin(), links_.end(), [link_id](const Link& link) -> bool {
							return link.id == link_id;
								});
						assert(iter != links_.end());
						links_.erase(iter);
					}
				}

				ImGui::End();
			}


			//セーブするファイル名
			void save(const std::string& filename) {

				// ファイルパスを作成
				std::string ini_filepath = "Application/Editor/" + filename + ".ini";
				std::string bytes_filepath = "Application/Editor/" + filename + ".bytes";

				//状態をセーブ
				ImNodes::SaveCurrentEditorStateToIniFile(ini_filepath.c_str());

				//エディタの状態をバイナリファイルに書き出す
				std::fstream fout(bytes_filepath, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

				//ノード情報をコピー
				const size_t num_nodes = nodes_.size();
				fout.write(reinterpret_cast<const char*>(&num_nodes), static_cast<std::streamsize>(sizeof(size_t)));
				for (const Node& node : nodes_) {
					fout.write(reinterpret_cast<const char*>(&node.id), sizeof(int));
					fout.write(reinterpret_cast<const char*>(&node.type), sizeof(NodeType));
					
					// ノード名のセーブ
					size_t name_size = node.name.size();
					fout.write(reinterpret_cast<const char*>(&name_size), sizeof(size_t));
					fout.write(node.name.c_str(), name_size);

					// 出力名のセーブ
					size_t output_count = node.outputNames.size();
					fout.write(reinterpret_cast<const char*>(&output_count), sizeof(size_t));
					for (const auto& outputName : node.outputNames) {
						size_t output_name_size = outputName.size();
						fout.write(reinterpret_cast<const char*>(&output_name_size), sizeof(size_t));
						fout.write(outputName.c_str(), output_name_size);
					}
				}

				//リンク情報をコピー
				const size_t num_links = links_.size();
				fout.write(reinterpret_cast<const char*>(&num_links), static_cast<std::streamsize>(sizeof(size_t)));
				fout.write(reinterpret_cast<const char*>(links_.data()), static_cast<std::streamsize>(sizeof(Link) * num_links));

				//current_idをコピー
				fout.write(reinterpret_cast<const char*>(&current_id_), static_cast<std::streamsize>(sizeof(int)));
				fout.close();
			}

			
			//ロードするファイル名
			void load(const std::string& filename) {
				nodes_.clear(); // 既存ノードをクリア
				links_.clear(); // 既存リンクをクリア

				// ファイルパスを作成
				std::string ini_filepath = "Application/Editor/" + filename + ".ini";
				std::string bytes_filepath = "Application/Editor/" + filename + ".bytes";

				//状態をロード
				ImNodes::LoadCurrentEditorStateFromIniFile(ini_filepath.c_str());

				//エディタの状態をロード
				std::fstream fin(bytes_filepath, std::ios_base::in | std::ios_base::binary);
				if (!fin.is_open()) {
					return;
				}

				//ノード情報をコピー
				size_t num_nodes;
				fin.read(reinterpret_cast<char*>(&num_nodes), static_cast<std::streamsize>(sizeof(size_t)));
				nodes_.resize(num_nodes);
				for (size_t i = 0; i < num_nodes; ++i) {
					int id;
					size_t name_size;
					NodeType node_type;
					fin.read(reinterpret_cast<char*>(&id), sizeof(int));
					fin.read(reinterpret_cast<char*>(&node_type), sizeof(NodeType));

					// ノード名のセーブ
					fin.read(reinterpret_cast<char*>(&name_size), sizeof(size_t));
					std::string name(name_size, '\0');
					fin.read(&name[0], name_size);

					// 出力名のロード
					size_t output_count;
					fin.read(reinterpret_cast<char*>(&output_count), sizeof(size_t));
					std::vector<std::string> outputNames(output_count);
					for (size_t j = 0; j < output_count; ++j) {
						size_t output_name_size;
						fin.read(reinterpret_cast<char*>(&output_name_size), sizeof(size_t));
						std::string outputName(output_name_size, '\0');
						fin.read(&outputName[0], output_name_size);
						outputNames[j] = outputName; // 出力名を追加
					}
					nodes_[i] = Node(id, name, node_type, outputNames);
				}

				//リンク情報をコピー
				size_t num_links;
				fin.read(reinterpret_cast<char*>(&num_links), static_cast<std::streamsize>(sizeof(size_t)));
				links_.resize(num_links);
				fin.read(reinterpret_cast<char*>(links_.data()), static_cast<std::streamsize>(sizeof(Link) * num_links));

				//current_idをコピー
				fin.read(reinterpret_cast<char*>(&current_id_), static_cast<std::streamsize>(sizeof(int)));
				fin.close();
			}


			void GetNodeOutPut(){}

		private:

			void DeleatNode(){
		
		    }

		private:
			std::vector<Node> nodes_;
			std::vector<Link> links_;
			int  current_id_;
			char new_node_name[128] = ""; // 新しいノード名の入力用
			NodeType selected_type;
		};

		static SaveLoadEditor editor;
	}
}

