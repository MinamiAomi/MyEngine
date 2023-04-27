#pragma once

class GraphicsEngine {
public:
	static const uint32_t kSwapChainBufferCount = 2;

	void Initalize();
	void Finalize();

private:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<IDXGIFactory7> m_dxgiFactory;
	ComPtr<ID3D12Device5> m_device;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12GraphicsCommandList4> m_commandList;
	std::array<ComPtr<ID3D12CommandAllocator>, kSwapChainBufferCount> m_commandAllocators;
	ComPtr<ID3D12Fence> m_fence;
	uint64_t m_fenceValue = 0;
	HANDLE m_fenceEvent = nullptr;
	ComPtr<IDXGISwapChain4> m_swapChain;
	ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
	std::array<ComPtr<ID3D12Resource>, kSwapChainBufferCount> m_swapChainResources;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kSwapChainBufferCount> m_rtvHandles;
	ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap;
	ComPtr<ID3D12Resource> m_depthStencilResource;
	

};

