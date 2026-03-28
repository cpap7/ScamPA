#pragma once
//#include "Base.h"

#include <string>
#include <memory>
//#include <optional>

namespace SPA {
	class IFileDialogs {
	private:
		static std::unique_ptr<IFileDialogs> s_os_file_dialogs;
	
	public:
		virtual ~IFileDialogs() = default;

		// Empty string returned when cancelled (for windows)
		virtual std::string OpenFile(const char* a_filter = "All.\0*.*\0\0") = 0;
		virtual std::string SaveFile(const char* a_filter = "All.\0*.*\0\0", const char* a_default_extension = "") = 0;
		
		// Factory
		static std::unique_ptr<IFileDialogs> Create();
	};
}
