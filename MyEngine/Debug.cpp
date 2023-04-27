#include "stdafx.h"
#include "Debug.h"

void Debug::Log(const std::string& str) {
	OutputDebugStringA(str.c_str());
}

void Debug::Log(const std::wstring& str) {
	OutputDebugStringW(str.c_str());
}
