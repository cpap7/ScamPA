#pragma once
#include "../../Core/FileDialogs.h"

namespace SPA {
	class CWindowsFileDialogs : public IFileDialogs {
	public:
		// Empty string is returned when cancelled (at least for Windows)
		virtual std::string OpenFile(const char* a_filter = "All.\0*.*\0\0") override;
		virtual std::string SaveFile(const char* a_filter = "All.\0*.*\0\0", const char* a_default_extension = "") override;
	};

}
