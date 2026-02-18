#pragma once

namespace SPA {
	enum class EOperatingSystemType {
		None	= 0,
		Windows = 1,
		MacOS	= 2,
		Linux	= 3
	};

	enum class EBuildConfigurationType {
		None			= 0,
		Debug			= 1,
		Release			= 2,
		Distribution	= 3
	};

	class CPlatformInfo {
	private:
		friend class CApplication;

	private:
		static EOperatingSystemType s_os_platform;
		static EBuildConfigurationType s_build_configuration;

	public:
		inline static EOperatingSystemType		GetOSPlatform() { return s_os_platform; }
		inline static EBuildConfigurationType	GetBuildConfiguration() { return s_build_configuration; }

	};
}
