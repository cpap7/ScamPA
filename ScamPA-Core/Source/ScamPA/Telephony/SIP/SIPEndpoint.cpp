#include "spapch.h"
#include "SIPEndpoint.h"

namespace SPA {

	namespace Utilities {
		ESIPTransportType PJSIPTransportTypeToScamPAType(pjsip_transport_type_e a_type) {
			switch (a_type) {
				case PJSIP_TRANSPORT_UDP:		return ESIPTransportType::UDP;
				case PJSIP_TRANSPORT_TCP:		return ESIPTransportType::TCP;
			}

			return ESIPTransportType::UDP;
		}

		pjsip_transport_type_e ScamPATransportTypeToPJSIPType(ESIPTransportType a_type) {
			switch (a_type) {
				case ESIPTransportType::UDP:	return PJSIP_TRANSPORT_UDP;
				case ESIPTransportType::TCP:	return PJSIP_TRANSPORT_TCP;
			}

			return PJSIP_TRANSPORT_UDP;
		}

		std::string ScamPATransportTypeToString(ESIPTransportType a_type) {
			switch (a_type) {
				case ESIPTransportType::UDP:	return "UDP";
				case ESIPTransportType::TCP:	return "TCP";
			}

			return "UNKNOWN";
		}

		std::string PJSIPTransportTypeToString(pjsip_transport_type_e a_type) {
			switch (a_type) {
				case PJSIP_TRANSPORT_UDP:		return "UDP";
				case PJSIP_TRANSPORT_TCP:		return "TCP";
			}

			return "UNKNOWN";
		}
	}
	
	CSIPEndpoint::~CSIPEndpoint() {
		Shutdown();
	}

	void CSIPEndpoint::Init() {
		if (m_is_initialized) {
			return;
		}
		
		try {
			// Setup endpoint
			m_endpoint.libCreate();
			
			pj::EpConfig ep_config;
			ep_config.logConfig.level = 4; // PJSIP internal log verbosity
			ep_config.logConfig.consoleLevel = 0; // PJSIP console output is suppressed since it's routed via logger
			
			// For NAT traversal -- we resolve public IP via STUN
			ep_config.uaConfig.stunServer.push_back("stun.l.google.com:19302");

			m_endpoint.libInit(ep_config);
			m_endpoint.libStart();
			
		}
		catch (pj::Error& exception) {
			SPA_CORE_ERROR("(SIP) Endpoint initialization error: {0}", exception.info());
			return;
		}

		m_is_initialized = true;
		SPA_CORE_INFO("(SIP) Endpoint initialized");
	}

	void CSIPEndpoint::Shutdown() {
		if (!m_is_initialized) {
			return;
		}

		try {
			m_endpoint.libDestroy();
		}
		catch (pj::Error& exception) {
			SPA_CORE_ERROR("(SIP) Endpoint shutdown error: {0}", exception.info());
			//return;
		}
		m_is_initialized = false;
		SPA_CORE_INFO("(SIP) Endpoint shutdown complete");
	}
	
	void CSIPEndpoint::Reinit() {
		Shutdown();
		Init();
	}

	void CSIPEndpoint::CreateTransport(ESIPTransportType a_type, uint16_t a_port) {

		try {
			pjsip_transport_type_e transport_type = Utilities::ScamPATransportTypeToPJSIPType(a_type);
			pj::TransportConfig transport_config;
			transport_config.port = a_port;

			m_endpoint.transportCreate(transport_type, transport_config);
		}
		catch (pj::Error& exception) {
			SPA_CORE_ERROR("(SIP) Transport creation error: {0}", exception.info());
			return;
		}

		SPA_CORE_INFO("(SIP) Transport created on port {0}, type = {1}", a_port, Utilities::ScamPATransportTypeToString(a_type));
	}

}