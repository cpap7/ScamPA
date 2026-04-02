#include <ScamPA/ScamPA.h>
#include <ScamPA/Core/ClientEntryPoint.h>

#include "AppLayer.h"


namespace SPA {
	SPA::CApplication* SPA::CreateApplication(int argc, char** argv) {
		SPA_PROFILE_FUNCTION();

		SPA::SApplicationSpecification spec;
		spec.m_name = "ScamPA App";
		spec.m_width = 1280;
		spec.m_height = 720;
		spec.m_use_custom_titlebar = true;
		spec.m_window_resizeable = true;
		spec.m_center_window = true;

		SPA::CApplication* app = new SPA::CApplication(spec);
		app->PushLayer<SPA::CAppLayer>();
		
		app->SetMenubarCallback([app]() {
			if (ImGui::BeginMenu("File")) {

				if (ImGui::MenuItem("Exit")) {
					app->Close();
				}

				ImGui::EndMenu();
			}
		});
		return app;
	}
}

