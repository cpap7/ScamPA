#pragma once

#include <imgui.h>
#include <glm/glm.hpp>

namespace SPA::UI {

	namespace Colors {
		static inline float Convert_sRGB_FromLinear(float a_linear_value);
		static inline float Convert_sRGB_ToLinear(float a_srgb_value);
		ImVec4 ConvertFromSRGB(ImVec4 a_color);
		ImVec4 ConvertToSRGB(ImVec4 a_color);

		// To experiment with editor theme live you can change these constexpr into static
		// members of a static "Theme" class and add a quick ImGui window to adjust the colour values
		namespace Theme {
			constexpr auto accent = IM_COL32(236, 158, 36, 255);
			constexpr auto highlight = IM_COL32(39, 185, 242, 255);
			constexpr auto nice_blue = IM_COL32(83, 232, 254, 255);
			constexpr auto compliment = IM_COL32(78, 151, 166, 255);
			constexpr auto background = IM_COL32(36, 36, 36, 255);
			constexpr auto background_dark = IM_COL32(26, 26, 26, 255);
			constexpr auto titlebar = IM_COL32(21, 21, 21, 255);
			constexpr auto property_field = IM_COL32(15, 15, 15, 255);
			constexpr auto text = IM_COL32(192, 192, 192, 255);
			constexpr auto text_brighter = IM_COL32(210, 210, 210, 255);
			constexpr auto text_darker = IM_COL32(128, 128, 128, 255);
			constexpr auto text_error = IM_COL32(230, 51, 51, 255);
			constexpr auto muted = IM_COL32(77, 77, 77, 255);
			constexpr auto group_header = IM_COL32(47, 47, 47, 255);
			constexpr auto selection = IM_COL32(237, 192, 119, 255);
			constexpr auto selection_muted = IM_COL32(237, 201, 142, 23);
			constexpr auto background_popup = IM_COL32(50, 50, 50, 255);
			constexpr auto valid_prefab = IM_COL32(82, 179, 222, 255);
			constexpr auto invalid_prefab = IM_COL32(222, 43, 43, 255);
			constexpr auto missing_mesh = IM_COL32(230, 102, 76, 255);
			constexpr auto mesh_not_set = IM_COL32(250, 101, 23, 255);
		}
	}

	namespace Colors {
		inline float Convert_sRGB_FromLinear(float a_linear_value) {
			return a_linear_value <= 0.0031308f
				? a_linear_value * 12.92f
				: glm::pow<float>(a_linear_value, 1.0f / 2.2f) * 1.055f - 0.055f;
		}

		inline float Convert_sRGB_ToLinear(float a_srgb_value) {
			return a_srgb_value <= 0.04045f
				? a_srgb_value / 12.92f
				: glm::pow<float>((a_srgb_value + 0.055f) / 1.055f, 2.2f);
		}

		inline ImVec4 ConvertFromSRGB(ImVec4 a_color) {
			return ImVec4(Convert_sRGB_FromLinear(a_color.x),
				Convert_sRGB_FromLinear(a_color.y),
				Convert_sRGB_FromLinear(a_color.z),
				a_color.w
			);
		}

		inline ImVec4 ConvertToSRGB(ImVec4 a_color) {
			return ImVec4(std::pow(a_color.x, 2.2f),
				glm::pow<float>(a_color.y, 2.2f),
				glm::pow<float>(a_color.z, 2.2f),
				a_color.w
			);
		}

		inline ImU32 ColorWithValue(const ImColor& a_color, float a_value) {
			const ImVec4& col_row = a_color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(col_row.x, col_row.y, col_row.z, hue, sat, val);
			return ImColor::HSV(hue, sat, std::min(a_value, 1.0f));
		}

		inline ImU32 ColorWithSaturation(const ImColor& a_color, float a_saturation) {
			const ImVec4& col_row = a_color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(col_row.x, col_row.y, col_row.z, hue, sat, val);
			return ImColor::HSV(hue, std::min(a_saturation, 1.0f), val);
		}

		inline ImU32 ColorWithHue(const ImColor& a_color, float a_hue) {
			const ImVec4& col_row = a_color.Value;
			float h, s, v;
			ImGui::ColorConvertRGBtoHSV(col_row.x, col_row.y, col_row.z, h, s, v);
			return ImColor::HSV(std::min(a_hue, 1.0f), s, v);
		}

		inline ImU32 ColorWithMultipliedValue(const ImColor& a_color, float a_multiplier) {
			const ImVec4& col_row = a_color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(col_row.x, col_row.y, col_row.z, hue, sat, val);
			return ImColor::HSV(hue, sat, std::min(val * a_multiplier, 1.0f));
		}

		inline ImU32 ColorWithMultipliedSaturation(const ImColor& a_color, float a_multiplier) {
			const ImVec4& col_row = a_color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(col_row.x, col_row.y, col_row.z, hue, sat, val);
			return ImColor::HSV(hue, std::min(sat * a_multiplier, 1.0f), val);
		}

		inline ImU32 ColorWithMultipliedHue(const ImColor& a_color, float a_multiplier) {
			const ImVec4& col_row = a_color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(col_row.x, col_row.y, col_row.z, hue, sat, val);
			return ImColor::HSV(std::min(hue * a_multiplier, 1.0f), sat, val);
		}
	}

	void SetDarkTheme();
}
