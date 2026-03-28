#include "spapch.h"
#include "FileDialogs.h"
#include "PlatformDetection.h"

// Type is dependent on OS, meaning it'll be deduced at compile-time
#ifdef SPA_PLATFORM_WINDOWS
#include "../OS/Windows/WindowsFileDialogs.h"

namespace SPA {
	std::unique_ptr<IFileDialogs> IFileDialogs::Create() {
		return std::make_unique<CWindowsFileDialogs>();
	}
}

#elif SPA_PLATFORM_MACOS
// TODO: remove #error once added
#error MacOS backend for file dialog creation has not been implemented yet
#include "../OS/MacOS/MacOSFileDialogs.h"

namespace SPA {
	std::shared_ptr<IFileDialogs> IFileDialogs::Create() {
		return std::make_shared<CMacOSFileDialogs>();
	}
}

#elif SPA_PLATFORM_LINUX
// TODO: remove #error once added
#error Linux backend for file dialog creation has not been implemented yet
#include "../OS/Linux/LinuxFileDialogs.h"

namespace SPA {
	std::shared_ptr<IFileDialogs> IFileDialogs::Create() {
		return std::make_shared<CLinuxFileDialogs>();
	}
}

#endif
