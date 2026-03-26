#include "AIAgentContext.h"

namespace SPA {
	CAIAgentContext::CAIAgentContext(const SModelPaths& a_paths)
		: m_paths(a_paths) {
		Setup();
	}

	CAIAgentContext::~CAIAgentContext() {
		Shutdown();
	}

	void CAIAgentContext::Setup() {
		InitSTT();
		InitLLM();
		InitTTS();

		m_is_initialized = true;
	}

	void CAIAgentContext::InitSTT() {
		auto stt_config = VoxBox::CSTTEngine::GetDefaultConfig();
		stt_config.m_model_config.m_model_path = m_paths.m_stt_model_path;
		m_stt_engine = VoxBox::CSTTEngine::Create(stt_config);
	}

	void CAIAgentContext::InitLLM() {
		auto llm_config = VoxBox::CLLMEngine::GetDefaultConfig();
		llm_config.m_model_config.m_model_path = m_paths.m_llm_model_path;
		llm_config.m_prompt_config.m_try_prompts_by_model = true;
		m_llm_engine = VoxBox::CLLMEngine::Create(llm_config);
	}

	void CAIAgentContext::InitTTS() {
		auto tts_config = VoxBox::CTTSEngine::GetDefaultConfig();
		tts_config.m_voice_config.m_model_onnx_path = m_paths.m_tts_model_onnx_path;
		tts_config.m_voice_config.m_model_onnx_json_path = m_paths.m_tts_model_onnx_json_path;
		//tts_config.m_voice_config.m_espeak_data_path = m_paths.m_tts_espeak_data_path;
		m_tts_engine = VoxBox::CTTSEngine::Create(tts_config);
	}

	void CAIAgentContext::Shutdown() {
		m_stt_engine.reset();
		m_llm_engine.reset();
		m_tts_engine.reset();

		m_is_initialized = false;
	}

	
}