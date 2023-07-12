#pragma once
#include <string>


class Utility {
public:
	// string -> wstring
	std::wstring ConvertString(const std::string& str);
	// wstring -> string
	std::string ConvertString(const std::wstring& str);

	void Log(const std::string& message);
};
