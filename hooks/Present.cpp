#ifndef _WIN64
#error Has to be Build in x64
#endif

#include <pch.h>
#include <base.h>

#include "../configs/Configs.h"

HRESULT PRESENT_CALL Base::Hooks::Present(IDXGISwapChain* thisptr, UINT SyncInterval, UINT Flags)
{
	Config cfg;

	if (!Data::InitImGui)
	{
		Data::pSwapChain = thisptr;
		if (SUCCEEDED(Data::pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&Data::pDxDevice11)))
		{
			Data::pDxDevice11->GetImmediateContext(&Data::pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			Data::pSwapChain->GetDesc(&sd);
			Data::hWindow = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			Data::pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			Data::pDxDevice11->CreateRenderTargetView(pBackBuffer, NULL, &Data::pMainRenderTargetView);
			pBackBuffer->Release();
			Data::oWndProc = (WNDPROC)SetWindowLongPtr(Data::hWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
			ImGui_ImplWin32_Init(Data::hWindow);
			ImGui_ImplDX11_Init(Data::pDxDevice11, Data::pContext);
			Data::InitImGui = true;
		}
	}

	if (!Data::InitImGui) return Data::oPresent(thisptr, SyncInterval, Flags);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	auto clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);

	ImGuiStyle& style = ImGui::GetStyle();

	auto colors = style.Colors;
	colors[ImGuiCol_FrameBg] = ImColor(30, 30, 30);
	colors[ImGuiCol_FrameBgHovered] = ImColor(40, 40, 40);
	colors[ImGuiCol_FrameBgActive] = ImColor(85, 129, 241);
	colors[ImGuiCol_Separator] = ImColor(85, 129, 241);
	colors[ImGuiCol_SliderGrab] = ImColor(85, 129, 241);
	colors[ImGuiCol_SliderGrabActive] = ImColor(85, 129, 241);
	colors[ImGuiCol_PopupBg] = ImColor(30, 30, 30);
	colors[ImGuiCol_ScrollbarBg] = ImColor(0, 0, 0, 0);
	colors[ImGuiCol_ScrollbarGrab] = ImColor(85, 129, 241);
	colors[ImGuiCol_ScrollbarGrabActive] = ImColor(85, 129, 241);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(85, 129, 241, 200);
	colors[ImGuiCol_Border] = ImColor(40, 40, 50);
	colors[ImGuiCol_ChildBg] = ImColor(40, 40, 50);
	colors[ImGuiCol_Header] = ImColor(40, 40, 40);
	colors[ImGuiCol_HeaderHovered] = ImColor(85, 129, 241, 200);
	colors[ImGuiCol_HeaderActive] = ImColor(85, 129, 241);
	colors[ImGuiCol_Button] = ImColor(35, 36, 44);
	colors[ImGuiCol_ButtonHovered] = ImColor(85, 129, 241, 200);
	colors[ImGuiCol_ButtonActive] = ImColor(85, 129, 241);
	colors[ImGuiCol_Text] = ImColor(255, 255, 255);

	if (Data::ShowMenu)
	{
        ImGui::SetNextWindowSize(ImVec2(650, 470));
        ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
        {
            static int tabs = 0;
            auto d = ImGui::GetWindowDrawList();
            auto p = ImGui::GetWindowPos();

            ImGui::BeginGroup();
            {
                if (ImGui::Button("Aimbot", ImVec2(120, 35)))
                    tabs = 0;
                ImGui::SameLine();
                if (ImGui::Button("Visuals", ImVec2(120, 35)))
                    tabs = 1;
                ImGui::SameLine();
                if (ImGui::Button("Misc", ImVec2(120, 35)))
                    tabs = 2;
                ImGui::SameLine();
                if (ImGui::Button("Colors", ImVec2(120, 35)))
                    tabs = 3;
                ImGui::SameLine();
                if (ImGui::Button("Configs", ImVec2(120, 35)))
                    tabs = 4;
            }
            ImGui::EndGroup();
            ImGui::BeginGroup();
            {
                static int sliderint = 0;
                static float sliderfloat = 0.f;
                static int combo = 0;
                static bool checkbox = 0;
                const char* combos[] = { "One", "Two" };
                switch (tabs)
                {
                case 0:
                    ImGui::BeginGroup();
					{
						ImGui::Columns(2, nullptr, false);
						ImGui::Checkbox("Checkbox", &checkbox);
						ImGui::SliderInt("Slider Int", &sliderint, 0, 20, "%d");
						ImGui::NextColumn();
						ImGui::SliderFloat("Slider Float", &sliderfloat, 0.f, 20.f, "%.1f");
						ImGui::Combo("Combo", &combo, combos, IM_ARRAYSIZE(combos));
					}
                    ImGui::EndGroup();
                    break;
                case 1:
					ImGui::BeginGroup();
					{
						ImGui::Spacing();
						ImGui::Text("Visuals tab");
					}
					ImGui::EndGroup();
                    break;
                case 2:
					ImGui::BeginGroup();
					{
						ImGui::Spacing();
						ImGui::Text("Misc tab");
					}
					ImGui::EndGroup();
                    break;
                case 3:
					ImGui::BeginGroup();
					{
						ImGui::Spacing();
						ImGui::Text("Colors tab");
					}
					ImGui::EndGroup();
                    break;
                case 4:
					ImGui::BeginGroup();
					{
						ImGui::Spacing();
						ImGui::Text("Configs tab");
					}
					ImGui::EndGroup();
                    break;
                }
            }
            ImGui::EndGroup();
        }
        ImGui::End();
	}

	ImGui::EndFrame();
	ImGui::Render();

	Data::pContext->OMSetRenderTargets(1, &Data::pMainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return Data::oPresent(thisptr, SyncInterval, Flags);
}