#include "spapch.h" // For PlatformDetection.h

#include "PlatformInfo.h"

// Operating system config
#ifdef SPA_PLATFORM_WINDOWS
namespace SPA {
	EOperatingSystemType CPlatformInfo::s_os_platform = EOperatingSystemType::Windows;
}

#elif SPA_PLATFORM_MACOS
namespace SPA {
	EOperatingSystemType CPlatformInfo::s_os_platform = EOperatingSystemType::MacOS;
}

#elif SPA_PLATFORM_LINUX
namespace SPA {
	EOperatingSystemType CPlatformInfo::s_os_platform = EOperatingSystemType::Linux;
}

#else
namespace SPA {
	EOperatingSystemType CPlatformInfo::s_os_platform = EOperatingSystemType::None;
}

#endif

// Build config
#ifdef SPA_DEBUG
namespace SPA {
	EBuildConfigurationType CPlatformInfo::s_build_configuration = EBuildConfigurationType::Debug;
}

#elif SPA_RELEASE
namespace SPA {
	EBuildConfigurationType CPlatformInfo::s_build_configuration = EBuildConfigurationType::Release;
}

#elif SPA_DIST
namespace SPA {
	EBuildConfigurationType CPlatformInfo::s_build_configuration = EBuildConfigurationType::Distribution;
}

#else
namespace SPA {
	EBuildConfigurationType CPlatformInfo::s_build_configuration = EBuildConfigurationType::None;
}

#endif

