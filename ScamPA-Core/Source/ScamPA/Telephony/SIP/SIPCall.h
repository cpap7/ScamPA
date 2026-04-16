#pragma once

#include <pjsua2.hpp>
#include <functional>

namespace SPA {
	enum class ESIPCallState : uint8_t {
		Idle = 0,			// No call 
		Ringing,			// During outbound call
		Active,				// Call connected
		Disconnected		// Call disconnected
	};
	
	using CallStateCallback = std::function<void(ESIPCallState a_state)>;

	class CSIPCall : public pj::Call {
	private:
		CallStateCallback m_call_state_callback;
		ESIPCallState m_call_state = ESIPCallState::Idle;
	
	public:
		CSIPCall(pj::Account& a_account, int a_call_id = PJSUA_INVALID_ID);
		~CSIPCall() = default;

		void Dial(const std::string& a_sip_uri); // Makes an outbound call
		void Answer(); 							 // Answers an inbound call (200 OK)
		void HangUp();

		// Accessors
		inline ESIPCallState GetCallState() const										 { return m_call_state;								}
		inline void SetCallStateCallback(const CallStateCallback& a_call_state_callback) { m_call_state_callback = a_call_state_callback;	}

	private: // Internal helpers
		void UpdateCallState(ESIPCallState a_state);

	public: // Overrides
		virtual void onCallState(pj::OnCallStateParam& a_params) override; // Transition table
		virtual void onCallMediaState(pj::OnCallMediaStateParam& a_params) override;
	};

}