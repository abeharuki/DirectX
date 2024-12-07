#pragma once
#include <string>

/**
 * @class Utility
 * @brief 文字列変換およびロギング機能を提供するクラス
 */
class Utility {
public:
	// string -> wstring
	static std::wstring ConvertString(const std::string& str);
	// wstring -> string
	static std::string ConvertString(const std::wstring& str);

	static void Log(const std::string& message);
};
