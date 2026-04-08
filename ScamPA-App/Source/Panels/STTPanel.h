#pragma once
#include <ScamPA/Core/Panel.h>
#include <ScamPA/Audio/DeviceSettings.h>
#include <ScamPA/Audio/AudioInputDevice.h>
#include <ScamPA/Chatbot/AIEngineManager.h>
#include <ScamPA/Utilities/Timer.h>

#include <string>
#include <memory>

namespace SPA {

	class CSTTPanel : public IPanel {
	private:
		std::string m_last_transcript;
		SAudioDeviceSettings m_device_settings;

		CAIEngineManager& m_manager;
		std::unique_ptr<IAudioDevice> m_audio_input_device;
		
		CTimer m_silence_timer;
		float m_silence_threshold	= 0.005f; // RMS energy floor
		float m_silence_duration	= 1.0f;   // Seconds of quiet after speech before auto-infer
		bool m_speech_detected		= false;
		bool m_is_recording			= false;

		EAudioDeviceType m_selected_device_type = EAudioDeviceType::Loopback;

	public:
		explicit CSTTPanel(CAIEngineManager& a_manager);
		~CSTTPanel();
		
		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUIRender() override;

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
