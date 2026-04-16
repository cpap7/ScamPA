#pragma once
#include <pjsua2.hpp>

#include <functional>

namespace SPA {

	// Forward declarations
	class CSIPCall;

	using IncomingCallCallback = std::function<void(pj::OnIncomingCallParam& a_params)>;

	// Used for SIP registration + incoming call dispatch
	// TODO: Instant messaging callback + overrides
	class CSIPAccount : public pj::Account {
	public:
		struct SSIPAccountConfig {
			std::string m_sip_service_provider_domain;		// e.g., "sip.twilio.com"
			std::string m_display_name;
			std::string m_username;
			std::string m_password;
		};

	private:
		SSIPAccountConfig m_account_config;
		IncomingCallCallback m_incoming_call_callback;

	public:
		CSIPAccount() = default;
		~CSIPAccount() = default;

	public: // // PJSIP overrides 
		// Called by PJSIP when calls arrive
		virtual void onRegState(pj::OnRegStateParam& a_params) override;
		virtual void onIncomingCall(pj::OnIncomingCallParam& a_params) override;

	public:
		void Register(const SSIPAccountConfig& a_config);
		void Unregister();

		inline void SetIncomingCallCallback(const IncomingCallCallback& a_callback) { m_incoming_call_callback = a_callback; }
		inline void SetAccountConfig(const SSIPAccountConfig& a_config)				{ m_account_config = a_config;			 }
		inline const SSIPAccountConfig& GetAccountConfig() const					{ return m_account_config;				 }

	};
}

