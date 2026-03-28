#pragma once

#include "../Renderer/VulkanImage.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <memory>

namespace SPA::UI {

	void ShiftCursorX(float a_distance);
	void ShiftCursorY(float a_distance);
	void ShiftCursor(float a_x, float a_y);

	ImRect GetItemRect();
	ImRect RectExpanded(const ImRect& a_rect, float a_x, float a_y);
	ImRect RectOffset(const ImRect& a_rect, float a_x, float a_y);
	ImRect RectOffset(const ImRect& a_rect, ImVec2 a_xy);

	void DrawButtonImage(const std::shared_ptr<SPA::CVulkanImage>& a_image_normal, 
		const std::shared_ptr<SPA::CVulkanImage>& a_image_hovered, 
		const std::shared_ptr<SPA::CVulkanImage>& a_image_pressed,
		ImU32 a_tint_normal, ImU32 a_tint_hovered, ImU32 a_tint_pressed,
		ImVec2 a_rect_min, ImVec2 a_rect_max
	);

	void DrawButtonImage(const std::shared_ptr<SPA::CVulkanImage>& a_image_normal,
		const std::shared_ptr<SPA::CVulkanImage>& a_image_hovered,
		const std::shared_ptr<SPA::CVulkanImage>& a_image_Pressed,
		ImU32 a_tint_normal, ImU32 a_tint_hovered, ImU32 a_tint_pressed, 
		ImRect a_rect
	);

	void DrawButtonImage(const std::shared_ptr<SPA::CVulkanImage>& a_image,
		ImU32 a_tint_normal, ImU32 a_tint_hovered, ImU32 a_tint_pressed,
		ImVec2 a_rect_min, ImVec2 a_rect_max
	);

	void DrawButtonImage(const std::shared_ptr<SPA::CVulkanImage>& a_image,
		ImU32 a_tint_normal, ImU32 a_tint_hovered, ImU32 a_tint_pressed,
		ImRect a_rect
	);

	void DrawButtonImage(const std::shared_ptr<SPA::CVulkanImage>& a_image_normal,
		const std::shared_ptr<SPA::CVulkanImage>& a_image_hovered,
		const std::shared_ptr<SPA::CVulkanImage>& a_image_pressed, 
		ImU32 a_tint_normal, ImU32 a_tint_hovered, ImU32 a_tint_pressed
	);

	void DrawButtonImage(const std::shared_ptr<SPA::CVulkanImage>& a_image,
		ImU32 a_tint_normal, ImU32 a_tint_hovered, ImU32 a_tint_pressed
	);

	void RenderWindowOuterBorders(ImGuiWindow* a_window);

	bool UpdateWindowManualResize(ImGuiWindow* a_window, ImVec2& a_new_size, ImVec2& a_new_position);

	// Menubar with custom rectangle
	bool BeginMenubar(const ImRect& a_bar_rectangle);
	void EndMenubar();

	bool ButtonCentered(const char* a_label, const ImVec2& a_size = ImVec2(0, 0));

	void DrawBorder(ImRect a_rect, float a_thickness = 1.0f, float a_rounding = 0.0f, float a_offset_x = 0.0f, float a_offset_y = 0.0f);

	// Utilities
	class CScopedStyle {
	public:
		CScopedStyle(const CScopedStyle&) = delete;
		CScopedStyle operator=(const CScopedStyle&) = delete;
		
		template<typename T>
		CScopedStyle(ImGuiStyleVar a_style_var, T a_value) { ImGui::PushStyleVar(a_style_var, a_value); }
		~CScopedStyle() { ImGui::PopStyleVar(); }
	};

	class CScopedColor {
	public:
		CScopedColor(const CScopedColor&) = delete;
		CScopedColor operator=(const CScopedColor&) = delete;
		
		template<typename T>
		CScopedColor(ImGuiCol a_color_id, T a_color) { ImGui::PushStyleColor(a_color_id, a_color); }
		~CScopedColor() { ImGui::PopStyleColor(); }
	};

	class CScopedFont {
	public:
		CScopedFont(const CScopedFont&) = delete;
		CScopedFont operator=(const CScopedFont&) = delete;
		
		CScopedFont(ImFont* a_font) { ImGui::PushFont(a_font); }
		~CScopedFont() { ImGui::PopFont(); }
	};

	class CScopedID {
	public:
		CScopedID(const CScopedID&) = delete;
		CScopedID operator=(const CScopedID&) = delete;
		
		template<typename T>
		CScopedID(T a_id) { ImGui::PushID(a_id); }
		~CScopedID() { ImGui::PopID(); }
	};

	class CScopedColorStack {
	private:
		int m_count;

	public:
		CScopedColorStack(const CScopedColorStack&) = delete;
		CScopedColorStack operator=(const CScopedColorStack&) = delete;

		template <typename ColorType, typename... OtherColors>
		CScopedColorStack(ImGuiCol a_first_color_id, ColorType a_first_color, OtherColors&& ... a_other_color_pairs)
			: m_count((sizeof... (a_other_color_pairs) / 2) + 1) {
			static_assert ((sizeof... (a_other_color_pairs) & 1u) == 0,
				"CScopedColorStack constructor expects a list of pairs of Color IDs and Colors as its arguments");

			PushColor(a_first_color_id, a_first_color, std::forward<OtherColors>(a_other_color_pairs)...);
		}

		~CScopedColorStack() { ImGui::PopStyleColor(m_count); }

	private:
		template <typename ColorType, typename... OtherColors>
		void PushColor(ImGuiCol a_color_id, ColorType a_color, OtherColors&& ... a_other_color_pairs) {
			if constexpr (sizeof... (a_other_color_pairs) == 0) {
				ImGui::PushStyleColor(a_color_id, a_color);
			}
			else {
				ImGui::PushStyleColor(a_color_id, a_color);
				PushColor(std::forward<OtherColors>(a_other_color_pairs)...);
			}
		}
	};

	class CScopedStyleStack {
	private:
		int m_count;

	public:
		CScopedStyleStack(const CScopedStyleStack&) = delete;
		CScopedStyleStack operator=(const CScopedStyleStack&) = delete;

		template <typename ValueType, typename... OtherStylePairs>
		CScopedStyleStack(ImGuiStyleVar a_first_style_var, ValueType a_first_value, OtherStylePairs&& ... a_other_style_pairs)
			: m_count((sizeof... (a_other_style_pairs) / 2) + 1) {
			static_assert ((sizeof... (a_other_style_pairs) & 1u) == 0,
				"CScopedStyleStack constructor expects a list of pairs of Color IDs and Colors as its arguments");

			PushStyle(a_first_style_var, a_first_value, std::forward<OtherStylePairs>(a_other_style_pairs)...);
		}

		~CScopedStyleStack() { ImGui::PopStyleVar(m_count); }

	private:
		template <typename ValueType, typename... OtherStylePairs>
		void PushStyle(ImGuiStyleVar a_style_var, ValueType a_value, OtherStylePairs&& ... a_other_style_pairs)	{
			if constexpr (sizeof... (a_other_style_pairs) == 0) {
				ImGui::PushStyleVar(a_style_var, a_value);
			}
			else {
				ImGui::PushStyleVar(a_style_var, a_value);
				PushStyle(std::forward<OtherStylePairs>(a_other_style_pairs)...);
			}
		}
	};

	class CScopedItemFlags {
	public:
		CScopedItemFlags(const CScopedItemFlags&) = delete;
		CScopedItemFlags operator=(const CScopedItemFlags&) = delete;

		CScopedItemFlags(const ImGuiItemFlags a_flags, const bool a_enable = true) { ImGui::PushItemFlag(a_flags, a_enable); }
		~CScopedItemFlags() { ImGui::PopItemFlag(); }
	};


}