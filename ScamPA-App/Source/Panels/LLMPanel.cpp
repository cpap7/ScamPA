#include "LLMPanel.h"
#include <ScamPA/Core/Application.h>
#include <ScamPA/Chatbot/ChatbotSerializer.h>

#include <imgui.h>

namespace SPA {
	CLLMPanel::CLLMPanel(CAIEngineManager& a_manager)
		: m_manager(a_manager) {

	}

	void CLLMPanel::OnInit() {

	}

	void CLLMPanel::OnShutdown() {

	}

	void CLLMPanel::OnUIRender() {
		ImGui::Begin("LLM Settings");
		auto* llm_engine = m_manager.GetLLMEngine();
		if (!llm_engine) {
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "LLM Engine Not Loaded");
			if (ImGui::Button("Load LLM Model")) {
				std::string llm_model_path = CApplication::GetApplicationInstance().OpenFile("Llama Model (*.gguf)\0*.gguf\0\0");
				if (!llm_model_path.empty()) {
					m_manager.LoadLLM(llm_model_path);
				}
			}
			ImGui::End();
			return;
		}

		ImGui::TextColored(ImVec4(0, 1, 0, 1), "LLM Engine Loaded");
		ImGui::TextDisabled("Model Path");
		ImGui::SameLine();
		ImGui::InputText("##llmmodelpath", (char*)m_manager.GetLLMModelPath().c_str(), ImGuiInputTextFlags_ReadOnly);
		if (ImGui::Button("Load LLM Model")) {
			std::string llm_model_path = CApplication::GetApplicationInstance().OpenFile("Llama Model (*.gguf)\0*.gguf\0\0");
			if (!llm_model_path.empty()) {
				m_manager.LoadLLM(llm_model_path);
			}
		}

		ImGui::Separator();

		if (ImGui::Button("Reset Context")) {
			llm_engine->Reset("");
			m_chat_history.clear();
		}

		// Chat history
		ImGui::BeginChild("ChatHistory", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);
		for (const auto& [prompt_msg, agent_msg] : m_chat_history) {
			ImGui::PushStyleColor(1, ImVec4(0.4f, 0.7f, 1.0f, 1.0f));
			ImGui::TextWrapped("Prompt: %s", prompt_msg.c_str());
			ImGui::PopStyleColor(1);
			ImGui::TextWrapped("Agent: %s", agent_msg.c_str());
			ImGui::Separator();
		}
		ImGui::EndChild();

		// Input
		ImGui::Text("Update Context Here");
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

		if (ImGui::Button("Save Context Snapshot")) {
			SaveContextSnapshot();
		}
		
		ImGui::SameLine();
		
		if (ImGui::Button("Load Context Snapshot")) {
			LoadContextSnapshot();
		}
		

		ImGui::End();
	}

	void CLLMPanel::SaveContextSnapshot() {
		std::string file_path = CApplication::GetApplicationInstance().SaveFile("BIN File (*.bin)\0*.bin\0", "bin");

		if (!file_path.empty()) {
			CChatbotSerializer serializer(m_manager);
			serializer.SerializeAIContextSnapshot(file_path);
		}
	}

	void CLLMPanel::LoadContextSnapshot() {
		std::string file_path = CApplication::GetApplicationInstance().OpenFile("BIN File (*.bin)\0*.bin\0");

		if (!file_path.empty()) {
			CChatbotSerializer serializer(m_manager);
			serializer.DeserializeAIContextSnapshot(file_path);
		}
	}
}