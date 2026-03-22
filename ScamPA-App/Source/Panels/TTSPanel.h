#pragma once
#include <ScamPA/Core/Panel.h>

#include "DeviceSettings.h"
#include "../AIAgent/AIAgentContext.h"

#include <memory>
#include <cstdint>

namespace SPA {

	class CTTSPanel	: public IPanel {
	private:
		char m_text_buffer[2048] = {};
		SAudioDeviceSettings m_device_settings;

		CAIAgentContext& m_ai_agent_context;
		std::unique_ptr<IAudioDevice> m_audio_output_device;
		
		float m_verbal_delay = 1.0f;

	public:
		explicit CTTSPanel(CAIAgentContext& a_context);
		~CTTSPanel();

		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUIRender() override;

	private:
		void RefreshDeviceList();
	};

}
