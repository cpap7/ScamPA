#include "ChatbotSerializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>
#include <json.hpp>

namespace SPA {
	CChatbotSerializer::CChatbotSerializer(CAIEngineManager& a_manager) 
		: m_manager(a_manager) {

	}

	void CChatbotSerializer::SerializeSession(const SChatSession& a_session, const std::string& a_file_path, ESerializationFormat a_format) {
		switch (a_format) {
			case ESerializationFormat::YAML: SerializeSessionYAML(a_session, a_file_path); break;
			case ESerializationFormat::JSON: SerializeSessionJSON(a_session, a_file_path); break;
		}
	}
	
	SChatSession CChatbotSerializer::DeserializeSession(const std::string& a_file_path, ESerializationFormat a_format) {
		switch (a_format) {
			case ESerializationFormat::YAML: return DeserializeSessionYAML(a_file_path);
			case ESerializationFormat::JSON: return DeserializeSessionJSON(a_file_path);
		}

		return {};
	}

	void CChatbotSerializer::SerializeAIContextSnapshot(const std::string& a_file_path) {
		if (!m_manager.IsLLMInitialized()) {
			return;
		}
		auto* llm_engine = m_manager.GetLLMEngine();
		//llm_engine->SnapshotUpdate();
		llm_engine->SnapshotToFile(a_file_path);
	}

	void CChatbotSerializer::DeserializeAIContextSnapshot(const std::string& a_file_path) {
		if (!m_manager.IsLLMInitialized()) {
			return;
		}
		auto* llm_engine = m_manager.GetLLMEngine();
		llm_engine->SnapshotFromFile(a_file_path);
		//llm_engine->SnapshotUpdate();
		llm_engine->SnapshotRestore();
	}

	void CChatbotSerializer::SerializeSessionYAML(const SChatSession& a_session, const std::string& a_file_path) {
		YAML::Emitter yaml_output;
		yaml_output << YAML::BeginMap; // Start
		yaml_output << YAML::Key << "Session" << YAML::Value << YAML::BeginMap; // Session
		yaml_output << YAML::Key << "SessionUUID64" << YAML::Value << static_cast<uint64_t>(a_session.m_uuid);
		yaml_output << YAML::Key << "SessionName" << YAML::Value << a_session.m_name;
		
		yaml_output << YAML::Key << "Exchanges" << YAML::Value << YAML::BeginSeq; // Exchanges
		for (const auto& chat_exchange : a_session.m_exchanges) {
			yaml_output << YAML::BeginMap; // Chat Exchange
			yaml_output << YAML::Key << "ExchangeUUID32" << YAML::Value << static_cast<uint32_t>(chat_exchange.m_uuid);
			yaml_output << YAML::Key << "Prompt" << YAML::Value << chat_exchange.m_prompt;
			yaml_output << YAML::Key << "Response" << YAML::Value << chat_exchange.m_response;
			yaml_output << YAML::EndMap; // Chat Exchange
		}
		yaml_output << YAML::EndSeq; // Exchanges
		yaml_output << YAML::EndMap; // Session
		yaml_output << YAML::EndMap; // Start

		std::ofstream yaml_filestream(a_file_path);
		yaml_filestream << yaml_output.c_str();
	}

	SChatSession CChatbotSerializer::DeserializeSessionYAML(const std::string& a_file_path) {
		SChatSession chat_session;
		YAML::Node root = YAML::LoadFile(a_file_path);
		if (auto session_node = root["Session"]) {
			chat_session.m_uuid = CUUID64(session_node["SessionUUID64"].as<uint64_t>());
			chat_session.m_name = session_node["SessionName"].as<std::string>();
			
			if (auto exchanges_node = session_node["Exchanges"]) {
				for (const auto& chat_exchange : exchanges_node) {
					SChatExchange exchange;

					exchange.m_uuid		= CUUID32(chat_exchange["ExchangeUUID32"].as<uint32_t>());
					exchange.m_prompt	= chat_exchange["Prompt"].as<std::string>();
					exchange.m_response = chat_exchange["Response"].as<std::string>();

					chat_session.m_exchanges.push_back(std::move(exchange));
				}
			}
		}

		return chat_session;
	}

	void CChatbotSerializer::SerializeSessionJSON(const SChatSession& a_session, const std::string& a_file_path) {
		nlohmann::json json_output;
		auto& session_node = json_output["Session"];
		session_node["SessionUUID64"] = static_cast<uint64_t>(a_session.m_uuid);
		session_node["SessionName"] = a_session.m_name;

		auto& chat_exchanges = session_node["Exchanges"]; // Exchanges
		for (const auto& exchange : a_session.m_exchanges) {
			nlohmann::json chat_entry;

			chat_entry["ExchangeUUID32"]	= static_cast<uint32_t>(exchange.m_uuid);
			chat_entry["Prompt"]			= exchange.m_prompt;
			chat_entry["Response"]			= exchange.m_response;

			chat_exchanges.push_back(std::move(chat_entry));
		}

		int indent_spacing = 4;
		std::ofstream json_filestream(a_file_path);
		json_filestream << json_output.dump(indent_spacing);
	}

	SChatSession CChatbotSerializer::DeserializeSessionJSON(const std::string& a_file_path) {
		SChatSession chat_session;

		std::ifstream input_file(a_file_path);
		nlohmann::json json_input;
		input_file >> json_input;

		if (json_input.contains("Session")) {
			auto& session_node = json_input["Session"];
			
			chat_session.m_uuid = CUUID64(session_node["SessionUUID64"].get<uint64_t>());
			chat_session.m_name = session_node["SessionName"].get<std::string>();
			
			if (session_node.contains("Exchanges")) {
				for (const auto& chat_entry : session_node["Exchanges"]) {
					SChatExchange exchange;

					exchange.m_uuid		= CUUID32(chat_entry["ExchangeUUID32"].get<uint32_t>());
					exchange.m_prompt	= chat_entry["Prompt"].get<std::string>();
					exchange.m_response	= chat_entry["Response"].get<std::string>();

					chat_session.m_exchanges.push_back(std::move(exchange));
				}
			}
		}

		return chat_session;
	}
}