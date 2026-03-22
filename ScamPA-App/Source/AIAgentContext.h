#pragma once

#include <VoxBoxSTT.h>
#include <VoxBoxLLM.h>
#include <VoxBoxTTS.h>

#include <memory>
#include <string>

namespace SPA {

	struct SModelPaths { // Populated via AppLayer's OnAttach()
		std::string m_stt_model_path;
		
		std::string m_llm_model_path;

		std::string m_tts_model_onnx_path;
		std::string m_tts_model_onnx_json_path;
		//std::string m_tts_espeak_data_path;
	};

	
	class CAIAgentContext {
	private:
		SModelPaths m_paths;

		std::unique_ptr<VoxBox::CSTTEngine> m_stt_engine = nullptr;
		std::unique_ptr<VoxBox::CLLMEngine> m_llm_engine = nullptr;
		std::unique_ptr<VoxBox::CTTSEngine> m_tts_engine = nullptr;
		
		bool m_is_initialized = false;

	public:
		explicit CAIAgentContext(const SModelPaths& a_paths);
		~CAIAgentContext();
		
		// Called by panels
		void InitSTT();
		void InitLLM();
		void InitTTS();
		
		void Shutdown();

		inline bool IsInitialized() const { return m_is_initialized; }

		// Accessors used by panels
		inline VoxBox::CSTTEngine* GetSTTEngine() const { return m_stt_engine.get(); }
		inline VoxBox::CLLMEngine* GetLLMEngine() const { return m_llm_engine.get(); }
		inline VoxBox::CTTSEngine* GetTTSEngine() const { return m_tts_engine.get(); }
	};
}

