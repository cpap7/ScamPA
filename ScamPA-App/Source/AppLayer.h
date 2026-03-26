#pragma once
#include <ScamPA/Core/Layer.h>

#include "AIAgent/AIAgentContext.h"
#include "AIAgent/ChatbotStateMachine.h"

// Panels
#include "Panels/STTPanel.h"
#include "Panels/LLMPanel.h"
#include "Panels/TTSPanel.h"
#include "Panels/ChatbotPanel.h"

#include <memory>

namespace SPA {
	class CAppLayer : public ILayer {
	private:
		std::unique_ptr<CAIAgentContext> m_ai_agent_context;
		std::unique_ptr<CChatbotStateMachine> m_chatbot_state_machine;

		// Panels
		std::unique_ptr<CSTTPanel> m_stt_panel;
		std::unique_ptr<CLLMPanel> m_llm_panel;
		std::unique_ptr<CTTSPanel> m_tts_panel;
		std::unique_ptr<CChatbotPanel> m_chatbot_panel;

	public:
		CAppLayer();
		~CAppLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float a_timestep) override;
		virtual void OnUIRender() override;
		virtual void OnEvent(IEvent& a_event) override;


	};
}