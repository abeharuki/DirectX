#include <imnodes.h>
#include <imgui.h>

#include <algorithm>
#include <cassert>
#include <fstream>
#include <ios> // for std::streamsize
#include <stddef.h>
#include <vector>

namespace example
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
                ImGui::Begin("Save & load");
                ImGui::TextUnformatted("Press A to add a node.");

                ImNodes::BeginNodeEditor();

                // ノード追加用のポップアップを表示
                if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                    ImNodes::IsEditorHovered() && ImGui::IsKeyReleased(ImGuiKey_A))
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
                        nodes_.push_back(Node(node_id, 0.f, std::string(new_node_name)));
                        ImGui::CloseCurrentPopup(); // ポップアップを閉じる
                    }
                    ImGui::EndPopup();
                }

               
                for (auto iter = nodes_.begin(); iter != nodes_.end();)
                {
                    Node& node = *iter;
                    ImNodes::BeginNode(node.id);

                    ImNodes::BeginNodeTitleBar();
                    ImGui::TextUnformatted(node.name.c_str()); // ノード名を表示
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

                    // ノードを右クリックして削除
                    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(1)) // 右クリック
                    {
                        ImGui::OpenPopup("Node Context Menu");
                    }

                    if (ImGui::BeginPopup("Node Context Menu"))
                    {
                        if (ImGui::MenuItem("Delete Node"))
                        {
                            iter = nodes_.erase(iter); // ノードを削除
                            ImNodes::ClearNodeSelection(node.id); // ノードをグラフから削除
                            continue; // ループをスキップ
                        }
                        ImGui::EndPopup();
                    }

                    ++iter;// 次のノードに進む
                }

                for (const Link& link : links_)
                {
                    ImNodes::Link(link.id, link.start_attr, link.end_attr);
                }

                ImNodes::EndNodeEditor();

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

            void save()
            {
                // Save the internal imnodes state
                ImNodes::SaveCurrentEditorStateToIniFile("save_load.ini");

                // Dump our editor state as bytes into a file

                std::fstream fout(
                    "save_load.bytes", std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

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

            void load()
            {
                // Load the internal imnodes state
                ImNodes::LoadCurrentEditorStateFromIniFile("save_load.ini");

                // Load our editor state into memory

                std::fstream fin("save_load.bytes", std::ios_base::in | std::ios_base::binary);

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

