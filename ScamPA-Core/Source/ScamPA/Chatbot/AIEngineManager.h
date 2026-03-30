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

	
	class CAIEngineManager {
	private:
		SModelPaths m_paths;

		// For async loading
		std::thread m_stt_thread;
		std::thread m_llm_thread;
		std::thread m_tts_thread;

		std::unique_ptr<VoxBox::CSTTEngine> m_stt_engine = nullptr;
		std::unique_ptr<VoxBox::CLLMEngine> m_llm_engine = nullptr;
		std::unique_ptr<VoxBox::CTTSEngine> m_tts_engine = nullptr;

		std::atomic<bool> m_stt_engine_loading{ false };
		std::atomic<bool> m_llm_engine_loading{ false };
		std::atomic<bool> m_tts_engine_loading{ false };
		
		//bool m_is_initialized = false;

	public:
		explicit CAIEngineManager(const SModelPaths& a_paths);
		~CAIEngineManager();
		
		// Called by panel
		void Setup(const SModelPaths& a_paths);

		// Async loading functions
		void LoadSTT(const std::string& a_stt_model_path);
		void LoadLLM(const std::string& a_llm_model_path);
		void LoadTTS(const std::string& a_tts_model_path, const std::string& a_tts_model_json_path);
		
		void Shutdown();

		// Accessors used by panels
		inline bool IsSTTInitialized() const							{ return m_stt_engine != nullptr;				}
		inline bool IsLLMInitialized() const							{ return m_llm_engine != nullptr;				}
		inline bool IsTTSInitialized() const							{ return m_tts_engine != nullptr;				}
		inline bool IsAllInitialized() const							{ return IsSTTInitialized() && IsLLMInitialized() && IsTTSInitialized(); }

		inline bool IsSTTLoading() const								{ return m_stt_engine_loading.load();			}
		inline bool IsLLMLoading() const								{ return m_llm_engine_loading.load();			}
		inline bool IsTTSLoading() const								{ return m_tts_engine_loading.load();			}

		inline VoxBox::CSTTEngine* GetSTTEngine() const					{ return m_stt_engine.get();					}
		inline VoxBox::CLLMEngine* GetLLMEngine() const					{ return m_llm_engine.get();					}
		inline VoxBox::CTTSEngine* GetTTSEngine() const					{ return m_tts_engine.get();					}

		inline const std::string& GetSTTModelPath() const				{ return m_paths.m_stt_model_path;				}
		inline const std::string& GetLLMModelPath() const				{ return m_paths.m_llm_model_path;				}
		inline const std::string& GetTTSOnnxModelPath() const			{ return m_paths.m_tts_model_onnx_path;			}
		inline const std::string& GetTTSOnnxModelJsonPath() const		{ return m_paths.m_tts_model_onnx_json_path;	}

		inline void SetSTTModelPath(const std::string& a_path)			{ m_paths.m_stt_model_path = a_path;			}
		inline void SetLLMModelPath(const std::string& a_path)			{ m_paths.m_llm_model_path = a_path;			}
		inline void SetTTSOnnxModelPath(const std::string& a_path)		{ m_paths.m_tts_model_onnx_path = a_path;		}
		inline void SetTTSOnnxModelJsonPath(const std::string& a_path)	{ m_paths.m_tts_model_onnx_json_path = a_path;	}

	};
}

