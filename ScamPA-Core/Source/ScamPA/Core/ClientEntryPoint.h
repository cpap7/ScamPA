#pragma once

#ifdef SPA_PLATFORM_WINDOWS

bool g_application_running = true;



namespace SPA {
	// Implemented by CLIENT apps
	extern CApplication* CreateApplication(int argc, char** argv);

	int main(int argc, char** argv) {
		while (g_application_running) {
			// Setup
			SPA::InitializeCore();
			SPA::CApplication* app = SPA::CreateApplication(argc, argv);
			app->Run();

			// Cleanup
			delete app;
			SPA::ShutdownCore();
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
