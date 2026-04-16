#pragma once
#include <ScamPA/Core/GUIInterfaces.h>
#include <ScamPA/Core/Logger.h>

#include <vector>
#include <string>
#include <string_view>
#include <format>
#include <functional>

#include <imgui.h>

namespace SPA {

	class CConsolePanel : IPanel {
	public:
		enum class EActiveTab : uint8_t { 
			All = 0, 
			Core, 
			Client
		};

	private:
		struct SMessageInfo {
			std::string m_tag;
			std::string m_message;
			uint32_t m_color = 0xffffffff;
			bool m_is_italic = false;
			EActiveTab m_source = EActiveTab::All; // which logger produced this

			SMessageInfo() = default;

			SMessageInfo(const std::string& a_message, uint32_t a_color = 0xffffffff)
				: m_message(a_message), m_color(a_color) {
			}

			SMessageInfo(const std::string& a_tag, const std::string& a_message, uint32_t a_color = 0xffffffff)
				: m_tag(a_tag), m_message(a_message), m_color(a_color) {
			}
		};

	private: // Members
		ImGuiTextFilter m_filter;

		std::string m_title;
		std::string m_message_buffer;
		std::vector<SMessageInfo> m_message_history;

		EActiveTab m_active_tab = EActiveTab::All;
		bool m_auto_scroll = true;
		bool m_scroll_to_bottom = false;

	public:
		CConsolePanel(std::string_view a_title = "ScamPA Console");
		~CConsolePanel() = default;

		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUIRender() override;

		void ClearLog();


	private:
		void DrainSinkMessages();
		void RenderMessageList(EActiveTab a_tab_filter);

	public:

		template<typename... Args>
		inline void AddMessage(std::string_view a_fmt, Args&&... a_args) {
			std::string message_string = std::vformat(a_fmt, std::make_format_args(a_args...));
			m_message_history.push_back(SMessageInfo(message_string));
		}

		template<typename... Args>
		inline void AddItalicMessage(std::string_view a_fmt, Args&&... a_args) {
			std::string message_string = std::vformat(a_fmt, std::make_format_args(a_args...));
			SMessageInfo info(message_string);
			info.m_is_italic = true;
			m_message_history.push_back(info);
		}

		template<typename... Args>
		inline void AddTaggedMessage(std::string_view a_tag, std::string_view a_fmt, Args&&... a_args) {
			std::string message_string = std::vformat(a_fmt, std::make_format_args(a_args...));
			m_message_history.push_back(SMessageInfo(std::string(a_tag), message_string));
		}

		template<typename... Args>
		inline void AddMessageWithColor(uint32_t a_color, std::string_view a_fmt, Args&&... a_args) {
			std::string message_string = std::vformat(a_fmt, std::make_format_args(a_args...));
			m_message_history.push_back(SMessageInfo(message_string, a_color));
		}

		template<typename... Args>
		inline void AddItalicMessageWithColor(uint32_t a_color, std::string_view a_fmt, Args&&... a_args) {
			std::string message_string = std::vformat(a_fmt, std::make_format_args(a_args...));
			SMessageInfo info(message_string, a_color);
			info.m_is_italic = true;
			m_message_history.push_back(info);
		}

		template<typename... Args>
		inline void AddTaggedMessageWithColor(uint32_t a_color, std::string_view a_tag, std::string_view a_fmt, Args&&... a_args) {
			std::string message_string = std::vformat(a_fmt, std::make_format_args(a_args...));
			m_message_history.push_back(SMessageInfo(std::string(a_tag), message_string, a_color));
		}

	};
}