#pragma once
#include "../Debug/Instrumentor.h"

#ifdef SPA_PLATFORM_WINDOWS

	bool g_application_running = true;

	namespace SPA {
		// Implemented by CLIENT apps
		extern CApplication* CreateApplication(int argc, char** argv);
	
		int main(int argc, char** argv) {
			while (g_application_running) {
				// Initialization
				SPA_PROFILE_BEGIN_SESSION("Startup", "SPAProfile-Startup.json");
				SPA::InitializeCore();
				SPA::CApplication* app = SPA::CreateApplication(argc, argv);
				SPA_PROFILE_END_SESSION();
				
				// Runtime
				SPA_PROFILE_BEGIN_SESSION("Runtime", "SPAProfile-Runtime.json");
				app->Run();
				SPA_PROFILE_END_SESSION();

				// Shutdown
				SPA_PROFILE_BEGIN_SESSION("Shutdown", "SPAProfile-Shutdown.json");
				delete app;
				SPA::ShutdownCore();
				SPA_PROFILE_END_SESSION();

			}

			return 0;
		}

	}

	#ifdef SPA_DIST

		#include <Windows.h>

		int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
			return SPA::main(__argc, __argv);
		}

	#else

		int main(int argc, char** argv) {
			return SPA::main(argc, argv);
		}

	#endif // SPA_DIST

#endif // SPA_PLATFORM_WINDOWS
