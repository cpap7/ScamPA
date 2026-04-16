#include "SIPTab.h"
#include "../../GUIUtilities.h"

#include <ScamPA/Core/Logger.h>

#include <imgui.h>

namespace SPA {

	namespace Utilities {
		std::string ScamPACallStateToString(ESIPCallState a_state) {
			switch (a_state) {
				case ESIPCallState::Idle:			return "Idle";
				case ESIPCallState::Ringing:		return "Ringing";
				case ESIPCallState::Active:			return "Active";
				case ESIPCallState::Disconnected:	return "Disconnected";
				default: break;
			}

			return "Idle";
		}

		ImVec4 ScamPACallStateToColor(ESIPCallState a_state) {
			switch (a_state) {
				case ESIPCallState::Idle:			return ImVec4(1, 1, 1, 1);
				case ESIPCallState::Ringing:		return ImVec4(1, 1, 0, 1);
				case ESIPCallState::Active:			return ImVec4(0, 1, 0, 1);
				case ESIPCallState::Disconnected:	return ImVec4(1, 0, 0, 1);
				default: break;
			}

			return ImVec4(1, 1, 1, 1);
		}
	}


	CSIPTab::CSIPTab() {
		OnInit();
	}

	CSIPTab::~CSIPTab() {
		OnShutdown();
	}

	void CSIPTab::OnInit() {

	}

	void CSIPTab::OnShutdown() {
		// Reverse/LIFO order
		if (m_sip_call) {
			m_sip_call->HangUp();
			m_sip_call.reset();
		}

		if (m_sip_account) {
			m_sip_account->Unregister();
			m_sip_account.reset();
			m_account_registered = false;
		}

		if (m_sip_endpoint) {
			m_sip_endpoint->Shutdown();
			m_sip_endpoint.reset();
			m_endpoint_initialized = false;
		}
	}

	void CSIPTab::OnTabRender() {
		DisplayEndpointControls();
		DisplayAccountControls();
		DisplayCallControls();
		DisplayStatusInfo();
	}

	void CSIPTab::DisplayEndpointControls() {
		if (GUI::BeginTreeNode("Endpoint Settings", false)) {
			if (!m_endpoint_initialized) {
				// TODO: Make this better
				static const char* transport_labels[] = { "UDP", "TCP" };
				ImGui::SetNextItemWidth(120);
				ImGui::Combo("Transport Type", &m_selected_transport_type, transport_labels, IM_ARRAYSIZE(transport_labels));
				ImGui::SetNextItemWidth(120);

				// TODO: use uint16_t maybe? (wrap-around)
				ImGui::InputInt("Port (5060 = default)", &m_port);
				if (m_port < 0) {
					m_port = 0;
				}
				if (m_port > 65535) {
					m_port = 65535;
				}

				if (ImGui::Button("Setup Endpoint")) {
					SetupEndpoint();
				}
			}
			else {
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "Endpoint Active");
				ImGui::SameLine();
				if (ImGui::Button("Shutdown")) {
					ShutdownEndpoint();
				}
			}

			GUI::EndTreeNode();
		}
	}

	void CSIPTab::SetupEndpoint() {
		m_sip_endpoint = std::make_unique<CSIPEndpoint>();
		m_sip_endpoint->Init();
		m_sip_endpoint->CreateTransport(static_cast<ESIPTransportType>(m_selected_transport_type), static_cast<uint16_t>(m_port));
		m_endpoint_initialized = m_sip_endpoint->IsInitialized();
	}

	void CSIPTab::ShutdownEndpoint() {
		OnShutdown(); // Shuts down everything
	}

	void CSIPTab::DisplayAccountControls() {
		if (!m_endpoint_initialized) {
			return;
		}
		if (GUI::BeginTreeNode("Account Settings", false)) {
			if (!m_account_registered) {
				ImGui::TextDisabled("Domain");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(-1);
				ImGui::InputText("##sip_domain", m_domain_buf, sizeof(m_domain_buf));

				ImGui::TextDisabled("Display Name");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(-1);
				ImGui::InputText("##sip_displayname", m_display_name_buf, sizeof(m_display_name_buf));

				ImGui::TextDisabled("Username");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(-1);
				ImGui::InputText("##sip_username", m_username_buf, sizeof(m_username_buf));

				// TODO: investigate hashing w/ pjsip
				ImGui::TextDisabled("Password");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(-1);
				ImGui::InputText("##sip_password", m_password_buf, sizeof(m_password_buf), ImGuiInputTextFlags_Password);

				// NOTE: Display name is optional
				bool fields_filled = strlen(m_domain_buf) > 0 && strlen(m_username_buf) > 0 && strlen(m_password_buf) > 0;
				if (!fields_filled) { ImGui::BeginDisabled(); }

				if (ImGui::Button("Register")) {
					RegisterAccount();
				}

				if (!fields_filled) { ImGui::EndDisabled(); }
			}
			else {
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "Registered: %s@%s", m_username_buf, m_domain_buf);
				ImGui::SameLine();
				if (ImGui::Button("Unregister")) {
					UnregisterAccount();
				}
			}

			GUI::EndTreeNode();
		}
	}

	void CSIPTab::RegisterAccount() {
		m_sip_account = std::make_unique<CSIPAccount>();
		CSIPAccount::SSIPAccountConfig account_config;
		account_config.m_sip_service_provider_domain	= m_domain_buf;
		account_config.m_display_name					= m_display_name_buf;
		account_config.m_username						= m_username_buf;
		account_config.m_password						= m_password_buf;

		m_sip_account->Register(account_config);
		m_account_registered = true;
	}

	void CSIPTab::UnregisterAccount() {
		if (m_sip_call) {
			m_sip_call->HangUp();
			m_sip_call.reset();
		}

		m_sip_account->Unregister();
		m_sip_account.reset();
		m_account_registered = false;
	}

	void CSIPTab::DisplayCallControls() {
		if (!m_account_registered) {
			return;
		}

		if (GUI::BeginTreeNode("Call Settings")) {
			bool is_active = m_sip_call != nullptr;
			ESIPCallState call_state = is_active ? m_sip_call->GetCallState() : ESIPCallState::Idle;
	
			if (!is_active || call_state == ESIPCallState::Disconnected) {
				if (call_state == ESIPCallState::Disconnected) {
					m_sip_call.reset();
				}

				ImGui::TextDisabled("SIP URI");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(-1);
				ImGui::InputText("##dial_uri", m_dial_uri_buf, sizeof(m_dial_uri_buf));

				bool uri_filled = strlen(m_dial_uri_buf) > 0;
				if (!uri_filled) { ImGui::BeginDisabled(); }

				if (ImGui::Button("Dial")) {
					m_sip_call = std::make_unique<CSIPCall>(*m_sip_account);
					m_sip_call->Dial(m_dial_uri_buf);
				}

				if (!uri_filled) { ImGui::EndDisabled(); }
			}
			else {
				std::string state_string = Utilities::ScamPACallStateToString(call_state);
				ImVec4 state_color = (call_state == ESIPCallState::Active) ? ImVec4(0, 1, 0, 1) : ImVec4(1, 1, 0, 1);

				ImGui::TextColored(state_color, "Call State: %s", state_string.c_str());
				if (ImGui::Button("Hang Up")) {
					m_sip_call->HangUp();
				}
			}

			GUI::EndTreeNode();
		}
	}
	
	void CSIPTab::DisplayStatusInfo() {
		if (GUI::BeginTreeNode("Status", false)) {
			ImGui::Text("Endpoint: %s", m_endpoint_initialized ? "Initialized" : "Not Initialized");
			ImGui::Text("Account: %s", m_account_registered ? "Registered" : "Not Registered");
			ImGui::Text("Call: %s", m_sip_call ? Utilities::ScamPACallStateToString(m_sip_call->GetCallState()).c_str() : "Not Initialized");

			GUI::EndTreeNode();
		}
	}

}