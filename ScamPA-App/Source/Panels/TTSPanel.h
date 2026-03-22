#pragma once
#include <ScamPA/Core/Panel.h>
#include "../AIAgentContext.h"

#include <string>

namespace SPA {

	class CTTSPanel	: public IPanel {
	private:
		char m_text_buffer[2048] = {};
		CAIAgentContext& m_ai_agent_context;
		float m_speed = 1.0f;

	public:
		explicit CTTSPanel(CAIAgentContext& a_context);

		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUIRender() override;
	};

}
