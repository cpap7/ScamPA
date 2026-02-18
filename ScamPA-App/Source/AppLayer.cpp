#include "AppLayer.h"

#include <ScamPA/ScamPA.h>

namespace SPA {
	CAppLayer::CAppLayer() {

	}

	CAppLayer::~CAppLayer() {
	
	}

	void CAppLayer::OnAttach() {

	}
	void CAppLayer::OnDetach() {

	}

	void CAppLayer::OnUpdate(float a_timestep) {

	}

	void CAppLayer::OnUIRender() {
		ImGui::Begin("Hello");
		ImGui::Button("Button");
		ImGui::End();

		ImGui::ShowDemoWindow();
	}

	void CAppLayer::OnEvent(IEvent& a_event) {

	}
}