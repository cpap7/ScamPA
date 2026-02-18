#include "spapch.h"
#include "Base.h"

#include "Logger.h"

#define SPA_BUILD_VERSION "v0.1a"

namespace SPA {
	void InitializeCore() {
		CLogger::Init();

		SPA_CORE_TRACE("ScamPA-Core Engine {0}", SPA_BUILD_VERSION);
		SPA_CORE_TRACE("[ENGINE] Initializing...");
	}

	void ShutdownCore() {
		SPA_CORE_TRACE("[ENGINE] Shutting down...");
	}
}