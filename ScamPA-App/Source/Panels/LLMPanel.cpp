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
		SPA_PROFILE_FUNCTION();

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

		// Sampler settings
		// Full vocab -> Top-K filter -> Top-P filter -> Min-P filter -> Temperature scaling -> Weighted random draw(seeded)
		ImGui::Text("Sampler Settings");
		/*
		Temperature (m_temp) 
		- Scales the raw logits before converting to probabilities. 
		- Higher values flatten the distribution (more random/creative), lower values sharpen it (more deterministic/focused). 
		- At 0.0, it becomes greedy sampling, always picks the highest-probability token.
		*/
		if (ImGui::SliderFloat("Temperature", &m_temp, 0.0f, 2.0f, "%.2f")) {
			llm_engine->SetTemperature(m_temp);
		}
		/*
		Top-P (m_top_p, nucleus sampling) 
		- Keeps the smallest set of tokens whose cumulative probability reaches P.
		- At 1.0, it's disabled (all tokens pass). 
		- At 0.9, tokens are sorted by probability and accumulated until they sum to 90% — everything else is discarded. 
			- This adapts dynamically: when the model is confident, fewer tokens pass; when uncertain, more pass.
		*/
		if (ImGui::SliderFloat("Top Probability", &m_top_p, 0.0f, 1.0f, "%.2f")) {
			llm_engine->SetTopP(m_top_p);
		}
		/*
		Min-P (m_min_p) 
		- Discards any token whose probability is less than min_p * probability_of_best_token. 
		- At 0.0, it's disabled. 
		- At 0.05, any token with less than 5% of the top token's probability is removed. 
		- Unlike Top-P, this scales relative to the best candidate rather than using an absolute cumulative threshold.
		*/
		if (ImGui::SliderFloat("Min Probability", &m_min_p, 0.0f, 1.0f, "%.2f")) {
			llm_engine->SetMinP(m_min_p);
		}
		/*
		Top-K (m_top_k) 
		- Keeps only the K most probable tokens and discards the rest. 
		- At 0 (or <= 0), it's disabled — all tokens in the vocabulary are kept. 
		- At 40, only the 40 highest-probability tokens survive to the next stage.
		*/
		if (ImGui::SliderInt("Top K-most Probable", &m_top_k, 0, 200)) {
			llm_engine->SetTopK(m_top_k);
		}

		/*
		Seed (m_seed) 
		- Initializes the random number generator used by llama_sampler_init_dist() for the final weighted random selection.
		- At -1 (0xFFFFFFFF), a random seed is used — different outputs each run. 
		- A fixed seed produces deterministic output given identical context and sampler settings.
		*/
		int seed_input = static_cast<int>(m_seed);
		if (ImGui::InputInt("Seed (-1 = random)", &seed_input)) {
			m_seed = static_cast<uint32_t>(seed_input);
			llm_engine->SetSeed(m_seed);
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
		SPA_PROFILE_FUNCTION();

		std::string file_path = CApplication::GetApplicationInstance().SaveFile("BIN File (*.bin)\0*.bin\0", "bin");

		if (!file_path.empty()) {
			CChatbotSerializer serializer(m_manager);
			serializer.SerializeAIContextSnapshot(file_path);
		}
	}

	void CLLMPanel::LoadContextSnapshot() {
		SPA_PROFILE_FUNCTION();

		std::string file_path = CApplication::GetApplicationInstance().OpenFile("BIN File (*.bin)\0*.bin\0");

		if (!file_path.empty()) {
			CChatbotSerializer serializer(m_manager);
			serializer.DeserializeAIContextSnapshot(file_path);
		}
	}
}