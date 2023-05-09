#include "stdafx.h"
#include "ImGuiManager.h"
#include "Graphics.h"

void ImGuiManager::Initalize(HWND hwnd, ID3D12Device* device, uint32_t backBufferCount) {

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = 1;
	
	auto hr = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&descriptorHeap_));
	assert(SUCCEEDED(hr));

	auto context = ImGui::CreateContext();
	assert(context);

	bool isInit = ImGui_ImplWin32_Init(hwnd);
	assert(isInit);

	isInit = ImGui_ImplDX12_Init(
		device,
		backBufferCount,
		DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
		descriptorHeap_.Get(),
		descriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		descriptorHeap_->GetGPUDescriptorHandleForHeapStart());
	assert(isInit);
}

void ImGuiManager::BeginFrame() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::Render(ID3D12GraphicsCommandList* commandList) {
	ImGui::Render();
	ID3D12DescriptorHeap* ppHeaps[] = { descriptorHeap_.Get()};
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}

void ImGuiManager::Finalize() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
