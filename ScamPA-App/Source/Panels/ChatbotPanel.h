#pragma once
#include <ScamPA/Core/Panel.h>
#include <ScamPA/Chatbot/ChatbotStateMachine.h>
#include <ScamPA/Chatbot/ChatComponents.h>

#include <string>
#include <vector>

namespace SPA {

	class CChatbotPanel : public IPanel {
	private:
		SChatSession m_active_session;

		// Copies of pollable results which are updated each frame
		std::string m_stt_transcript;
		std::string m_llm_response;
		std::string m_error;

		CChatbotStateMachine& m_state_machine;

		bool m_results_committed = true; // For tracking transcript & response commits to the panel
	
	public:
		explicit CChatbotPanel(CChatbotStateMachine& a_state_machine);

		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUIRender() override;

		inline bool IsPipelineActive() { return (m_state_machine.GetState() == EChatbotState::Inferring) || (m_state_machine.GetState() == EChatbotState::Speaking); }

		inline const SChatSession& GetActiveSession() const { return m_active_session; }
		inline void SetActiveSession(SChatSession a_session) { m_active_session = std::move(a_session); }

	private: // Helpers
		void UpdateChatLog();
		void CommitChatLog();

		void SaveToYAML();
		void SaveToJSON();

		void LoadFromYAML();
		void LoadFromJSON();
	};
}

