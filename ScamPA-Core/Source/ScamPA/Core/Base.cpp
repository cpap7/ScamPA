#include "spapch.h"
#include "Base.h"

#include "Logger.h"

namespace SPA {
	void InitializeCore() {
		SPA_PROFILE_FUNCTION();
		CLogger::Init();

		SPA_CORE_TRACE("ScamPA-Core Engine {0}", SPA_BUILD_VERSION);
		SPA_CORE_TRACE("[ENGINE] Initializing...");
	}

	void ShutdownCore() {
		SPA_PROFILE_FUNCTION();
		SPA_CORE_TRACE("[ENGINE] Shutting down...");
	}
}