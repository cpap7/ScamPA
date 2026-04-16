#pragma once

#include <string>

namespace SPA {
	class IPanel { // Panel/Window
	public:
		virtual ~IPanel() = default;

		virtual void OnInit() {}
		virtual void OnShutdown() {}
		virtual void OnUIRender() {}
	};

	class ITab { // Component of panel 
	public:
		virtual ~ITab() = default;

		virtual const std::string& GetTabName() const = 0;

		virtual void OnInit() {}
		virtual void OnShutdown() {}
		virtual void OnTabRender() {}
	};
}