#include <imnodes.h>
#include <imgui.h>

#include <algorithm>
#include <cassert>
#include <fstream>
#include <ios> // for std::streamsize
#include <stddef.h>
#include <vector>
#include <string>

namespace NodeEditor
{
	namespace
	{
		struct Node
		{
			int   id;
			float value;
			std::string name; // ノード名を追加

			Node() = default;
			Node(const int i, const float v, const std::string& n) : id(i), value(v), name(n) {}
		};

		struct Link
		{
			int id;
			int start_attr, end_attr;
		};

		class SaveLoadEditor
		{
		public:
			SaveLoadEditor() : nodes_(), links_(), current_id_(0), new_node_name("Node") {}

			void show()
			{

				bool node_deleted = false;

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
					if (ImGui::Button("Create Node"))
					{
						const int node_id = ++current_id_;
						ImNodes::SetNodeScreenSpacePos(node_id, ImGui::GetMousePos());
						nodes_.push_back(Node(node_id, 0.f, std::string(new_node_name))); // ノードを追加
						new_node_name[0] = '\0'; // 名前をリセット
						ImGui::CloseCurrentPopup(); // ポップアップを閉じる
					}
					ImGui::EndPopup();
				}


				for (auto iter = nodes_.begin(); iter != nodes_.end();)
				{
					Node& node = *iter;
					ImNodes::BeginNode(node.id);

					ImNodes::BeginNodeTitleBar();
					ImGui::TextUnformatted(node.name.c_str());
					ImNodes::EndNodeTitleBar();

					ImNodes::BeginInputAttribute(node.id << 8);
					ImGui::TextUnformatted("input");
					ImNodes::EndInputAttribute();

					ImNodes::BeginStaticAttribute(node.id << 16);
					ImGui::PushItemWidth(120.f);
					ImGui::DragFloat("value", &node.value, 0.01f);
					ImGui::PopItemWidth();
					ImNodes::EndStaticAttribute();

					ImNodes::BeginOutputAttribute(node.id << 24);
					const float text_width = ImGui::CalcTextSize("output").x;
					ImGui::Indent(120.f + ImGui::CalcTextSize("value").x - text_width);
					ImGui::TextUnformatted("output");
					ImNodes::EndOutputAttribute();

					ImNodes::EndNode();

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
							// リンクを削除
							links_.erase(std::remove_if(links_.begin(), links_.end(),
								[node](const Link& link) {
									return link.start_attr == node.id || link.end_attr == node.id;
								}), links_.end());

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

				for (const Link& link : links_)
				{
					ImNodes::Link(link.id, link.start_attr, link.end_attr);
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


			//現在のノードの位置や状態を保存
			void save()
			{
				// Save the internal imnodes state
				ImNodes::SaveCurrentEditorStateToIniFile("Application/Editor/save_load.ini");

				// Dump our editor state as bytes into a file

				std::fstream fout(
					"Application/Editor/save_load.bytes", std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

				// copy the node vector to file
				const size_t num_nodes = nodes_.size();
				fout.write(
					reinterpret_cast<const char*>(&num_nodes),
					static_cast<std::streamsize>(sizeof(size_t)));
				fout.write(
					reinterpret_cast<const char*>(nodes_.data()),
					static_cast<std::streamsize>(sizeof(Node) * num_nodes));

				// copy the link vector to file
				const size_t num_links = links_.size();
				fout.write(
					reinterpret_cast<const char*>(&num_links),
					static_cast<std::streamsize>(sizeof(size_t)));
				fout.write(
					reinterpret_cast<const char*>(links_.data()),
					static_cast<std::streamsize>(sizeof(Link) * num_links));

				// copy the current_id to file
				fout.write(
					reinterpret_cast<const char*>(&current_id_), static_cast<std::streamsize>(sizeof(int)));
			}

			//前回の状態で出力
			void load()
			{
				// Load the internal imnodes state
				ImNodes::LoadCurrentEditorStateFromIniFile("Application/Editor/save_load.ini");

				// Load our editor state into memory

				std::fstream fin("Application/Editor/save_load.bytes", std::ios_base::in | std::ios_base::binary);

				if (!fin.is_open())
				{
					return;
				}

				// copy nodes into memory
				size_t num_nodes;
				fin.read(reinterpret_cast<char*>(&num_nodes), static_cast<std::streamsize>(sizeof(size_t)));
				nodes_.resize(num_nodes);
				fin.read(
					reinterpret_cast<char*>(nodes_.data()),
					static_cast<std::streamsize>(sizeof(Node) * num_nodes));

				// copy links into memory
				size_t num_links;
				fin.read(reinterpret_cast<char*>(&num_links), static_cast<std::streamsize>(sizeof(size_t)));
				links_.resize(num_links);
				fin.read(
					reinterpret_cast<char*>(links_.data()),
					static_cast<std::streamsize>(sizeof(Link) * num_links));

				// copy current_id into memory
				fin.read(reinterpret_cast<char*>(&current_id_), static_cast<std::streamsize>(sizeof(int)));
			}



		private:
			std::vector<Node> nodes_;
			std::vector<Link> links_;
			int  current_id_;
			char new_node_name[128]; // 新しいノード名の入力用
		};

		static SaveLoadEditor editor;
	}
}

