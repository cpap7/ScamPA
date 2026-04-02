#pragma once
#include <ScamPA/Core/Panel.h>
#include <ScamPA/Chatbot/AIEngineManager.h>

#include <string>
#include <vector>

namespace SPA {

	class CLLMPanel : public IPanel {
	private:
		char m_input_buffer[1024] = {};
		std::string m_pending_response;
		std::vector<std::pair<std::string, std::string>> m_chat_history; // { user, agent }

		CAIEngineManager& m_manager;

		uint32_t m_seed = -1;		// -1 = random; wraps to 0xFFFFFFFF = LLAMA_DEFAULT_SEED;
		int32_t m_top_k = 0;		// (<= 0) = vocab size
		float m_top_p = 1.0f;		// 1.0f = disabled
		float m_min_p = 0.0f;		// 0.0f = disabled
		float m_temp = 0.0f;		// temp <= 0.0 to sample greedily, 0.0 to not output probabil.
		
	public:
		explicit CLLMPanel(CAIEngineManager& a_manager);
		
		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUIRender() override;

	private:
		void SaveContextSnapshot();
		void LoadContextSnapshot();

	};

}
