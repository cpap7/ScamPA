#pragma once
#include <ScamPA/Core/Panel.h>
#include <ScamPA/Audio/AudioDevice.h>

#include "../AIAgent/AIAgentContext.h"

#include <string>
#include <memory>

namespace SPA {

	class CSTTPanel : public IPanel {
	private:
		std::string m_last_transcript;
		std::vector<SAudioDeviceInfo> m_device_list;

		CAIAgentContext& m_ai_agent_context;
		std::unique_ptr<IAudioDevice> m_audio_input_device;
		
		int32_t m_selected_device_index = -1;
		bool m_device_list_dirty		= true; // Refresh on 1st frame
		bool m_is_recording				= false;

	public:
		explicit CSTTPanel(CAIAgentContext& a_context);
		~CSTTPanel();
		
		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUIRender() override;

	private:
		void RefreshDeviceList();
	};

}
