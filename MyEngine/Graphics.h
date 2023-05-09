#pragma once

class Graphics {
public:
	static const uint32_t kSwapChainBufferCount = 2;

	void Initalize(const HWND& hwnd, uint32_t clientWidth, uint32_t clientHeight);
	void Finalize();

	void SubmitCommandList();
	void WaitForCommandList();
	void ResetCommandList();

	void SetRenderTarget();


	inline ID3D12Device5* GetDevice() { return device_.Get(); }
	inline ID3D12GraphicsCommandList4* GetCommandList() { return commandList_.Get(); }

private:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	void CreateDevice();
	void CreateCommand();
	void CreateFence();
	void CreateSwapChain();
	void CreateDescriptorHeaps();
	void CreateResources();

	HWND hwnd_ = nullptr;
	ComPtr<IDXGIFactory7> dxgiFactory_;
	ComPtr<ID3D12Device5> device_;
	ComPtr<ID3D12CommandQueue> commandQueue_;
	ComPtr<ID3D12GraphicsCommandList4> commandList_;
	ComPtr<ID3D12CommandAllocator> commandAllocator_;
	ComPtr<ID3D12Fence> fence_;
	uint64_t fenceValue_ = 0;
	HANDLE fenceEvent_ = nullptr;
	ComPtr<IDXGISwapChain4> swapChain_;
	uint32_t swapChainResourceWidth_ = 0;
	uint32_t swapChainResourceHeight_ = 0;
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	std::array<ComPtr<ID3D12Resource>, kSwapChainBufferCount> swapChainResources_;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kSwapChainBufferCount> rtvHandles_ = {};
	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;
	ComPtr<ID3D12Resource> depthStencilResource_;
};

