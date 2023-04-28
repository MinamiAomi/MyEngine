#include "stdafx.h"
#include "Graphics.h"
#include "Debug.h"

void Graphics::Initalize(const HWND& hwnd, uint32_t clientWidth, uint32_t clientHeight) {
	m_hwnd = hwnd;
	m_swapChainResourceWidth = clientWidth;
	m_swapChainResourceHeight = clientHeight;

	CreateDevice();
	CreateCommand();
	CreateFence();
	CreateSwapChain();
	CreateDescriptorHeaps();
	CreateResources();
}

void Graphics::Finalize() {
}

void Graphics::CreateDevice() {

#ifdef _DEBUG	
	// デバッグ時のみ
	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行えるようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
		debugController->Release();
	}
#endif
	
	HRESULT hr = S_FALSE;

	// DXGIファクトリーの生成
	hr = CreateDXGIFactory(IID_PPV_ARGS(&m_dxgiFactory));
	assert(SUCCEEDED(hr));

	// 使用するアダプタ用の変数
	IDXGIAdapter4* useAdapter = nullptr;

	// 良い順にアダプターを頼む
	for (uint32_t i = 0;
		m_dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND;
		++i) {
		// アダプター情報を取得
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));
		// ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// 採用したアダプタ情報を出力
			Debug::Log(std::format(L"Use Adapter:{}\n", adapterDesc.Description));
			break;
		}
		useAdapter = nullptr; // ソフトウェアアダプタは見なかったことにする
	}
	assert(useAdapter != nullptr);


	// 機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
	// 高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		// 採用したアダプターデバイスを生成
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&m_device));
		// 指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr)) {
			// 生成できたのでログ出力を行ってループを抜ける
			Debug::Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	assert(m_device != nullptr);
	useAdapter->Release();
	Debug::Log("Complete create D3D12Device!!!\n"); // 初期化完了のログを出す

#ifdef _DEBUG
	// デバッグ時のみ
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(m_device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// やばいエラーの時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラーの時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
		// 解放
		infoQueue->Release();
	}
#endif
}

void Graphics::CreateCommand() {
	HRESULT hr = S_FALSE;
	// コマンドキューを生成
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = m_device->CreateCommandQueue(
		&commandQueueDesc, 
		IID_PPV_ARGS(&m_commandQueue));
	assert(SUCCEEDED(hr));

	// コマンドアロケータを生成
	for (auto& itr : m_commandAllocators) {
		hr = m_device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT, 
			IID_PPV_ARGS(&itr));
		assert(SUCCEEDED(hr));
	}

	// コマンドリストを生成
	hr = m_device->CreateCommandList(
		0, 
		D3D12_COMMAND_LIST_TYPE_DIRECT, 
		m_commandAllocators[0].Get(), 
		nullptr, 
		IID_PPV_ARGS(&m_commandList));
	assert(SUCCEEDED(hr));
}

void Graphics::CreateFence() {
	// フェンスを生成
	HRESULT hr = m_device->CreateFence(
		m_fenceValue, 
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&m_fence));
	assert(SUCCEEDED(hr));

	m_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(m_fenceEvent != nullptr);
}

void Graphics::CreateSwapChain() {
	HRESULT hr = S_FALSE;
	// スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = m_swapChainResourceWidth;		// 画面幅
	swapChainDesc.Height = m_swapChainResourceHeight;	// 画面高
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 色の形式
	swapChainDesc.SampleDesc.Count = 1;					// マルチサンプル市内
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描画ターゲットとして利用する
	swapChainDesc.BufferCount = kSwapChainBufferCount;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// モニタに移したら、中身を破棄
	
	ComPtr<IDXGISwapChain1> swapChain1;
	// スワップチェーンを生成
	hr = m_dxgiFactory->CreateSwapChainForHwnd(
		m_commandQueue.Get(), 
		m_hwnd, 
		&swapChainDesc, 
		nullptr, 
		nullptr, 
		&swapChain1);
	assert(SUCCEEDED(hr));

	hr = swapChain1.As(&m_swapChain);
	assert(SUCCEEDED(hr));
}

void Graphics::CreateDescriptorHeaps() {
	HRESULT hr = S_FALSE;
	// RTV用ディスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc.NumDescriptors = kSwapChainBufferCount;
	hr = m_device->CreateDescriptorHeap(
		&rtvDescriptorHeapDesc, 
		IID_PPV_ARGS(&m_rtvDescriptorHeap));
	assert(SUCCEEDED(hr));

	D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc{};
	dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescriptorHeapDesc.NumDescriptors = 1;
	hr = m_device->CreateDescriptorHeap(
		&dsvDescriptorHeapDesc,
		IID_PPV_ARGS(&m_dsvDescriptorHeap));
	assert(SUCCEEDED(hr));
}

void Graphics::CreateResources() {
	HRESULT hr = S_FALSE;
	// SwapChainResourceの生成とRTVの生成
	for (uint32_t i = 0; i < kSwapChainBufferCount; ++i) {
		// SwapChainからResourceを引っ張ってくる
		hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainResources[i]));
		assert(SUCCEEDED(hr));
		// RTVの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		// ディスクリプタの先頭を取得
		m_rtvHandles[i] = m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		// ディスクリプタハンドルをずらす
		m_rtvHandles[i].ptr += static_cast<size_t>(i) * m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		// RTVを生成
		m_device->CreateRenderTargetView(m_swapChainResources[i].Get(), &rtvDesc, m_rtvHandles[i]);
	}

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC depthStencilResourceDesc{};
	depthStencilResourceDesc.Width = m_swapChainResourceWidth;
	depthStencilResourceDesc.Height = m_swapChainResourceHeight;
	depthStencilResourceDesc.MipLevels = 1;
	depthStencilResourceDesc.DepthOrArraySize = 1;
	depthStencilResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilResourceDesc.SampleDesc.Count = 1;
	depthStencilResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	hr = m_device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&m_depthStencilResource));
	assert(SUCCEEDED(hr));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	m_device->CreateDepthStencilView(
		m_depthStencilResource.Get(), 
		&dsvDesc, 
		m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}
