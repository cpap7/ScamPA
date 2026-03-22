#include "STTPanel.h"
#include <imgui.h>

namespace SPA {
	CSTTPanel::CSTTPanel(CAIAgentContext& a_context) 
		: m_ai_agent_context(a_context) {

	}

	void CSTTPanel::OnInit() {

	}

	void CSTTPanel::OnShutdown() {

	}

	void CSTTPanel::OnUIRender() {
		ImGui::Begin("Speech-To-Text");

		auto* stt_engine = m_ai_agent_context.GetSTTEngine();
		if (!stt_engine) {
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "STT Engine Not Loaded");
			if (ImGui::Button("Load STT Model")) {
				m_ai_agent_context.InitSTT();
			}
			ImGui::End();
			return;
		}
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "STT Engine Loaded");

		// TODO: Wire up audio capture + transcription
		// These are just placeholders for now
		if (m_is_recording) {
			if (ImGui::Button("Stop Recording")) {
				m_is_recording = false;
				// TODO: Create audio buffer, call stt_engine->Transcribe()
			}
		}
		else {
			if (ImGui::Button("Start Recording")) {
				m_is_recording = true;
				// TODO: Begin audio capture
			}
		}
		if (!m_last_transcript.empty()) {
			ImGui::Separator();
			ImGui::TextWrapped("Transcript: %s", m_last_transcript.c_str());
		}

		ImGui::End();
	}
}