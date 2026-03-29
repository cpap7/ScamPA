#pragma once
#include "../AIAgent/AIEngineManager.h"
#include "ChatComponents.h"

#include <string>
#include <cstdint>

namespace SPA {
	enum class ESerializationFormat : uint8_t {
		YAML = 0,
		JSON
	};

	class CChatbotSerializer {
	private:
		CAIEngineManager& m_manager;

	public:
		explicit CChatbotSerializer(CAIEngineManager& a_manager);
		
		// Session serialization/deserialization entry point functions
		void SerializeSession(const SChatSession& a_session, const std::string& a_file_path, ESerializationFormat a_format = ESerializationFormat::YAML);
		SChatSession DeserializeSession(const std::string& a_file_path, ESerializationFormat a_format = ESerializationFormat::YAML);

		// AI context snapshots (raw binaries)
		void SerializeAIContextSnapshot(const std::string& a_file_path);
		void DeserializeAIContextSnapshot(const std::string& a_file_path);

	private:
		// YAML
		void SerializeSessionYAML(const SChatSession& a_session, const std::string& a_file_path);
		SChatSession DeserializeSessionYAML(const std::string& a_file_path);

		// JSON
		void SerializeSessionJSON(const SChatSession& a_session, const std::string& a_file_path);
		SChatSession DeserializeSessionJSON(const std::string& a_file_path);

	};
}

