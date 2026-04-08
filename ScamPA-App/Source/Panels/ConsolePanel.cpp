#include "ConsolePanel.h"
#include <ScamPA/ImGui/ImGuiLayer.h>
#include <misc/cpp/imgui_stdlib.h>

namespace SPA {
	namespace Utilities {
		uint32_t SpdlogLevelToColor(spdlog::level::level_enum a_level) {
			switch (a_level) {
			case spdlog::level::trace:    return IM_COL32(200, 200, 200, 255); // gray
			case spdlog::level::debug:    return IM_COL32(58, 150, 221, 255); // blue
			case spdlog::level::info:     return IM_COL32(19, 161, 14, 255); // green
			case spdlog::level::warn:     return IM_COL32(249, 241, 165, 255); // yellow
			case spdlog::level::err:      return IM_COL32(231, 72, 86, 255); // red
			case spdlog::level::critical: return IM_COL32(255, 50, 50, 255); // bright red
			default:                      return IM_COL32(255, 255, 255, 255);
			}
		}
	
	}


	CConsolePanel::CConsolePanel(std::string_view a_title)
		: m_title(a_title) {
	}

	void CConsolePanel::ClearLog() {
		m_message_history.clear();
	}

	void CConsolePanel::OnInit() {

	}

	void CConsolePanel::OnShutdown() {

	}

	

	void CConsolePanel::DrainSinkMessages() {
		// Core messages
		auto core_msgs = CLogger::GetCoreConsoleSink()->Drain();
		for (auto& msg : core_msgs) {
			SMessageInfo info;
			info.m_tag = "[CORE]";
			info.m_message = std::move(msg.m_message);
			info.m_color = Utilities::SpdlogLevelToColor(msg.m_level);
			info.m_source = ETab::Core;
			m_message_history.push_back(std::move(info));
		}

		// Client messages
		auto client_msgs = CLogger::GetClientConsoleSink()->Drain();
		for (auto& msg : client_msgs) {
			SMessageInfo info;
			info.m_tag = "[CLIENT]";
			info.m_message = std::move(msg.m_message);
			info.m_color = Utilities::SpdlogLevelToColor(msg.m_level);
			info.m_source = ETab::Client;
			m_message_history.push_back(std::move(info));
		}
	}

	void CConsolePanel::RenderMessageList(ETab a_tab_filter) {
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
		const float text_padding = 8.0f;

		ImGui::SetCursorPosY(text_padding);
		for (size_t i{}; i < m_message_history.size(); i++) {
			auto& entry = m_message_history[i];

			// Skip entries that don't match the active tab
			if (a_tab_filter != ETab::All && entry.m_source != a_tab_filter)
				continue;

			if (!m_filter.PassFilter(entry.m_message.c_str()))
				continue;

			ImGui::SetCursorPosX(text_padding);

			ImGui::PushStyleColor(ImGuiCol_Text, ImColor(entry.m_color).Value);

			if (!entry.m_tag.empty()) {
				ImGui::PushFont(CImGuiLayer::GetFont("Bold"));
				ImGui::TextUnformatted(entry.m_tag.c_str());
				ImGui::PopFont();
				ImGui::SameLine(0.0f, text_padding);
			}

			if (entry.m_is_italic)
				ImGui::PushFont(CImGuiLayer::GetFont("Italic"));

			ImGui::TextUnformatted(entry.m_message.c_str());

			if (entry.m_is_italic)
				ImGui::PopFont();

			ImGui::PopStyleColor();
		}

		if (m_scroll_to_bottom || (m_auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);

		m_scroll_to_bottom = false;

		ImGui::PopStyleVar();
	}

	void CConsolePanel::OnUIRender() {
		// Drain any new messages from spdlog sinks each frame
		DrainSinkMessages();

		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(m_title.c_str())) {
			ImGui::End();
			return;
		}

		

		// Tab bar: All | Core | Client
		if (ImGui::BeginTabBar("LogTabs")) {
			if (ImGui::BeginTabItem("All"))     { m_active_tab = ETab::All;    ImGui::EndTabItem(); }
			if (ImGui::BeginTabItem("Core"))    { m_active_tab = ETab::Core;   ImGui::EndTabItem(); }
			if (ImGui::BeginTabItem("Client"))  { m_active_tab = ETab::Client; ImGui::EndTabItem(); }
			ImGui::EndTabBar();
		}

		// Reserve enough left-over height for 1 separator + 1 input text
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (ImGui::BeginPopupContextWindow()) {
			if (ImGui::Selectable("Clear")) ClearLog();
			ImGui::EndPopup();
		}

		RenderMessageList(m_active_tab);

		ImGui::EndChild();
		ImGui::PopStyleVar();
		ImGui::Separator();

		// Options menu
		if (ImGui::BeginPopup("Options")) {
			ImGui::Checkbox("Auto-scroll", &m_auto_scroll);
			ImGui::EndPopup();
		}

		// Options, Filter
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");

		ImGui::SameLine();
		ImGui::Text("Search");
		ImGui::SameLine();
		m_filter.Draw("##search", 180);
		ImGui::Separator();
		

		ImGui::End();
	}

}