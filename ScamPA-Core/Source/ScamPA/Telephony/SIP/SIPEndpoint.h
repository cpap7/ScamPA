#pragma once
#include <pjsua2.hpp>

namespace SPA {
	
	enum class ESIPTransportType : uint8_t {
		UDP = 0,
		TCP = 1
	};

	// Wrapper for PJSIP's endpoint data structure (pj::Endpoint)
	// Single instance (or singleton) managing global PJSIP lifecycle
	// Owns SIP transport & global PJSIP config
	class CSIPEndpoint {
	private:
		pj::Endpoint m_endpoint;
		bool m_is_initialized = false;

	public:
		CSIPEndpoint() = default;
		~CSIPEndpoint();

		inline CSIPEndpoint(const CSIPEndpoint& a_other) = delete;
		inline void operator=(const CSIPEndpoint& a_other) = delete;

		void Init();
		void Shutdown();
		void Reinit();

		// Creates either UDP or TCP transport on a given port (where 5060 = default)
		void CreateTransport(ESIPTransportType a_type = ESIPTransportType::UDP, uint16_t a_port = 5060);

		inline pj::Endpoint& GetEndpoint()	{ return m_endpoint;		}
		inline bool IsInitialized() const	{ return m_is_initialized;	}

	};

}
