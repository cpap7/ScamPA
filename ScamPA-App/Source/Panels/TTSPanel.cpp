#include "TTSPanel.h"

#include <ScamPA/Core/Logger.h>
#include <ScamPA/Audio/AudioOutputDevice.h>

#include <imgui.h>

namespace SPA {
	CTTSPanel::CTTSPanel(CAIAgentContext& a_context) 
		: m_ai_agent_context(a_context) {
		OnInit();
	}

	CTTSPanel::~CTTSPanel() {
		OnShutdown();
	}

	void CTTSPanel::OnInit() {
		SAudioDeviceConfig config;
		config.m_sample_rate = 22050; // piper default
		config.m_channels = 1;
		config.m_sample_format = EAudioSampleFormat::Int16;
		config.m_device_type = EAudioDeviceType::Playback; // Play to speakers

		m_audio_output_device = IAudioDevice::Create(config);

		RefreshDeviceList();
	}

	void CTTSPanel::OnShutdown() {
		m_audio_output_device.reset();
	}

	void CTTSPanel::OnUIRender() {
		ImGui::Begin("Text-To-Speech Settings");
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
		ImGui::TextDisabled("Model Path ");
		ImGui::SameLine();
		ImGui::InputText("##ttsmodelonnxpath", (char*)m_ai_agent_context.GetTTSOnnxModelPath().c_str(), ImGuiInputTextFlags_ReadOnly);
		
		ImGui::TextDisabled("Config Path");
		ImGui::SameLine();
		ImGui::InputText("##ttsmodelonnxjsonpath", (char*)m_ai_agent_context.GetTTSOnnxModelJsonPath().c_str(), ImGuiInputTextFlags_ReadOnly);


		ImGui::InputTextMultiline("##tts_text", m_text_buffer, sizeof(m_text_buffer), ImVec2(-1, 100));
		if (ImGui::Button("Synthesize Audio") && m_text_buffer[0] != '\0') {
			// NOTE: This will be blocking, so this might be better off being offloaded to a worker thread
			VoxBox::SAudioResult result = tts_engine->Synthesize(m_text_buffer);
			if (result.Success()) {
				SPA_CLIENT_INFO("TTS synthesized {0} samples @ {1} Hz", result.SampleCount(), result.SampleRate());

				auto* output_device = static_cast<CAudioOutputDevice*>(m_audio_output_device.get());
				if (output_device) {
					output_device->ClearBuffer();
					output_device->SubmitSamples(result.SampleData(), result.SampleCount());
					output_device->Start();
				}
			}
		}


		ImGui::Separator();
		ImGui::Text("Voice Settings");

		if (ImGui::SliderFloat("Verbal Delay", &m_verbal_delay, 0.5f, 2.0f, "%.2f")) {
			tts_engine->SetSpeed(m_verbal_delay);
		}

		ImGui::Separator();

		ImGui::Text("Device Settings");

		{ // Output device selection
			const char* preview = "System Default";
			if ((m_device_settings.m_selected_device_index >= 0) && (m_device_settings.m_selected_device_index < static_cast<int32_t>(m_device_settings.m_device_list.size()))) {
				preview = m_device_settings.m_device_list[m_device_settings.m_selected_device_index].m_name.c_str();
			}

			ImGui::SetNextItemWidth(-1);
			if (ImGui::BeginCombo("Output Device", preview)) {
				// First entry = "System Default"
				bool is_default_selected = (m_device_settings.m_selected_device_index == -1);
				if (ImGui::Selectable("System Default", is_default_selected)) {
					if (m_device_settings.m_selected_device_index != -1) {
						m_device_settings.m_selected_device_index = -1;
						
						if (m_audio_output_device) {
							m_audio_output_device->SetDeviceByIndex(-1);
						}
					}
				}
				
				for (int32_t i{}; i < static_cast<int32_t>(m_device_settings.m_device_list.size()); ++i) {
					const auto& info = m_device_settings.m_device_list[i];
					bool is_selected = (m_device_settings.m_selected_device_index == i);

					// Label default devices within the list
					std::string label = info.m_is_default ? info.m_name + " (Default)" : info.m_name;

					if (ImGui::Selectable(label.c_str(), is_selected)) {
						if (m_device_settings.m_selected_device_index != i) {
							m_device_settings.m_selected_device_index = i;
							if (m_audio_output_device) {
								m_audio_output_device->SetDeviceByIndex(info.m_index);
							}
						}
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				
				ImGui::EndCombo();
			}

			//ImGui::SameLine();
			if (ImGui::Button("Refresh Device List##output")) {
				RefreshDeviceList();
			}

		}

		ImGui::Separator();

		ImGui::End();
	}

	void CTTSPanel::RefreshDeviceList() {
		if (m_audio_output_device) {
			m_device_settings.m_device_list = m_audio_output_device->GetDeviceList();
			m_device_settings.m_selected_device_index = -1; // Reset to system default on refresh

			// Auto select current default
			for (int32_t i{}; i < static_cast<int32_t>(m_device_settings.m_device_list.size()); ++i) {
				if (m_device_settings.m_device_list[i].m_is_default) {
					m_device_settings.m_selected_device_index = i;
					break;
				}
			}
		}
	}
}