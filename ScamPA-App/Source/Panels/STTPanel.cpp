#include "STTPanel.h"
#include <ScamPA/Core/Logger.h>
#include <ScamPA/Audio/AudioInputDevice.h>
#include <ScamPA/Core/Application.h>

#include <imgui.h>

namespace SPA {

	namespace Utilities {
		static std::vector<float> ConvertToFloatSamples(const std::vector<int16_t>& a_raw_samples) {
			std::vector<float> float_samples(a_raw_samples.size());

			for (size_t i{}; i < a_raw_samples.size(); ++i) {
				float_samples[i] = static_cast<float>(a_raw_samples[i] / 32768.0f);
			}

			return float_samples;
		}

	}

	CSTTPanel::CSTTPanel(CAIEngineManager& a_manager)
		: m_manager(a_manager) {
		OnInit();
	}

	CSTTPanel::~CSTTPanel() {
		OnShutdown();
	}

	void CSTTPanel::OnInit() {
		SAudioDeviceConfig config;
		config.m_sample_rate	= 16000; // whisper.cpp default
		config.m_channels		= 1;
		config.m_sample_format	= EAudioSampleFormat::Int16;
		config.m_device_type	= m_selected_device_type; // Default = loopback (audio from speakers)
		
		m_audio_input_device = IAudioDevice::Create(config);
		
		RefreshDeviceList();
	}

	void CSTTPanel::OnShutdown() {
		m_audio_input_device.reset();
	}

	void CSTTPanel::OnUIRender() {
		ImGui::Begin("Speech-To-Text Settings");

		auto* stt_engine = m_manager.GetSTTEngine();
		if (!stt_engine) {
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "STT Engine Not Loaded");
			if (ImGui::Button("Load STT Model")) {
				std::string stt_model_path = CApplication::GetApplicationInstance().OpenFile("Whisper Model (*.bin)\0*.bin\0\0");
				if (!stt_model_path.empty()) {
					m_manager.LoadSTT(stt_model_path);
				}
			}
			ImGui::End();
			return;
		}
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "STT Engine Loaded");
		ImGui::TextDisabled("Model Path");
		ImGui::SameLine();
		ImGui::InputText("##sttmodelpath", (char*)m_manager.GetSTTModelPath().c_str(), ImGuiInputTextFlags_ReadOnly);
		if (ImGui::Button("Load STT Model")) {
			std::string stt_model_path = CApplication::GetApplicationInstance().OpenFile("Whisper Model (*.bin)\0*.bin\0\0");
			if (!stt_model_path.empty()) {
				m_manager.LoadSTT(stt_model_path);
			}
		}

		ImGui::Separator();

		auto* input_device = static_cast<CAudioInputDevice*>(m_audio_input_device.get());
		if (!input_device) {
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Audio Device Failed To Initialize");
			ImGui::End();
			return;
		}

		ImGui::Text("Input Audio Device Settings");
		
		{ // Device type selection
			static const char* device_type_labels[] = {
				"Capture (Mic)",
				"Loopback (Speakers)"
			};
			
			static const EAudioDeviceType device_type_values[] = {
				EAudioDeviceType::Capture,
				EAudioDeviceType::Loopback
			};

			constexpr uint32_t device_type_count = 2;

			// Find current index for preview
			uint32_t current_index = 0;
			for (uint32_t i{}; i < device_type_count; ++i) {
				if (device_type_values[i] == m_selected_device_type) {
					current_index = i;
					break;
				}
			}

			if (m_is_recording) { // Do not allow changes while recording audio
				ImGui::BeginDisabled();
			}

			ImGui::SetNextItemWidth(-1);
			if (ImGui::BeginCombo("Device Type", device_type_labels[current_index])) {
				for (uint32_t i{}; i < 2; ++i) {
					bool is_selected = (device_type_values[i] == m_selected_device_type);
					
					if (ImGui::Selectable(device_type_labels[i], is_selected)) {
						if (device_type_values[i] != m_selected_device_type) {
							// Update internal value, then refresh
							m_selected_device_type = device_type_values[i]; 
							Reinit();
						}
					}

					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			if (m_is_recording) {
				ImGui::EndDisabled();
			}
		}
		
		

		{ // Input device selection
			const char* preview = "System Default";
			if ((m_device_settings.m_selected_device_index >= 0) && 
				(m_device_settings.m_selected_device_index < static_cast<int32_t>(m_device_settings.m_device_list.size()))) {
				preview = m_device_settings.m_device_list[m_device_settings.m_selected_device_index].m_name.c_str();
			}

			if (m_is_recording) { // Do not allow changes while recording
				ImGui::BeginDisabled();
			}

			ImGui::SetNextItemWidth(-1);
			if (ImGui::BeginCombo("Input Device", preview)) {
				// First entry = "System Default"
				bool is_default_selected = (m_device_settings.m_selected_device_index == -1);
				if (ImGui::Selectable("System Default", is_default_selected)) {
					if (m_device_settings.m_selected_device_index != -1) {
						m_device_settings.m_selected_device_index = -1;

						if (m_audio_input_device) {
							m_audio_input_device->SetDeviceByIndex(-1);
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
							if (m_audio_input_device) {
								m_audio_input_device->SetDeviceByIndex(info.m_index);
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
			if (ImGui::Button("Refresh Device List##input")) {
				RefreshDeviceList();
			}

			if (m_is_recording) {
				ImGui::EndDisabled();
			}

		}

		ImGui::Separator();

		if (m_is_recording) {
			if (ImGui::Button("Stop Recording")) {
				m_is_recording = false;
				input_device->Stop();

				// Drain samples & convert to float for whisper.cpp
				std::vector<int16_t> raw_samples = input_device->ConsumeBuffer();
				if (!raw_samples.empty()) { 
					std::vector<float> float_samples = Utilities::ConvertToFloatSamples(raw_samples);
					VoxBox::STranscriptResult result = stt_engine->Transcribe(float_samples);
					
					m_last_transcript = result.Success() ? result.m_text : "";
					if (m_last_transcript.empty()) {
						SPA_CORE_WARN("(STT Panel) Transcription failed!");
					}
				}
			}
		}
		else {
			if (ImGui::Button("Start Recording")) {
				m_is_recording = true;
				input_device->Start();
			}
		}
		if (!m_last_transcript.empty()) {
			ImGui::Separator();
			ImGui::TextWrapped("Transcript: %s", m_last_transcript.c_str());
		}

		ImGui::End();
	}

	void CSTTPanel::RefreshDeviceList() {
		if (m_audio_input_device) {
			m_device_settings.m_device_list = m_audio_input_device->GetDeviceList();
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

	void CSTTPanel::Reinit() {
		OnShutdown();
		OnInit();
	}
}