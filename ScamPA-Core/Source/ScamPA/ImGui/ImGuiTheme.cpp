#include "spapch.h"
#include "ImGuiTheme.h"

namespace SPA::UI {

	void SetDarkTheme() {
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4(&colors)[ImGuiCol_COUNT] = style.Colors;

		// Base dark background
		colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.085f, 0.09f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.075f, 0.98f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.085f, 0.085f, 0.095f, 1.00f);

		// Neutral/dark surface tiers
		const ImVec4 surface_low = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
		const ImVec4 surface_mid = ImVec4(0.16f, 0.16f, 0.19f, 1.00f);
		const ImVec4 surface_high = ImVec4(0.20f, 0.20f, 0.23f, 1.00f);

		// Deep royal purple color family (#4B006E base) for panel/window borders and separators
		const ImVec4 accent = ImVec4(0.294f, 0.000f, 0.431f, 1.00f);		// #4B006E
		const ImVec4 accent_hover = ImVec4(0.365f, 0.000f, 0.537f, 1.00f);	// #5D0089
		const ImVec4 accent_active = ImVec4(0.463f, 0.000f, 0.710f, 1.00f);	// #7600B5

		// Headers
		colors[ImGuiCol_Header] = surface_mid;
		colors[ImGuiCol_HeaderHovered] = surface_high;
		colors[ImGuiCol_HeaderActive] = surface_high;

		// Tabs
		colors[ImGuiCol_Tab] = surface_low;
		colors[ImGuiCol_TabHovered] = surface_high;
		colors[ImGuiCol_TabActive] = surface_mid;
		colors[ImGuiCol_TabUnfocused] = surface_low;
		colors[ImGuiCol_TabUnfocusedActive] = surface_mid;

		// Title bars 
		colors[ImGuiCol_TitleBg] = surface_low;
		colors[ImGuiCol_TitleBgActive] = surface_low;
		colors[ImGuiCol_TitleBgCollapsed] = surface_low;

		// Frames (inputs, sliders)
		colors[ImGuiCol_FrameBg] = surface_mid;
		colors[ImGuiCol_FrameBgHovered] = surface_high;
		colors[ImGuiCol_FrameBgActive] = surface_high;

		// Buttons
		colors[ImGuiCol_Button] = surface_mid;
		colors[ImGuiCol_ButtonHovered] = surface_high;
		colors[ImGuiCol_ButtonActive] = surface_high;

		// Interactive accents
		colors[ImGuiCol_CheckMark] = ImVec4(0.85f, 0.85f, 0.90f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.65f, 0.65f, 0.70f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.75f, 0.75f, 0.80f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.45f, 0.45f, 0.50f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.60f, 0.60f, 0.65f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.70f, 0.70f, 0.75f, 1.00f);

		// Scrollbar
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.06f, 0.065f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = surface_low;
		colors[ImGuiCol_ScrollbarGrabHovered] = surface_mid;
		colors[ImGuiCol_ScrollbarGrabActive] = surface_high;

		// Separators
		colors[ImGuiCol_Separator] = ImVec4(0.24f, 0.24f, 0.27f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = accent_hover; // = ImVec4(0.32f, 0.32f, 0.36f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = accent_active; //= ImVec4(0.36f, 0.36f, 0.40f, 1.00f);

		// Borders
		colors[ImGuiCol_Border] = accent; // Purple
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

		// Text
		colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.95f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.55f, 1.00f);

		// Drag & Drop 
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.80f, 0.80f, 0.90f, 1.00f);

		// Plot
		colors[ImGuiCol_PlotLines] = ImVec4(0.60f, 0.60f, 0.65f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.75f, 0.75f, 0.80f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.60f, 0.60f, 0.65f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.75f, 0.75f, 0.80f, 1.00f);

		// Rounding and spacing
		style.WindowRounding = 6.0f;
		style.FrameRounding = 5.0f;
		style.GrabRounding = 4.0f;
		style.PopupRounding = 6.0f;
		style.TabRounding = 6.0f;

		// Borders on panels/windows 
		style.WindowBorderSize = 1.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupBorderSize = 1.0f;
		style.FrameBorderSize = 0.0f;
		style.TabBorderSize = 0.0f;

		style.ItemSpacing = ImVec2(8.0f, 6.0f);
		style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
		style.WindowPadding = ImVec2(10.0f, 10.0f);
		style.FramePadding = ImVec2(8.0f, 6.0f);
		style.ScrollbarSize = 14.0f;
	}

}