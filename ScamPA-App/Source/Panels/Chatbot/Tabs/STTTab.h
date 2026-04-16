#pragma once
#include <ScamPA/Core/GUIInterfaces.h>

#include <ScamPA/Audio/AudioDeviceSettings.h>
#include <ScamPA/Audio/AudioInputDevice.h>

#include <ScamPA/Chatbot/AIEngineManager.h>

#include <ScamPA/Utilities/Timer.h>

#include <string>
#include <memory>

namespace SPA {
	// Speech-to-text/audio-speech-recognition tab

	class CSTTTab : public ITab {
	private:
		// Interface-related
		std::string m_name = "STT Settings";

		// Output STT
		std::string m_last_transcript;
		
		// Audio config
		SAudioDeviceSettings m_device_settings;

		// STT/LLM/TTS manager
		CAIEngineManager& m_manager;
		
		// Audio-device related
		std::unique_ptr<IAudioDevice> m_audio_input_device = nullptr;
		
		CTimer m_silence_timer;
		float m_silence_threshold	= 0.005f; // RMS energy floor
		float m_silence_duration	= 1.0f;   // Seconds of quiet after speech before auto-infer
		bool m_speech_detected		= false;
		bool m_is_recording			= false;

		EAudioDeviceType m_selected_device_type = EAudioDeviceType::Loopback;

	public:
		explicit CSTTTab(CAIEngineManager& a_manager);
		~CSTTTab();
		
		inline virtual const std::string& GetTabName() const override { return m_name; }

		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnTabRender() override;

	private:
		void DisplayFilePathSettings();
		
		void DisplayAudioDeviceSettings();
		void RefreshAudioDeviceList();
		void ReloadAudioDevice(); // Used internally after changing the device type within the UI (calls shutdown + init function)

		void DisplayDebugUtilities();
		void DrainSamples(CAudioInputDevice* a_input_device);
		
		void Reinit();

	};

}
