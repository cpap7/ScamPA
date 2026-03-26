#include "ChatbotPanel.h"

#include <imgui.h>

namespace SPA {

	namespace Utilities {
		static std::string UpdateStatusLabel(EChatbotState a_state) {
			std::string status_label = "Unknown";
			switch (a_state) {
				case EChatbotState::Idle:				status_label = "Idle";				break;
				case EChatbotState::Listening:			status_label = "Listening...";		break;
				case EChatbotState::Transcribing:		status_label = "Transcribing...";	break;
				case EChatbotState::Inferring:			status_label = "Inferring...";		break;
				case EChatbotState::Speaking:			status_label = "Speaking";			break;
				case EChatbotState::Error:				status_label = "Error";				break;
				default: break;
			}
			return status_label;
		}

		static ImVec4 UpdateStatusColor(EChatbotState a_state) {
			ImVec4 status_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			switch (a_state) {
				case EChatbotState::Idle:				status_color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); break;
				case EChatbotState::Listening:			status_color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f); break;
				case EChatbotState::Transcribing:		status_color = ImVec4(1.0f, 0.8f, 0.2f, 1.0f); break;
				case EChatbotState::Inferring:			status_color = ImVec4(0.4f, 0.7f, 1.0f, 1.0f); break;
				case EChatbotState::Speaking:			status_color = ImVec4(0.3f, 1.0f, 0.3f, 1.0f); break;
				case EChatbotState::Error:				status_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); break;
				default: break;
			}

			return status_color;
		}
	}


	CChatbotPanel::CChatbotPanel(CChatbotStateMachine& a_state_machine)
		: m_state_machine(a_state_machine) {

	}

	void CChatbotPanel::OnInit() {

	}
	
	void CChatbotPanel::OnShutdown() {

	}
	
	void CChatbotPanel::OnUIRender() {
		ImGui::Begin("Chatbot FSM");
		
		EChatbotState state		= m_state_machine.GetState();

		// Check if models are loaded
		auto& context = m_state_machine.GetContext();
		if (!context.IsSTTInitialized() || !context.IsLLMInitialized() || !context.IsTTSInitialized()) {
			// TODO: This is some ugly yan-dev-like code, so refactor later
			
			// STT
			if (!context.IsSTTInitialized()) {
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Speech-To-Text Engine: Not Loaded");
			}
			else {
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "Speech-To-Text Engine: Loaded");
			}
			
			// LLM
			if (!context.IsLLMInitialized()) {
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Large Language Model Engine: Not Loaded");
			}
			else {
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "Large Language Model Engine: Loaded");
			}

			// TTS
			if (!context.IsTTSInitialized()) {
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Text-To-Speech Engine: Not Loaded");
			}
			else {
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "Text-To-Speech Model Engine: Loaded");
			}

			ImGui::End();
			return;
		}

		// Status
		std::string status_label	= Utilities::UpdateStatusLabel(state);
		ImVec4 status_color			= Utilities::UpdateStatusColor(state);

		ImGui::TextColored(status_color, "Status: %s", status_label.c_str());
		ImGui::Separator();

		// Controls
		if (state == EChatbotState::Idle || state == EChatbotState::Error) {
			if (ImGui::Button("Start")) {
				m_state_machine.OnEvent(EChatbotEvent::Record);
			}
		}
		else if (state == EChatbotState::Listening) {
			if (ImGui::Button("Infer Response")) {
				m_state_machine.OnEvent(EChatbotEvent::Infer);		
			}
		}
		if (state == EChatbotState::Speaking) {
			m_state_machine.OnUpdate();
			UpdateChatLog();
		}

		if (!(state == EChatbotState::Idle)) {
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				m_state_machine.OnEvent(EChatbotEvent::Cancel);
			}
		}
		
		ImGui::Separator();

		// Show in-progress results above history
		ImGui::Text("Current Dialogue");
		if (state == EChatbotState::Inferring || state == EChatbotState::Speaking) {
			if (!m_stt_transcript.empty()) {
				ImGui::TextWrapped("Prompt: %s", m_stt_transcript.c_str());
			}
			if (!m_llm_response.empty()) {
				ImGui::TextWrapped("Agent: %s", m_llm_response.c_str());
			}

			// Poll results & commit to transcript history on cycle completion
			UpdateChatLog();
			CommitChatLog();
		}

		ImGui::Separator();

		// Chat history
		ImGui::BeginChild("ChatbotHistory", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);
		ImGui::Text("Chat History");
		ImGui::Separator();
		for (const auto& [prompt_msg, agent_msg] : m_chat_log) {
			ImGui::TextWrapped("Prompt: %s", prompt_msg.c_str());			
			ImGui::TextWrapped("Agent: %s", agent_msg.c_str());
			ImGui::Separator();
		}
		ImGui::EndChild();

		// Display error in footer
		if (state == EChatbotState::Error && !m_error.empty()) {
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: %s", m_error.c_str());
		}

		ImGui::End();
	}

	void CChatbotPanel::UpdateChatLog() {
		// Refresh cached results during STT transcription & LLM response after inference
		std::string last_transcript = m_state_machine.GetLastSTTTranscript();
		std::string last_response = m_state_machine.GetLastLLMResponse();
		std::string last_error = m_state_machine.GetLastError();

		if (!last_transcript.empty())	{ m_stt_transcript = std::move(last_transcript);	}
		if (!last_response.empty())		{ m_llm_response = std::move(last_response);		}
		if (!last_error.empty())		{ m_error = std::move(last_error);					}
	}

	void CChatbotPanel::CommitChatLog() {
		// Check if the full cycle is completed (Speaking -> Listening)
		EChatbotState state = m_state_machine.GetState();
		if (state == EChatbotState::Listening) {
			if (!m_stt_transcript.empty() && !m_llm_response.empty()) { // Commit to history
				m_chat_log.emplace_back(std::move(m_stt_transcript), std::move(m_llm_response));
			}
			m_stt_transcript.clear();
			m_llm_response.clear();
			m_error.clear();
		}
	}
}