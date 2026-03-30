#pragma once
#include <ScamPA/Core/Panel.h>
#include <ScamPA/Chatbot/AIEngineManager.h>
#include <ScamPA/Audio/DeviceSettings.h>

#include <memory>
#include <cstdint>

namespace SPA {

	class CTTSPanel	: public IPanel {
	private:
		char m_text_buffer[2048] = {};
		SAudioDeviceSettings m_device_settings;

		CAIEngineManager& m_manager;
		std::unique_ptr<IAudioDevice> m_audio_output_device;
		
		float m_verbal_delay	= 1.0f;
		float m_noise_scale		= 1.0f;

	public:
		explicit CTTSPanel(CAIEngineManager& a_manager);
		~CTTSPanel();

		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUIRender() override;

	private:
		void RefreshDeviceList();
	};

}
