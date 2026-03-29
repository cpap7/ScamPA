#pragma once
#include <ScamPA/Core/Panel.h>
#include <ScamPA/Audio/AudioDevice.h>

#include "../AIAgent/AIEngineManager.h"

#include <string>
#include <vector>

namespace SPA {

	class CLLMPanel : public IPanel {
	private:
		char m_input_buffer[1024] = {};
		std::string m_pending_response;
		std::vector<std::pair<std::string, std::string>> m_chat_history; // { user, agent }

		CAIEngineManager& m_manager;
		
	public:
		explicit CLLMPanel(CAIEngineManager& a_manager);
		
		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUIRender() override;

	private:
		void SaveContextSnapshot();
		void LoadContextSnapshot();

	};

}
