#pragma once
#include <ScamPA/Core/Panel.h>
#include "../AIAgentContext.h"

#include <string>
#include <vector>

namespace SPA {

	class CLLMPanel : public IPanel {
	private:
		char m_input_buffer[1024] = {};
		std::string m_pending_response;
		std::vector<std::pair<std::string, std::string>> m_chat_history; // { user, agent }

		CAIAgentContext& m_ai_agent_context;
		
	public:
		explicit CLLMPanel(CAIAgentContext& a_context);
		
		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUIRender() override;
	};

}
