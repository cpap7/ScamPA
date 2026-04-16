#include "spapch.h"
#include "SIPAccount.h"

namespace SPA {

	void CSIPAccount::onRegState(pj::OnRegStateParam& a_params) {
		pj::AccountInfo info = getInfo();
		SPA_CORE_INFO("(SIP) Registration state: {0} (code = {1})", info.regIsActive ? "Active" : "Inactive", a_params.code);
	}

	void CSIPAccount::onIncomingCall(pj::OnIncomingCallParam& a_params) {
		SPA_CORE_INFO("(SIP) Incoming call (id = {0})", a_params.callId);

		if (m_incoming_call_callback) {
			m_incoming_call_callback(a_params);
		}
	}

	void CSIPAccount::Register(const SSIPAccountConfig& a_config) {
		m_account_config = a_config; // Update internal config
		pj::AccountConfig pj_account_config;

		try {
			// TODO: display name --> Name Example <sip:username@domain.com>
			pj_account_config.idUri					 = "sip:" + a_config.m_username + "@" + a_config.m_sip_service_provider_domain;
			pj_account_config.regConfig.registrarUri = "sip:" + a_config.m_sip_service_provider_domain;
			
			pj::AuthCredInfo cred("digest", "*", a_config.m_username, 0, a_config.m_password);
			pj_account_config.sipConfig.authCreds.push_back(cred);

			create(pj_account_config); // No default account (for now)
		}
		catch (pj::Error& exception) {
			SPA_CORE_ERROR("(SIP) Registration failed: {0}", exception.info());
			return;
		}

		SPA_CORE_INFO("(SIP) Account registered: {0}", pj_account_config.idUri);	
	}

	void CSIPAccount::Unregister() {
		if (isValid()) {
			//pj::AccountInfo info = getInfo();

			shutdown();

			SPA_CORE_INFO("(SIP) Account unregistered: {0}@{1}", m_account_config.m_username, m_account_config.m_sip_service_provider_domain);
		}
	}
}