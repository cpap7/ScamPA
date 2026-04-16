#include "AppLayer.h"

#include <ScamPA/ScamPA.h>

namespace SPA {
	CAppLayer::CAppLayer() {

	}

	CAppLayer::~CAppLayer() {
	
	}

	void CAppLayer::OnAttach() {
		//SPA_PROFILE_FUNCTION();

		CApplication::GetApplicationInstance().GetWindowHandle().SetTitle(SPA_VERSION_LONG);

		// Configure paths
		SModelPaths paths;
		paths.m_stt_model_path;				 //= "Assets/Models/STT/ggml-small.bin";
		paths.m_llm_model_path;				 //= "Assets/Models/LLM/llama-3.2-1b-instruct-q8_0.gguf";
		paths.m_tts_model_onnx_json_path;	 //= "Assets/Models/TTS/en_US-libritts_r-medium.onnx.json";
		paths.m_tts_model_onnx_path;		 //= "Assets/Models/TTS/en_US-libritts_r-medium.onnx";

		// Create AI agent context & state machine
		m_manager					= std::make_unique<CAIEngineManager>(paths);

		// Create panels
		m_chatbot_panel				= std::make_unique<CChatbotPanel>(*m_manager);
		m_settings_panel			= std::make_unique<CSettingsPanel>(*m_manager);
		m_console_panel				= std::make_unique<CConsolePanel>();
	}

	void CAppLayer::OnDetach() {
		//SPA_PROFILE_FUNCTION();
		
		// Panels
		m_console_panel.reset();
		m_settings_panel.reset();
		m_chatbot_panel.reset();
		
		
		// AI engine manager
		m_manager.reset();
	}

	void CAppLayer::OnUpdate(float a_timestep) {
		
	}

	void CAppLayer::OnUIRender() {		
		//ImGui::ShowDemoWindow();

		m_chatbot_panel->OnUIRender();
		m_settings_panel->OnUIRender();
		m_console_panel->OnUIRender();
	}

	void CAppLayer::OnEvent(IEvent& a_event) {
		// TODO
	}
}