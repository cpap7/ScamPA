#include <ScamPA/ScamPA.h>
#include <ScamPA/Core/ClientEntryPoint.h>

#include "AppLayer.h"


namespace SPA {
	SPA::CApplication* SPA::CreateApplication(int argc, char** argv) {
		SPA_PROFILE_FUNCTION();

		SPA::SApplicationSpecification spec;
		spec.m_name					= "ScamPA App";
		spec.m_icon_path			= "Assets/Resources/scampa_logo_circle.png";
		spec.m_width				= 1600;
		spec.m_height				= 900;
		spec.m_use_custom_titlebar	= true;
		spec.m_window_resizeable	= true;
		spec.m_center_window		= true;

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

