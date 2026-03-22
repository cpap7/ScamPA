#include "TTSPanel.h"
#include <ScamPA/Core/Logger.h>

#include <imgui.h>

namespace SPA {
	CTTSPanel::CTTSPanel(CAIAgentContext& a_context) 
		: m_ai_agent_context(a_context) {

	}

	void CTTSPanel::OnInit() {

	}

	void CTTSPanel::OnShutdown() {

	}

	void CTTSPanel::OnUIRender() {
		ImGui::Begin("Text-To-Speech");
		auto* tts_engine = m_ai_agent_context.GetTTSEngine();
		if (!tts_engine) {
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "TTS Engine Not Loaded");
			if (ImGui::Button("Load TTS Model")) {
				m_ai_agent_context.InitTTS();
			}
			ImGui::End();
			return;
		}
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "TTS Engine Loaded");
		ImGui::InputTextMultiline("##tts_text", m_text_buffer, sizeof(m_text_buffer), ImVec2(-1, 100));

		if (ImGui::SliderFloat("Speed", &m_speed, 0.5f, 2.0f, "%.2f")) {
			tts_engine->SetSpeed(m_speed);
		}

		if (ImGui::Button("Synthesize") && m_text_buffer[0] != '\0') {
			// NOTE: This will be blocking, so this might be better off being offloaded to a worker thread
			VoxBox::SAudioResult result = tts_engine->Synthesize(m_text_buffer);
			if (result.Success()) {
				// TODO: Play audio via audio output system
				SPA_CLIENT_INFO("TTS synthesized {0} samples @ {1} Hz", result.SampleCount(), result.SampleRate());
			}
		}

		ImGui::End();
	}
}