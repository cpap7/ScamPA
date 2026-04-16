#pragma once
#include <ScamPA/Core/GUIInterfaces.h>

#include <ScamPA/Chatbot/AIEngineManager.h>

// Tabs
#include "Tabs/STTTab.h"
#include "Tabs/LLMTab.h"
#include "Tabs/TTSTab.h"
#include "Tabs/SIPTab.h"

#include <memory>

namespace SPA {
	class CSettingsPanel : public IPanel {
	private:
		std::unique_ptr<CSTTTab> m_stt_tab = nullptr;
		std::unique_ptr<CLLMTab> m_llm_tab = nullptr;
		std::unique_ptr<CTTSTab> m_tts_tab = nullptr;
		std::unique_ptr<CSIPTab> m_sip_tab = nullptr;

	public:
		explicit CSettingsPanel(CAIEngineManager& a_manager);
		~CSettingsPanel();

		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUIRender() override;

		// Accessors
		inline CSTTTab& GetSTTTab() { return *m_stt_tab; }
		inline CLLMTab& GetLLMTab() { return *m_llm_tab; }
		inline CTTSTab& GetTTSTab() { return *m_tts_tab; }
		inline CSIPTab& GetSIPTab() { return *m_sip_tab; }

	};

}
