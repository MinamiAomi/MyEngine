#pragma once

class ImGuiManager {
public:
	void Initalize(HWND hwnd, ID3D12Device* device, uint32_t backBufferCount);
	void BeginFrame();
	void Render(ID3D12GraphicsCommandList* commandList);
	void Finalize();

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
};

