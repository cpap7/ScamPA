#pragma once

#include "../Utilities/UUID.h"

#include <vector>
#include <string>

namespace SPA {
	// Chatlog/string-atlas alias
	//using ChatLog = std::vector<std::pair<std::string, std::string>>;

	struct SChatExchange { // Single prompt/response exchange within a session
		std::string m_prompt;
		std::string m_response;
		CUUID32 m_uuid; // Auto generated on construction
		float m_stt_confidence = 0.0f;

		SChatExchange() = default;
		SChatExchange(const std::string& a_prompt, const std::string& a_response, float a_stt_confidence) 
			: m_prompt(a_prompt), m_response(a_response), m_stt_confidence(a_stt_confidence) { }
		SChatExchange(const std::string& a_prompt, const std::string& a_response, float a_stt_confidence, const CUUID32& a_uuid) 
			: m_prompt(a_prompt), m_response(a_response), m_stt_confidence(a_stt_confidence), m_uuid(a_uuid) { }
		
		~SChatExchange() = default;
		SChatExchange(const SChatExchange& a_other) = default;
		SChatExchange(SChatExchange&& a_other) = default;

		// TODO: change return type to void; not necessary as part of the method signature
		SChatExchange& operator=(const SChatExchange& a_other) = default;
		SChatExchange& operator=(SChatExchange&& a_other) = default;

	};

	struct SChatSession { // Manages ordered list of exchanges
		std::string m_name;
		std::vector<SChatExchange> m_exchanges;
		CUUID64 m_uuid; // Auto generated on construction
		
		SChatSession() = default;		
		SChatSession(const std::string& a_name, const std::vector<SChatExchange>& a_exchanges) 
			: m_name(a_name), m_exchanges(a_exchanges) { }
		SChatSession(const std::string& a_name, const std::vector<SChatExchange>& a_exchanges, const CUUID64& a_uuid) 
			: m_name(a_name), m_exchanges(a_exchanges), m_uuid(a_uuid) { }
		
		~SChatSession() = default;
		SChatSession(const SChatSession& a_other) = default;
		SChatSession(SChatSession&& a_other) = default;

		// TODO: change return type to void; not necessary as part of the method signature
		SChatSession& operator=(const SChatSession& a_other) = default;
		SChatSession& operator=(SChatSession&& a_other) = default;

		// Used by chatbot panel to append chat exchanges
		inline void AddExchange(const std::string& a_prompt, const std::string& a_response, float a_stt_confidence = 0.0f) {
			m_exchanges.emplace_back(a_prompt, a_response, a_stt_confidence);
		}
	};

	

	
}