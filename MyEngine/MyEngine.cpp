#include "stdafx.h"
#include "MyEngine.h"
#include "Window.h"
#include "GraphicsEngine.h"

void MyEngine::Run() {
	Window window;
	window.Initalize("Title", 1280, 720);
	
	GraphicsEngine graphicsEngine;
	graphicsEngine.Initalize();
	
	window.Show();

	MSG msg{};
	// ウィンドウの×ボタンがが押されるまでループ
	while (msg.message != WM_QUIT) {
		// Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			
		}
	}

	graphicsEngine.Finalize();
	window.Close();
}
