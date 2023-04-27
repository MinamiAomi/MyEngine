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
	assert(!m_hwnd);
	
	m_name = name;
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

	m_width = wrc.right - wrc.left;
	m_height = wrc.bottom - wrc.top;

	// ウィンドウの生成
	m_hwnd = CreateWindow(
		wc.lpszClassName,		// 利用するクラス名
		wname.c_str(),				// タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	// よく見るウィンドウスタイル
		CW_USEDEFAULT,			// 表示X座標（WindowsOSに任せる）
		CW_USEDEFAULT,			// 表示Y座標（WindowsOSに任せる）
		m_width,	// ウィンドウ横幅
		m_height,	// ウィンドウ縦幅
		nullptr,				// 親ウィンドウハンドル
		nullptr,				// メニューハンドル
		wc.hInstance,			// インスタンスハンドル
		nullptr);				// オプション
}

void Window::Show() {
	assert(m_hwnd);
	ShowWindow(m_hwnd, SW_SHOW);
	Debug::Log("Show window\n");
}

void Window::Close() {
	assert(m_hwnd);
	CloseWindow(m_hwnd);
	Debug::Log("Close window\n");
}
