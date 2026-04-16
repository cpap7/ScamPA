#pragma once
#include <ScamPA/Core/GUIInterfaces.h>
#include <ScamPA/Chatbot/ChatbotStateMachine.h>
#include <ScamPA/Chatbot/ChatComponents.h>

#include <string>
#include <vector>

namespace SPA {

	struct SPollableResults { // Copies of pollable results which are updated each frame
		std::string m_stt_transcript;
		std::string m_llm_response;
		std::string m_error;

		float m_stt_confidence = 0.0f;
	};

	struct SChatbotAgent {
		SPollableResults m_pollable_results;
		SChatSession m_active_session;

		std::string m_name;
		std::unique_ptr<CChatbotStateMachine> m_state_machine = nullptr;

		bool m_results_committed = true; // For tracking transcript & response commits to the panel

		inline bool IsPipelineActive() { return (m_state_machine != nullptr) ? (m_state_machine->GetState() == EChatbotState::Inferring) || (m_state_machine->GetState() == EChatbotState::Speaking) : false; }

		inline const SChatSession& GetActiveSession() const { return m_active_session; }
		inline void SetActiveSession(SChatSession a_session) {
			m_name = a_session.m_agent_name; // Update name
			m_active_session = std::move(a_session); 
		}
	};

	class CChatbotPanel : public IPanel {
	private:
		std::vector<std::unique_ptr<SChatbotAgent>> m_agents;
		CAIEngineManager& m_manager;

		int m_agent_counter = 0;
		int m_pending_remove_index = -1;
	
	public:
		explicit CChatbotPanel(CAIEngineManager& a_manager);
		~CChatbotPanel() = default;

		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUIRender() override;

		SChatbotAgent* CreateAgent(const std::string& a_agent_name = "");
		void RemoveAgent(size_t a_index);

	private: // Helpers
		void RenderAgentTab(SChatbotAgent& a_agent);
		void UpdateChatLog(SChatbotAgent& a_agent);
		void CommitChatLog(SChatbotAgent& a_agent);

		void SaveToYAML(SChatbotAgent& a_agent);
		void SaveToJSON(SChatbotAgent& a_agent);

		void LoadFromYAML(SChatbotAgent& a_agent);
		void LoadFromJSON(SChatbotAgent& a_agent);
	};
}

