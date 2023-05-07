#pragma once

class Graphics {
public:
	static const uint32_t kSwapChainBufferCount = 2;

	void Initalize(const HWND& hwnd, uint32_t clientWidth, uint32_t clientHeight);
	void Finalize();

private:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	void CreateDevice();
	void CreateCommand();
	void CreateFence();
	void CreateSwapChain();
	void CreateDescriptorHeaps();
	void CreateResources();

	HWND m_hwnd = nullptr;
	ComPtr<IDXGIFactory7> m_dxgiFactory;
	ComPtr<ID3D12Device5> m_device;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12GraphicsCommandList4> m_commandList;
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12Fence> m_fence;
	uint64_t m_fenceValue = 0;
	HANDLE m_fenceEvent = nullptr;
	ComPtr<IDXGISwapChain4> m_swapChain;
	uint32_t m_swapChainResourceWidth = 0;
	uint32_t m_swapChainResourceHeight = 0;
	ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
	std::array<ComPtr<ID3D12Resource>, kSwapChainBufferCount> m_swapChainResources;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kSwapChainBufferCount> m_rtvHandles = {};
	ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap;
	ComPtr<ID3D12Resource> m_depthStencilResource;
};

