#pragma once
#include "Window.h"
#include "Graphics.h"

namespace MyEngine {
	void Run();

	const std::string kWindowTitle = "Title";
	const uint32_t kClientWidth = 1280;
	const uint32_t kClientHeight = 720;
	
	static Window window;
	static Graphics graphicsEngine;
};

