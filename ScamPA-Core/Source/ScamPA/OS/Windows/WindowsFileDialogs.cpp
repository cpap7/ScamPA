#include "spapch.h"
#include "WindowsFileDialogs.h"
#include "../../Core/Application.h"

#include <Windows.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace SPA {
	std::string CWindowsFileDialogs::OpenFile(const char* a_filter) {
		OPENFILENAMEA ofn; // Dialog box structure
		CHAR sz_file[260] = { 0 }; // If using TCHAR macros

		// Save current working directory
		CHAR saved_cwd[MAX_PATH] = { 0 };
		GetCurrentDirectoryA(MAX_PATH, saved_cwd);

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)CApplication::GetApplicationInstance().GetWindowHandle().GetNativeWindow());
		ofn.lpstrFile = sz_file;
		ofn.nMaxFile = sizeof(sz_file);
		ofn.lpstrFilter = a_filter;
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE) {
			SetCurrentDirectoryA(saved_cwd);
			return ofn.lpstrFile;
		}
		return std::string();
	}

	std::string CWindowsFileDialogs::SaveFile(const char* a_filter, const char* a_default_extension) {
		OPENFILENAMEA ofn; // Dialog box structure
		CHAR sz_file[260] = { 0 }; // If using TCHAR macros

		// Save current working directory
		CHAR saved_cwd[MAX_PATH] = { 0 };
		GetCurrentDirectoryA(MAX_PATH, saved_cwd);

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)CApplication::GetApplicationInstance().GetWindowHandle().GetNativeWindow());
		ofn.lpstrFile = sz_file;
		ofn.nMaxFile = sizeof(sz_file);
		ofn.lpstrFilter = a_filter;
		ofn.nFilterIndex = 1;
		ofn.lpstrDefExt = a_default_extension; // I.e., if a_default_extension = "sasc" --> .sasc
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetSaveFileNameA(&ofn) == TRUE) {
			SetCurrentDirectoryA(saved_cwd);
			return ofn.lpstrFile;
		}
		return std::string();
	}
}