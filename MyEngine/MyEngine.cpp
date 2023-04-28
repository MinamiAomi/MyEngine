#include "stdafx.h"
#include "MyEngine.h"

void MyEngine::Run() {
	
	window.Initalize(kWindowTitle, kClientWidth, kClientHeight);
	graphicsEngine.Initalize(window.GetHWND(), kClientWidth, kClientHeight);
	
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
