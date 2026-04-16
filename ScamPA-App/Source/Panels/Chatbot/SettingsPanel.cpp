#include "SettingsPanel.h"

#include <imgui.h>

namespace SPA {
	CSettingsPanel::CSettingsPanel(CAIEngineManager& a_manager) {
		// Create AI tabs
		m_stt_tab = std::make_unique<CSTTTab>(a_manager);
		m_llm_tab = std::make_unique<CLLMTab>(a_manager);
		m_tts_tab = std::make_unique<CTTSTab>(a_manager);

		// Create SIP tab
		m_sip_tab = std::make_unique<CSIPTab>();
	}
	
	CSettingsPanel::~CSettingsPanel() {
		OnShutdown();
	}

	void CSettingsPanel::OnInit() {
		// Nothing (for now)
	}

	void CSettingsPanel::OnShutdown() {
		m_sip_tab.reset();
		m_tts_tab.reset();
		m_llm_tab.reset();
		m_stt_tab.reset();
	}

	void CSettingsPanel::OnUIRender() {
		ImGui::Begin("Settings");
		if (ImGui::BeginTabBar("##settings_tab")) {
			
			if (ImGui::BeginTabItem(m_stt_tab->GetTabName().c_str())) {
				m_stt_tab->OnTabRender();
				ImGui::EndTabItem();
			}
			
			if (ImGui::BeginTabItem(m_llm_tab->GetTabName().c_str())) {
				m_llm_tab->OnTabRender();
				ImGui::EndTabItem();
			}
			
			if (ImGui::BeginTabItem(m_tts_tab->GetTabName().c_str())) {
				m_tts_tab->OnTabRender();
				ImGui::EndTabItem();
			}
			
			if (ImGui::BeginTabItem(m_sip_tab->GetTabName().c_str())) {
				m_sip_tab->OnTabRender();
				ImGui::EndTabItem();
			}
			
			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}