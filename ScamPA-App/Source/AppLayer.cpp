#include "AppLayer.h"

#include <ScamPA/ScamPA.h>

namespace SPA {
	CAppLayer::CAppLayer() {

	}

	CAppLayer::~CAppLayer() {
	
	}

	void CAppLayer::OnAttach() {
		std::string version			= CApplication::GetApplicationInstance().GetApplicationVersion();
		std::string configuration	= CApplication::GetApplicationInstance().GetConfigurationName();
		std::string platform		= CApplication::GetApplicationInstance().GetPlatformName();
		std::string window_title	= "ScamPA App (" + version + ") - " + configuration + " (" + platform + ")";
		CApplication::GetApplicationInstance().GetWindowHandle().SetTitle(window_title);

		// Configure paths
		SModelPaths paths;
		paths.m_stt_model_path				= "Assets/Models/STT/ggml-small.bin";
		paths.m_llm_model_path				= "Assets/Models/LLM/llama-3.2-1b-instruct-q8_0.gguf";
		
		paths.m_tts_model_onnx_json_path	= "Assets/Models/TTS/en_US-libritts_r-medium.onnx.json";
		paths.m_tts_model_onnx_path			= "Assets/Models/TTS/en_US-libritts_r-medium.onnx";
		//paths.m_tts_espeak_data_path		= "Assets/Models/TTS/espeak-ng-data";

		// Create AI agent context & state machine
		m_manager				= std::make_unique<CAIEngineManager>(paths);
		m_chatbot_state_machine = std::make_unique<CChatbotStateMachine>(*m_manager);

		// Create panels
		m_stt_panel			= std::make_unique<CSTTPanel>(*m_manager);
		m_llm_panel			= std::make_unique<CLLMPanel>(*m_manager);
		m_tts_panel			= std::make_unique<CTTSPanel>(*m_manager);
		m_chatbot_panel		= std::make_unique<CChatbotPanel>(*m_chatbot_state_machine);

	}

	void CAppLayer::OnDetach() {
		m_tts_panel.reset();
		m_llm_panel.reset();
		m_stt_panel.reset();
		m_chatbot_panel.reset();
		m_chatbot_state_machine.reset();
		m_manager.reset();
	}

	void CAppLayer::OnUpdate(float a_timestep) {
		// TODO: poll async inference results here
	}

	void CAppLayer::OnUIRender() {
		m_stt_panel->OnUIRender();
		m_llm_panel->OnUIRender();
		m_tts_panel->OnUIRender();
		m_chatbot_panel->OnUIRender();
	}

	void CAppLayer::OnEvent(IEvent& a_event) {

	}
}