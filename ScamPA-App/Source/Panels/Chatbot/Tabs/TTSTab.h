#pragma once
#include <ScamPA/Core/GUIInterfaces.h>

#include <ScamPA/Chatbot/AIEngineManager.h>

#include <ScamPA/Audio/AudioDeviceSettings.h>

#include <memory>
#include <cstdint>

namespace SPA {
	// Text-to-speech tab

	class CTTSTab : public ITab {
	private:
		// Input buffer (for debug tests)
		char m_text_buffer[2048] = {};
		
		// Interface-related
		std::string m_name = "TTS Settings";
		
		// Audio device-related
		SAudioDeviceSettings m_device_settings;
		std::unique_ptr<IAudioDevice> m_audio_output_device = nullptr;
		
		// STT/LLM/TTS manager
		CAIEngineManager& m_manager;
		
		// Voice settings
		float m_verbal_delay	= 1.0f;
		float m_noise_scale		= 0.667f;

	public:
		explicit CTTSTab(CAIEngineManager& a_manager);
		~CTTSTab();

		inline virtual const std::string& GetTabName() const override { return m_name; }

		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnTabRender() override;

	private:
		void DisplayFilePathSettings();
		
		void DisplayAudioDeviceSettings();
		void RefreshAudioDeviceList();

		void DisplayVoiceSettings();
		void DisplayDebugUtilities();
	};

}
