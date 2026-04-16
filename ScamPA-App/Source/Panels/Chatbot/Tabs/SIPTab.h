#pragma once
#include <ScamPA/Core/GUIInterfaces.h>

#include <ScamPA/Telephony/SIP/SIPEndpoint.h>
#include <ScamPA/Telephony/SIP/SIPAccount.h>
#include <ScamPA/Telephony/SIP/SIPCall.h>

#include <memory>
#include <string>

namespace SPA {

	class CSIPTab : public ITab {
	private:
		// Dial URI input buffer
		char m_dial_uri_buf[256]		= "sip:echo@sip2sip.info";

		// Account config input buffers
		char m_domain_buf[256]			= "sip.twilio.com";
		char m_display_name_buf[128]	= "";
		char m_username_buf[128]		= "";
		char m_password_buf[128]		= "";
		
		// Interface-related
		std::string m_name = "SIP Settings";

		// SIP stack
		std::unique_ptr<CSIPEndpoint> m_sip_endpoint	= nullptr;
		std::unique_ptr<CSIPAccount> m_sip_account		= nullptr;
		std::unique_ptr<CSIPCall> m_sip_call			= nullptr;

		// Endpoint config
		int m_port = 5060; // Default
		int m_selected_transport_type = 0; // UDP = 0, TCP = 1
		//ESIPTransportType m_transport_type = ESIPTransportType::UDP;


		// State tracking
		bool m_endpoint_initialized = false;
		bool m_account_registered   = false;

	public:
		CSIPTab();
		~CSIPTab();

		inline virtual const std::string& GetTabName() const override { return m_name; }

		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnTabRender() override;

	private:
		void DisplayEndpointControls();
		void SetupEndpoint();
		void ShutdownEndpoint();

		void DisplayAccountControls();
		void RegisterAccount();
		void UnregisterAccount();
		
		void DisplayCallControls();

		void DisplayStatusInfo();
	};

}