#include "editor.h"


namespace Editor
{
	
	void NodeEditor::show(const std::string& filename)
	{

		bool node_deleted = false;
		static std::vector<std::string> outputNames;
		if (ImGui::Begin(filename.c_str())) {

			ImGui::TextUnformatted("Press N to add a node.");

			if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
				if (!isFocused_) {
					// フォーカスを失う前の状態を保存
					offset_ = ImNodes::EditorContextGetPanning();
				}
				isFocused_ = true;
			}
			else {
				if (isFocused_) {
					// フォーカスが失われたときにのみパニングをリセット
					ImNodes::EditorContextResetPanning(offset_);
					isFocused_ = false;
				}
			}

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
				if (ImGui::RadioButton("QuadrupleOutput Node", selected_type == NodeType::QuadrupleOutputNode))
					selected_type = NodeType::QuadrupleOutputNode;
				ImGui::SameLine();
				if (ImGui::RadioButton("NoOutput Node", selected_type == NodeType::NoOutputNode))
					selected_type = NodeType::NoOutputNode;


				if (selected_type != NodeType::NoOutputNode) {
					int outputCount = (selected_type == NodeType::BasicNode) ? 1 :
						(selected_type == NodeType::DoubleOutputNode ? 2 :
							(selected_type == NodeType::TripleOutputNode ? 3 :
								(selected_type == NodeType::QuadrupleOutputNode ? 4 : 0)));

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

				float maxOutputWidth = 0.0f;

				// outputNamesの最大幅を計算
				for (const auto& outputName : node.outputNames) {
					float width = ImGui::CalcTextSize(outputName.c_str()).x;
					if (width > maxOutputWidth) {
						maxOutputWidth = width;
					}
				}

				ImNodes::BeginNode(node.id);
				

				// ノードタイトルバーに名前とタイプを表示
				ImNodes::BeginNodeTitleBar();
				ImGui::Text("%s (%s)", node.name.c_str(),
					node.type == NodeType::BasicNode ? "Basic" :
					node.type == NodeType::DoubleOutputNode ? "Double Output" :
					node.type == NodeType::TripleOutputNode ? "Triple Output" :
					node.type == NodeType::QuadrupleOutputNode ? "Quadruple Output" :
					"No Output");
				ImNodes::EndNodeTitleBar();

				// 共通の入力属性
				ImNodes::BeginInputAttribute(node.id << 8);
				ImGui::TextUnformatted("input");
				ImNodes::EndInputAttribute();

				// NodeTypeによる出力属性の条件分岐
				if (node.type != NodeType::NoOutputNode) {
					for (int i = 0; i < (node.type == NodeType::QuadrupleOutputNode ? 4 : (node.type == NodeType::TripleOutputNode ? 3 : (node.type == NodeType::DoubleOutputNode ? 2 : 1))); ++i) {
						ImNodes::BeginOutputAttribute((node.id << 24) + i);
						float textWidth = ImGui::CalcTextSize(node.outputNames.size() > i ? node.outputNames[i].c_str() : ("output " + std::to_string(i + 1)).c_str()).x;
						ImGui::Indent(maxOutputWidth - textWidth + 20.0f); // +20.0fは追加の余白

						ImGui::TextUnformatted(node.outputNames.size() > i ? node.outputNames[i].c_str() : ("output " + std::to_string(i + 1)).c_str());
						ImNodes::EndOutputAttribute();
					}
				}

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

					ImGui::Text("Links:");
					bool hasLinks = false; // リンクがあるかどうかのフラグ
					for (const auto& link : links_) {
						// リンクの始点と終点のノードIDを取得
						int startNodeId = link.start_attr >> 24; // 出力属性用
						int endNodeId = link.end_attr >> 8; // 入力属性用

						// 現在のノードID
						int currentNodeId = node.id;

						// ノードの名前を取得するための変数
						std::string connectedNodeName;
						std::string linkType;

						// リンクの始点ノードの名前を取得
						if (startNodeId != currentNodeId) {
							auto startNode = std::find_if(nodes_.begin(), nodes_.end(), [startNodeId](const Node& node) {
								return node.id == startNodeId;
								});

							if (startNode != nodes_.end()) {
								connectedNodeName = startNode->name; // 出力ノードの名前を取得
								linkType = "Input"; // リンクの種類を設定
							}
						}

						// リンクの終点ノードの名前を取得
						if (endNodeId != currentNodeId) {
							auto endNode = std::find_if(nodes_.begin(), nodes_.end(), [endNodeId](const Node& node) {
								return node.id == endNodeId;
								});

							if (endNode != nodes_.end()) {
								connectedNodeName = endNode->name; // 入力ノードの名前を取得
								linkType = "Output"; // リンクの種類を設定
							}
						}

						// 現在のノードと接続されている場合
						if (startNodeId == currentNodeId || endNodeId == currentNodeId) {
							hasLinks = true; // リンクがあることを示すフラグを立てる

							// メニューアイテムの表示
							if (!connectedNodeName.empty()) {
								if (ImGui::MenuItem(("Delete Link to " + linkType + " " + connectedNodeName).c_str())) {
									// 特定のリンクを削除
									links_.erase(std::remove_if(links_.begin(), links_.end(), [&](const Link& l) {
										return l.id == link.id; // 選択されたリンクのIDが一致する場合
										}), links_.end());
								}
							}
						}
					}



					if (!hasLinks) {
						ImGui::Text("No links available."); // リンクがなかった場合のメッセージ
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
		};
		

		ImGui::End();
	}


	//セーブするファイル名
	void NodeEditor::save(const std::string& filename) {

		if (isFocused_) {

			// ファイルパスを作成
			std::string ini_filepath = "resources/NodeEditor/" + filename + ".ini";
			std::string bytes_filepath = "resources/NodeEditor/" + filename + ".bytes";

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
	}

	//ロードするファイル名
	void NodeEditor::load(const std::string& filename) {
		nodes_.clear(); // 既存ノードをクリア
		links_.clear(); // 既存リンクをクリア

		// ファイルパスを作成
		std::string ini_filepath = "resources/NodeEditor/" + filename + ".ini";
		std::string bytes_filepath = "resources/NodeEditor/" + filename + ".bytes";

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

	//引数のNodeNameのnum番目のoutputがつながっているノードを返す
	Node NodeEditor::GetLinkNode(const std::string& nodename, int outputnum) {
		// ノードのIDを取得
		auto startNodeIt = std::find_if(nodes_.begin(), nodes_.end(), [nodename](const Node& node) {
			return node.name == nodename;
			});

		if (startNodeIt == nodes_.end()) {
			throw std::runtime_error("指定されたノードが見つかりません");
		}

		int startNodeId = startNodeIt->id;

		// outputnum 番目の出力に対応するリンクを探す
		auto linkIt = std::find_if(links_.begin(), links_.end(), [&](const Link& link) {
			return (link.start_attr >> 24) == startNodeId && (link.start_attr & 0xFF) == outputnum;
			});

		// リンクが見つからなかった場合、自分のノードを返す
		if (linkIt == links_.end()) {
			return *startNodeIt; // 自分のノードを返す(指定された出力リンクがない)
		}

		// 終点ノードを検索して取得
		int endNodeId = (linkIt->end_attr >> 8);
		auto endNodeIt = std::find_if(nodes_.begin(), nodes_.end(), [endNodeId](const Node& node) {
			return node.id == endNodeId;
			});

		// 終点ノードが見つからなかった場合、自分のノードを返す
		if (endNodeIt == nodes_.end()) {
			return *startNodeIt; // 自分のノードを返す(リンク先のノードがない)
		}

		return *endNodeIt; // リンク先のノードを返す
	}


	
}

