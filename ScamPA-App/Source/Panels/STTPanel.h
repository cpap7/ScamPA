#pragma once
#include <ScamPA/Core/Panel.h>
#include "../AIAgentContext.h"

#include <string>

namespace SPA {

	class CSTTPanel : public IPanel {
	private:
		std::string m_last_transcript;
		CAIAgentContext& m_ai_agent_context;
		bool m_is_recording = false;

	public:
		explicit CSTTPanel(CAIAgentContext& a_context);

		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUIRender() override;
	};

}
