#include "stdafx.h"
#include "Window.h"
#include "Debug.h"
#include "StringUtils.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	// メッセージに対してゲーム固有の処理を行う
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void Window::Initalize(const std::string& name, uint32_t clientWidth, uint32_t clientHeight) {
	assert(!hwnd_);
	
	name_ = name;
	std::wstring wname = String::Convert(name);

	// ウィンドウクラスを生成
	WNDCLASS wc{};
	// ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	// ウィンドウクラス名
	wc.lpszClassName = wname.c_str();
	// インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスを登録
	RegisterClass(&wc);

	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc{ 0,0,static_cast<LONG>(clientWidth),static_cast<LONG>(clientHeight) };
	// クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	width_ = wrc.right - wrc.left;
	height_ = wrc.bottom - wrc.top;

	// ウィンドウの生成
	hwnd_ = CreateWindow(
		wc.lpszClassName,		// 利用するクラス名
		wname.c_str(),				// タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	// よく見るウィンドウスタイル
		CW_USEDEFAULT,			// 表示X座標（WindowsOSに任せる）
		CW_USEDEFAULT,			// 表示Y座標（WindowsOSに任せる）
		width_,	// ウィンドウ横幅
		height_,	// ウィンドウ縦幅
		nullptr,				// 親ウィンドウハンドル
		nullptr,				// メニューハンドル
		wc.hInstance,			// インスタンスハンドル
		nullptr);				// オプション
}

void Window::Show() {
	assert(hwnd_);
	ShowWindow(hwnd_, SW_SHOW);
	Debug::Log("Show window\n");
}

void Window::Close() {
	assert(hwnd_);
	CloseWindow(hwnd_);
	Debug::Log("Close window\n");
}
