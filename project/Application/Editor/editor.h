#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <imnodes.h>
#include <imgui.h>
#include "Input.h"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <ios> 
#include <stddef.h>
#include <vector>
#include <string>
#include <iostream>

enum OutputNum
{
	Output1, // 1出力ポート
	Output2, // 2出力ポート
	Output3, // 3出力ポート
	Output4, // 4出力ポート
	Output5, // 5出力ポート
	Output6, // 6出力ポート
};

namespace Editor
{
	enum class NodeType
	{
		BasicNode,
		DoubleOutputNode, // 2出力ポート
		TripleOutputNode, // 3出力ポート
		QuadrupleOutputNode, // 4出力ポート
		QuintupleOutputNode, // 5出力ポート
		SextupleOutputNode,  // 6出力ポート
		NoOutputNode      // 0出力ポート
	};


	struct Node
	{
		int   id;
		std::string name; // ノード名を追加
		NodeType type; // ノードの種類を追加
		std::vector<std::string> outputNames; // 出力ポートの名前を保存するベクター

		bool isActive = false;

		Node() = default;
		Node(int i, const std::string& n, NodeType t, const std::vector<std::string>& outputNames) : id(i), name(n), type(t), outputNames(outputNames) {}
	};

	struct Link
	{
		int id;
		int start_attr, end_attr;
	};

	class NodeEditor
	{
	public:
		NodeEditor() : nodes_(), links_(), current_id_(0), new_node_name("Node"), selected_type(NodeType::BasicNode) {}

		void show(const std::string& filename);


		//セーブするファイル名
		void save(const std::string& filename);


		//ロードするファイル名
		void load(const std::string& filename);

		//引数のNodeNameのnum番目のoutputがつながっているノードを返す
		Node GetLinkNode(const std::string& nodename, int outputnum);

		void SetCurrentStateNode(int NodeId);

	private:
		std::vector<Node> nodes_;
		std::vector<Link> links_;
		int  current_id_;
		char new_node_name[128] = ""; // 新しいノード名の入力用
		NodeType selected_type;

		ImVec2 offset_;
		bool isFocused_;

		int currentStateNodeId_;
	};


}