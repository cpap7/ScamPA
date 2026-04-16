#include "ChatbotPanel.h"
#include <ScamPA/Chatbot/ChatbotSerializer.h>
#include <ScamPA/Core/Application.h>

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


	CChatbotPanel::CChatbotPanel(CAIEngineManager& a_manager)
		: m_manager(a_manager) {
	}

	void CChatbotPanel::OnInit() {

	}
	
	void CChatbotPanel::OnShutdown() {

	}
	
	void CChatbotPanel::OnUIRender() { // TODO: Cleanup
		//SPA_PROFILE_FUNCTION();

		ImGui::Begin("Chatbot Agents");
		

		// Check if models are loaded
		if (!m_manager.IsSTTInitialized() || !m_manager.IsLLMInitialized() || !m_manager.IsTTSInitialized()) {
			// TODO: This is some ugly yandev-like code, so refactor later
			
			// STT
			if (!m_manager.IsSTTInitialized()) {
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Speech-To-Text Engine: Not Loaded");
			}
			else {
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "Speech-To-Text Engine: Loaded");
			}
			
			// LLM
			if (!m_manager.IsLLMInitialized()) {
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Large Language Model Engine: Not Loaded");
			}
			else {
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "Large Language Model Engine: Loaded");
			}

			// TTS
			if (!m_manager.IsTTSInitialized()) {
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Text-To-Speech Engine: Not Loaded");
			}
			else {
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "Text-To-Speech Engine: Loaded");
			}

			ImGui::End();
			return;
		}
		
		if (ImGui::BeginTabBar("##agent_tabs", ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_TabListPopupButton)) {
			if (ImGui::TabItemButton("+")) {
				CreateAgent();
			}

			for (size_t i{}; i < m_agents.size(); ++i) {
				bool open = true;
				
				ImGui::PushID(static_cast<int>(i));
				
				if (ImGui::BeginTabItem(m_agents[i]->m_name.c_str(), &open)) {
					RenderAgentTab(*m_agents[i]);
					ImGui::EndTabItem();
				}

				ImGui::PopID();
				
				if (!open) {
					m_pending_remove_index = static_cast<int>(i);
				}
			}

			ImGui::EndTabBar();
		}
		
		// Deferred removal
		if (m_pending_remove_index >= 0) {
			RemoveAgent(static_cast<size_t>(m_pending_remove_index));
			m_pending_remove_index = -1;
		}

		ImGui::End();
	}
	
	SChatbotAgent* CChatbotPanel::CreateAgent(const std::string& a_agent_name) {
		auto agent = std::make_unique<SChatbotAgent>();
		agent->m_name = a_agent_name.empty() ? "Agent " + std::to_string(m_agent_counter++) : a_agent_name;
		agent->m_state_machine = std::make_unique<CChatbotStateMachine>(m_manager);

		return m_agents.emplace_back(std::move(agent)).get();
	}
	
	void CChatbotPanel::RemoveAgent(size_t a_index) {
		if (a_index < m_agents.size()) {
			m_agents[a_index]->m_state_machine->Shutdown();
			m_agents.erase(m_agents.begin() + a_index);
			m_agent_counter--;
		}
	}

	void CChatbotPanel::RenderAgentTab(SChatbotAgent& a_agent) {
		auto& fsm = *a_agent.m_state_machine;
		fsm.OnUpdate(); // Poll for automatic transitions
		EChatbotState state = fsm.GetState(); // FSM state

		// Status
		std::string status_label = Utilities::UpdateStatusLabel(state);
		ImVec4 status_color = Utilities::UpdateStatusColor(state);

		ImGui::TextColored(status_color, "Status: %s", status_label.c_str());
		ImGui::Separator();
		ImGui::TextDisabled("Agent UUID: %llx", a_agent.m_active_session.m_agent_uuid);
		ImGui::Separator();

		// Manual Controls
		if (state == EChatbotState::Idle || state == EChatbotState::Error) {
			if (ImGui::Button("Start")) {
				a_agent.m_state_machine->OnEvent(EChatbotEvent::Record);
			}
		}
		else if (state == EChatbotState::Listening) {
			if (ImGui::Button("Infer Response")) {
				a_agent.m_state_machine->OnEvent(EChatbotEvent::Infer);
			}
		}

		if (!(state == EChatbotState::Idle)) {
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				a_agent.m_state_machine->OnEvent(EChatbotEvent::Cancel);
			}
		}

		ImGui::Separator();

		// Show in-progress results above history
		ImGui::Text("Current Dialogue");
		if (state == EChatbotState::Inferring || state == EChatbotState::Speaking) {
			if (!a_agent.m_pollable_results.m_stt_transcript.empty()) {
				ImGui::TextWrapped("Prompt: %s (confidence = %.2f)", a_agent.m_pollable_results.m_stt_transcript.c_str(), a_agent.m_pollable_results.m_stt_confidence);
			}
			if (!a_agent.m_pollable_results.m_llm_response.empty()) {
				ImGui::TextWrapped("Agent: %s", a_agent.m_pollable_results.m_llm_response.c_str());
			}
		}

		// Poll results & commit to transcript history on cycle completion
		// Done every frame to ensure commits/updates
		UpdateChatLog(a_agent);
		CommitChatLog(a_agent);

		ImGui::Separator();

		// Chat history
		ImGui::BeginChild("ChatbotHistory", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);
		ImGui::Text("Chat History");
		ImGui::Separator();
		for (const auto& exchange : a_agent.m_active_session.m_exchanges) {
			ImGui::TextWrapped("Prompt: %s (confidence = %.2f)", exchange.m_prompt.c_str(), exchange.m_stt_confidence);
			ImGui::TextWrapped("Agent: %s", exchange.m_response.c_str());
			ImGui::Separator();
		}
		ImGui::EndChild();

		if (ImGui::Button("Clear Chat History")) {
			a_agent.m_active_session.m_exchanges.clear();
		}

		if (ImGui::Button("Save (YAML)")) {
			SaveToYAML(a_agent);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load (YAML)")) {
			LoadFromYAML(a_agent);
		}

		if (ImGui::Button("Save (JSON)")) {
			SaveToJSON(a_agent);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load (JSON)")) {
			LoadFromJSON(a_agent);
		}

		// Display error in footer
		if (state == EChatbotState::Error && !a_agent.m_pollable_results.m_error.empty()) {
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: %s", a_agent.m_pollable_results.m_error.c_str());
		}
	}

	void CChatbotPanel::UpdateChatLog(SChatbotAgent& a_agent) {
		//SPA_PROFILE_FUNCTION();

		// Refresh cached results during STT transcription & LLM response after inference
		std::string last_transcript = a_agent.m_state_machine->GetLastSTTTranscript();
		std::string last_response	= a_agent.m_state_machine->GetLastLLMResponse();
		std::string last_error		= a_agent.m_state_machine->GetLastError();
		float last_confidence		= a_agent.m_state_machine->GetLastSTTConfidence();

		if (!last_transcript.empty())	{ a_agent.m_pollable_results.m_stt_transcript	= std::move(last_transcript);		}
		if (!last_response.empty())		{ a_agent.m_pollable_results.m_llm_response		= std::move(last_response);			}
		if (!last_error.empty())		{ a_agent.m_pollable_results.m_error			= std::move(last_error);			}
		if (last_confidence > 0.0f)		{ a_agent.m_pollable_results.m_stt_confidence	= last_confidence;					}
	}

	void CChatbotPanel::CommitChatLog(SChatbotAgent& a_agent) {
		//SPA_PROFILE_FUNCTION();

		// Check if the full cycle is completed (Speaking -> Listening)
		EChatbotState state = a_agent.m_state_machine->GetState();
		if (state == EChatbotState::Listening) {
			if (!a_agent.m_pollable_results.m_stt_transcript.empty() && !a_agent.m_pollable_results.m_llm_response.empty()) { // Commit to history
				a_agent.m_active_session.AddExchange(
					a_agent.m_pollable_results.m_stt_transcript, 
					a_agent.m_pollable_results.m_llm_response, 
					a_agent.m_pollable_results.m_stt_confidence
				);
			}
			a_agent.m_pollable_results.m_stt_transcript.clear();
			a_agent.m_pollable_results.m_llm_response.clear();
			a_agent.m_pollable_results.m_error.clear();
			a_agent.m_pollable_results.m_stt_confidence = 0.0f;
		}
	}

	void CChatbotPanel::SaveToYAML(SChatbotAgent& a_agent) {
		//SPA_PROFILE_FUNCTION();

		std::string file_path = CApplication::GetApplicationInstance().SaveFile("YAML File (*.yaml)\0*.yaml\0", "yaml");
		
		if (!file_path.empty()) {
			CChatbotSerializer serializer(a_agent.m_state_machine->GetEngineManager());
			a_agent.m_active_session.m_agent_name = a_agent.m_name; // Update name
			serializer.SerializeSession(a_agent.m_active_session, file_path, ESerializationFormat::YAML);
		}
	}
	
	void CChatbotPanel::SaveToJSON(SChatbotAgent& a_agent) {
		//SPA_PROFILE_FUNCTION();

		std::string file_path = CApplication::GetApplicationInstance().SaveFile("JSON File (*.json)\0*.json\0", "json");
		
		if (!file_path.empty()) {
			CChatbotSerializer serializer(a_agent.m_state_machine->GetEngineManager());
			a_agent.m_active_session.m_agent_name = a_agent.m_name; // Update name
			serializer.SerializeSession(a_agent.m_active_session, file_path, ESerializationFormat::JSON);
		}
	}

	void CChatbotPanel::LoadFromYAML(SChatbotAgent& a_agent) {
		//SPA_PROFILE_FUNCTION();

		std::string file_path = CApplication::GetApplicationInstance().OpenFile("YAML File (*.yaml)\0*.yaml\0");
		
		if (!file_path.empty()) {
			CChatbotSerializer serializer(a_agent.m_state_machine->GetEngineManager());
			a_agent.SetActiveSession(serializer.DeserializeSession(file_path, ESerializationFormat::YAML));
		}
	}

	void CChatbotPanel::LoadFromJSON(SChatbotAgent& a_agent) {
		//SPA_PROFILE_FUNCTION();

		std::string file_path = CApplication::GetApplicationInstance().OpenFile("JSON File (*.json)\0*.json\0");
		
		if (!file_path.empty()) {
			CChatbotSerializer serializer(a_agent.m_state_machine->GetEngineManager());
			a_agent.SetActiveSession(serializer.DeserializeSession(file_path, ESerializationFormat::JSON));
		}
	}
}