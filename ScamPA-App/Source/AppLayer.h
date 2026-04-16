#pragma once
#include <ScamPA/Core/Layer.h>

#include <ScamPA/Chatbot/AIEngineManager.h>
//#include <ScamPA/Chatbot/ChatbotStateMachine.h>


// Panels
#include "Panels/Chatbot/ChatbotPanel.h"
#include "Panels/Chatbot/SettingsPanel.h"
#include "Panels/ConsolePanel.h"

#include <memory>

namespace SPA {
	class CAppLayer : public ILayer {
	private:
		// AI engines, which are shared across all agents
		std::unique_ptr<CAIEngineManager> m_manager = nullptr;

		// Panels
		std::unique_ptr<CChatbotPanel> m_chatbot_panel = nullptr;
		std::unique_ptr<CSettingsPanel> m_settings_panel = nullptr;
		std::unique_ptr<CConsolePanel> m_console_panel = nullptr;

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