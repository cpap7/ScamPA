#pragma once
#include <ScamPA/Core/Panel.h>
#include <ScamPA/Audio/AudioDevice.h>

#include "../AIAgent/AIAgentContext.h"

#include <memory>
#include <cstdint>

namespace SPA {

	class CTTSPanel	: public IPanel {
	private:
		char m_text_buffer[2048] = {};
		std::vector<SAudioDeviceInfo> m_device_list;

		CAIAgentContext& m_ai_agent_context;
		std::unique_ptr<IAudioDevice> m_audio_output_device;
		
		float m_verbal_delay			= 1.0f;
		int32_t m_selected_device_index = -1;
		bool m_device_list_dirty		= true; // Refresh on 1st frame

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
