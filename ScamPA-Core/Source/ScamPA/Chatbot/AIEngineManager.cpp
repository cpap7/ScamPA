#include "spapch.h"
#include "AIEngineManager.h"
#include "../Core/Logger.h"

namespace SPA {
	CAIEngineManager::CAIEngineManager(const SModelPaths& a_paths)
		: m_paths(a_paths) {
		Setup(a_paths);
	}

	CAIEngineManager::~CAIEngineManager() {
		Shutdown();
	}

	void CAIEngineManager::Setup(const SModelPaths& a_paths) {
		if (!a_paths.m_stt_model_path.empty()) { 
			LoadSTT(a_paths.m_stt_model_path);
		}
		if (!a_paths.m_llm_model_path.empty()) {
			LoadLLM(a_paths.m_llm_model_path);
		}
		if (!a_paths.m_tts_model_onnx_path.empty() && !a_paths.m_tts_model_onnx_json_path.empty()) {
			LoadTTS(a_paths.m_tts_model_onnx_path, a_paths.m_tts_model_onnx_json_path);
		}

		//if (IsAllInitialized()) {
		//	SPA_CLIENT_INFO("(AI Engine Manager) AI Engines loaded successfully!");
		//}
	}

	void CAIEngineManager::LoadSTT(const std::string& a_stt_model_path) {
		if (m_stt_engine_loading.load() || m_stt_engine) {
			return;
		}

		if (m_stt_thread.joinable()) {
			m_stt_thread.join();
		}

		m_paths.m_stt_model_path = a_stt_model_path; // For reloading
		m_stt_engine_loading.store(true);

		m_stt_thread = std::thread([this]() {
			auto stt_config	= VoxBox::CSTTEngine::GetDefaultConfig();
			stt_config.m_model_config.m_model_path = m_paths.m_stt_model_path;
			m_stt_engine = VoxBox::CSTTEngine::Create(stt_config);
			
			m_stt_engine_loading.store(false); // Reset
		});
	}

	void CAIEngineManager::LoadLLM(const std::string& a_llm_model_path) {
		if (m_llm_engine_loading.load() || m_llm_engine) {
			return;
		}

		if (m_llm_thread.joinable()) {
			m_llm_thread.join();
		}

		m_llm_engine_loading.store(true);
		m_paths.m_llm_model_path = a_llm_model_path; // For reloading

		m_llm_thread = std::thread([this]() {
			auto llm_config = VoxBox::CLLMEngine::GetDefaultConfig();
			llm_config.m_model_config.m_model_path = m_paths.m_llm_model_path;
			llm_config.m_prompt_config.m_try_prompts_by_model = true;
			m_llm_engine = VoxBox::CLLMEngine::Create(llm_config);

			m_llm_engine_loading.store(false); // Reset
		});		
	}

	void CAIEngineManager::LoadTTS(const std::string& a_tts_model_path, const std::string& a_tts_model_json_path) {
		if (m_tts_engine_loading.load() || m_tts_engine) {
			return;
		}

		if (m_tts_thread.joinable()) {
			m_tts_thread.join();
		}

		m_paths.m_tts_model_onnx_path = a_tts_model_path;			// For reloading
		m_paths.m_tts_model_onnx_json_path = a_tts_model_json_path;

		m_tts_engine_loading.store(true);
		m_tts_thread = std::thread([this]() {
			auto tts_config = VoxBox::CTTSEngine::GetDefaultConfig();
			tts_config.m_voice_config.m_model_onnx_path = m_paths.m_tts_model_onnx_path;
			tts_config.m_voice_config.m_model_onnx_json_path = m_paths.m_tts_model_onnx_json_path;
			//tts_config.m_voice_config.m_espeak_data_path = m_paths.m_tts_espeak_data_path;
			m_tts_engine = VoxBox::CTTSEngine::Create(tts_config);

			m_tts_engine_loading.store(false); // Reset
		});
	}

	void CAIEngineManager::Shutdown() {
		// Finish pending loads before destruction
		if (m_stt_thread.joinable()) {
			m_stt_thread.join();
		}
		if (m_llm_thread.joinable()) {
			m_llm_thread.join();
		}
		if (m_tts_thread.joinable()) {
			m_tts_thread.join();
		}

		m_stt_engine.reset();
		m_llm_engine.reset();
		m_tts_engine.reset();
	}
}