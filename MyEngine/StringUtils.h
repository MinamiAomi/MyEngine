#pragma once
#include <format>

namespace String {

	// wstringに変換
	std::wstring Convert(const std::string& str);
	// wstringから変換
	std::string Convert(const std::wstring& str);

};
