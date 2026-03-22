#include "LLMPanel.h"

#include <imgui.h>

namespace SPA {
	CLLMPanel::CLLMPanel(CAIAgentContext& a_context)
		: m_ai_agent_context(a_context) {

	}

	void CLLMPanel::OnInit() {

	}

	void CLLMPanel::OnShutdown() {

	}

	void CLLMPanel::OnUIRender() {
		ImGui::Begin("LLM Chat");
		auto* llm_engine = m_ai_agent_context.GetLLMEngine();
		if (!llm_engine) {
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "LLM Engine Not Loaded");
			if (ImGui::Button("Load LLM Model")) {
				m_ai_agent_context.InitLLM();
			}
			ImGui::End();
			return;
		}

		// Chat history
		ImGui::BeginChild("ChatHistory", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() - 2), true);
		for (const auto& [prompt_msg, agent_msg] : m_chat_history) {
			ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Prompt: %s", prompt_msg.c_str());
			ImGui::TextWrapped("Agent: %s", agent_msg.c_str());
			ImGui::Separator();
		}
		ImGui::EndChild();

		// Input
		bool send_input_text = ImGui::InputText("##prompt", m_input_buffer, sizeof(m_input_buffer), ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::SameLine();
		send_input_text |= ImGui::Button("Send");

		if (send_input_text && m_input_buffer[0] != '\0') {
			std::string prompt(m_input_buffer);
			m_input_buffer[0] = '\0'; // Clear buffer

			// NOTE: Blocks the UI thread
			VoxBox::SInferenceResult result = llm_engine->Query(prompt);
			if (result.Success()) {
				m_chat_history.emplace_back(prompt, result.m_text);
			}
		}
		if (ImGui::Button("Reset Context")) {
			llm_engine->Reset("");
			m_chat_history.clear();
		}

		ImGui::End();
	}
}