#pragma once

void MsgLoop(std::function<void()> updateFunc);

class Window {
public:
	void Initalize(const std::string& name, uint32_t clientWidth, uint32_t clientHeight);
	void Show();
	void Close();

	inline const HWND& GetHWND() const { return m_hwnd; }

private:
	HWND m_hwnd = nullptr;
	WNDCLASS m_windowClass{};
	uint32_t m_width = 0;
	uint32_t m_height = 0;
	std::string m_name;
};

