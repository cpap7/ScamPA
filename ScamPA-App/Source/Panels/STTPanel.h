#pragma once
#include <ScamPA/Core/Panel.h>

#include "DeviceSettings.h"
#include "../AIAgent/AIAgentContext.h"

#include <string>
#include <memory>

namespace SPA {

	class CSTTPanel : public IPanel {
	private:
		std::string m_last_transcript;
		SAudioDeviceSettings m_device_settings;

		CAIAgentContext& m_ai_agent_context;
		std::unique_ptr<IAudioDevice> m_audio_input_device;
		
		EAudioDeviceType m_selected_device_type = EAudioDeviceType::Loopback;
		bool m_is_recording						= false;

	public:
		explicit CSTTPanel(CAIAgentContext& a_context);
		~CSTTPanel();
		
		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUIRender() override;

	private:
		void RefreshDeviceList();
		void Reinit(); // Used internally after changing the device type within the UI (calls shutdown + init function)

	};

}
