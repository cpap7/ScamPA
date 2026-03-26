#pragma once
#include <ScamPA/Core/Panel.h>

#include "../AIAgent/ChatbotStateMachine.h"

#include <string>
#include <vector>

namespace SPA {
	using ChatLog = std::vector<std::pair<std::string, std::string>>;

	class CChatbotPanel : public IPanel {
	private:
		// Copies of pollable results which are updated each frame
		std::string m_stt_transcript;
		std::string m_llm_response;
		std::string m_error;
		ChatLog m_chat_log;

		CChatbotStateMachine& m_state_machine;

		bool m_results_committed = true; // For tracking transcript & response commits to the panel
	
	public:
		explicit CChatbotPanel(CChatbotStateMachine& a_state_machine);

		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUIRender() override;

		inline bool IsPipelineActive() { return (m_state_machine.GetState() == EChatbotState::Inferring) || (m_state_machine.GetState() == EChatbotState::Speaking); }

	private: // Helpers
		void UpdateChatLog();
		void CommitChatLog();
	};
}

