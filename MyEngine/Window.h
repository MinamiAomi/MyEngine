#pragma once

void MsgLoop(std::function<void()> updateFunc);

class Window {
public:
	void Initalize(const std::string& name, uint32_t clientWidth, uint32_t clientHeight);
	void Show();
	void Close();

	inline const HWND& GetHWND() const { return hwnd_; }

private:
	HWND hwnd_ = nullptr;
	WNDCLASS windowClass_{};
	uint32_t width_ = 0;
	uint32_t height_ = 0;
	std::string name_;
};

