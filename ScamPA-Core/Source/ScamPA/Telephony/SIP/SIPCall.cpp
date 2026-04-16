#include "spapch.h"
#include "SIPCall.h"

namespace SPA {
	
	namespace Utilities {
		// For use by Core logger
		std::string ScamPACallStateToStringInternal(ESIPCallState a_state) {
			switch (a_state) {
				case ESIPCallState::Idle:			return "IDLE";
				case ESIPCallState::Ringing:		return "RINGING";
				case ESIPCallState::Active:			return "ACTIVE";
				case ESIPCallState::Disconnected:	return "DISCONNECTED";
			}

			return "UNKNOWN";
		}
	}


	CSIPCall::CSIPCall(pj::Account& a_account, int a_call_id) 
		: pj::Call(a_account, a_call_id) {
	}

	void CSIPCall::Dial(const std::string& a_sip_uri) {
		try {
			pj::CallOpParam op_param(true); // True = use default call settings
			makeCall(a_sip_uri, op_param);

			UpdateCallState(ESIPCallState::Ringing);
		}
		catch (pj::Error& exception) {
			SPA_CORE_ERROR("(SIP) Dial error: {0}", exception.info());
			return;
		}

		SPA_CORE_INFO("(SIP) Dialing: {0}", a_sip_uri);
	}
	
	void CSIPCall::Answer() {
		try {
			pj::CallOpParam op_param;
			op_param.statusCode = PJSIP_SC_OK;
			answer(op_param);
		}
		catch (pj::Error& exception) {
			SPA_CORE_ERROR("(SIP) Call answer error: {0}", exception.info());
			return;
		}

		SPA_CORE_INFO("(SIP) Call answered");
	}

	void CSIPCall::HangUp() {
		try {
			pj::CallOpParam op_param;
			op_param.statusCode = PJSIP_SC_DECLINE;
			hangup(op_param);
		}
		catch (pj::Error& exception) {
			SPA_CORE_ERROR("(SIP) Call hang up error: {0}", exception.info());
			return;
		}

		SPA_CORE_INFO("(SIP) Call hung up");
	}

	void CSIPCall::UpdateCallState(ESIPCallState a_state) {
		SPA_CORE_INFO("(SIP) Transitioning call state: {0} -> {1}", Utilities::ScamPACallStateToStringInternal(m_call_state), Utilities::ScamPACallStateToStringInternal(a_state));
		m_call_state = a_state;

		if (m_call_state_callback) {
			m_call_state_callback(m_call_state);
		}
	}

	void CSIPCall::onCallState(pj::OnCallStateParam& a_params) {
		pj::CallInfo call_info = getInfo();

		switch (call_info.state) {
			case PJSIP_INV_STATE_CALLING:
			case PJSIP_INV_STATE_EARLY:			UpdateCallState(ESIPCallState::Ringing);		break;
			case PJSIP_INV_STATE_CONFIRMED:		UpdateCallState(ESIPCallState::Active);			break;
			case PJSIP_INV_STATE_DISCONNECTED:	UpdateCallState(ESIPCallState::Disconnected);	break;
			default: break;
		}
		SPA_CORE_INFO("(SIP) PJ call state: {0} (code = {1})", call_info.stateText, static_cast<int>(call_info.state));
	}

	void CSIPCall::onCallMediaState(pj::OnCallMediaStateParam& a_params) {
		pj::CallInfo call_info = getInfo();

		for (size_t i{}; i < call_info.media.size(); i++) {
			if (call_info.media[i].type == PJMEDIA_TYPE_AUDIO && call_info.media[i].status == PJSUA_CALL_MEDIA_ACTIVE) {
				pj::AudioMedia* audio = static_cast<pj::AudioMedia*>(getMedia(i));

				if (!audio) {
					continue;
				}

#if 1
				// TODO: Connect to CSIPAudioBridge, rather than default device
				// This is just for initial testing - routes to default speaker/mic
				pj::AudDevManager& audio_device_manager = pj::Endpoint::instance().audDevManager();
				audio->startTransmit(audio_device_manager.getPlaybackDevMedia());
				audio_device_manager.getCaptureDevMedia().startTransmit(*audio);

				SPA_CORE_INFO("(SIP) Audio media connected (index = {0})", i);
#endif
			}
		}
	}
}