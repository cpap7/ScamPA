#pragma once
#include <ScamPA/Core/GUIInterfaces.h>

#include <ScamPA/Chatbot/AIEngineManager.h>

#include <string>
#include <vector>

namespace SPA {

	class CLLMTab : public ITab {
	private:
		// Context input
		char m_input_buffer[1024] = {};

		// Interface-related
		std::string m_name = "LLM Settings";

		// Context output
		std::string m_pending_response;
		std::vector<std::pair<std::string, std::string>> m_chat_history; // { user, agent }

		// STT/LLM/TTS manager
		CAIEngineManager& m_manager;

		// Sampler settings
		uint32_t m_seed = -1;		// -1 = random; wraps to 0xFFFFFFFF = LLAMA_DEFAULT_SEED;
		int32_t m_top_k = 0;		// (<= 0) = vocab size
		float m_top_p	= 1.0f;		// 1.0f = disabled
		float m_min_p	= 0.0f;		// 0.0f = disabled
		float m_temp	= 0.8f;		// temp <= 0.0 to sample greedily, 0.0 to not output probabil.
		
	public:
		explicit CLLMTab(CAIEngineManager& a_manager);
		~CLLMTab() = default;

		inline virtual const std::string& GetTabName() const override { return m_name; }

		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnTabRender() override;

	private:
		void DisplayFilePathSettings();
		void DisplaySamplerSettings();
		void DisplayContextSettings();

		void SaveContextSnapshot();
		void LoadContextSnapshot();

	};

}
